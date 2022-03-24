#include <stdbool.h>
#include "mcc_generated_files/pin_manager.h"
#include "global_constants.h"
#include "torque_handling.h"
#include "configuration_variables.h"
#include "mcc_generated_files/tmr1.h"
#include "car_data.h"

static void check_apps_and_brakes_plausibility();
static void check_25_5_plausibility();
static void set_brake_state();
static void set_accelerator_state();
static void calculate_torque_request();

extern struct Car_Data car_data;

static double scaled_torque_limit = 0;    // stores the maximum torque for the current drive mode

static const uint16_t APPS1_25_PERCENT = (APPS1_REAL_MAX - APPS1_REAL_MIN) / 4 + APPS1_REAL_MIN;    // used for 25/5 plausibility check
static const uint16_t APPS1_5_PERCENT = (APPS1_REAL_MAX - APPS1_REAL_MIN) / 20 + APPS1_REAL_MIN;    // used for 25/5 plausibility check

static const uint16_t APPS_10PERCENT_PLAUSIBILITY = (APPS1_REAL_MAX - APPS1_REAL_MIN) / 10;

static const uint16_t RAW_APPS2_MIN_RANGE = CALC_APPS2_FROM_APPS1(APPS1_MIN_RANGE);         // lowest plausible accelerator position, anything lower indicates an error
static const uint16_t RAW_APPS2_REAL_MIN = CALC_APPS2_FROM_APPS1(APPS1_REAL_MIN);           // this is actually what the pedal reading is when it is not pressed
static const uint16_t RAW_APPS2_ACCEL_START = CALC_APPS2_FROM_APPS1(APPS1_ACCEL_START);     // this is where the ECU begins to request torque
static const uint16_t RAW_APPS2_WIDE_OPEN_THROTTLE = CALC_APPS2_FROM_APPS1(APPS1_WIDE_OPEN_THROTTLE);  // point for wide open throttle
static const uint16_t RAW_APPS2_REAL_MAX = CALC_APPS2_FROM_APPS1(APPS1_REAL_MAX);           // this is actually what is the pedal reading when it is fully pressed
static const uint16_t RAW_APPS2_MAX_RANGE = CALC_APPS2_FROM_APPS1(APPS1_MAX_RANGE);         // highest plausible accelerator position, anything higher indicates an error
static uint16_t RAW_APPS2_25_PERCENT;                                                       // used for 25/5 plausibility check
static uint16_t RAW_APPS2_5_PERCENT;                                                        // used for 25/5 plausibility check

static volatile bool is_plausible = true;
static volatile bool is_100_ms_plausible = true;

static volatile bool message_received = false;
static uint8_t message_not_received_count = 0;

static volatile uint16_t apps_1;
static volatile uint16_t raw_apps_2;
static volatile uint16_t scaled_apps_2;
static volatile uint16_t brake_1;
static volatile uint16_t brake_2;

static volatile uint8_t inverter_cmd_data[8];

static volatile int16_t torque_times_ten;

void initialize_apps_2()
{
    RAW_APPS2_25_PERCENT = (RAW_APPS2_REAL_MAX - RAW_APPS2_REAL_MIN) / 4 + RAW_APPS2_REAL_MIN;
    RAW_APPS2_5_PERCENT = (RAW_APPS2_REAL_MAX - RAW_APPS2_REAL_MIN) / 20 + RAW_APPS2_REAL_MIN;
}

void initialize_inverter_cmd_data()
{
    inverter_cmd_data[0] = 0;   // low order torque request byte
    inverter_cmd_data[1] = 0;   // high order torque request byte
    inverter_cmd_data[2] = 0;   // low order speed request byte (unused)
    inverter_cmd_data[3] = 0;   // high order speed request byte (unused)
    inverter_cmd_data[4] = 1;   // Boolean direction bit. 1 = forward, 0 = reverse
    inverter_cmd_data[5] = 0;   // 5.0: inverter enable, 0 for off. 5.1: discharge by sending current through motor (not used), 0 for disable. 5.2: 0 for torque mode
    inverter_cmd_data[6] = (TORQUE_MAX * 10) & 0xFF;            // low order torque limit byte
    inverter_cmd_data[7] = ((TORQUE_MAX * 10) >> 8) & 0xFF;     // high order torque limit byte
}

void set_pedal_position_data(uint16_t new_apps_1, uint16_t new_raw_apps_2, uint16_t new_brake_1, uint16_t new_brake_2)
{
    message_received = true;
    
    apps_1 = new_apps_1;
    
    raw_apps_2 = new_raw_apps_2;
    scaled_apps_2 = CALC_SCALED_APPS2(new_raw_apps_2);

    brake_1 = new_brake_1;
    brake_2 = new_brake_2;
    
    set_brake_state();
    set_accelerator_state();
    check_apps_and_brakes_plausibility();
    check_25_5_plausibility();
}

void check_apps_and_brakes_plausibility()
{
    // the below statement should be true when implausible.
    if (apps_1 < (scaled_apps_2 - APPS_10PERCENT_PLAUSIBILITY) || apps_1 > (scaled_apps_2 + APPS_10PERCENT_PLAUSIBILITY) // check for 10% Implausibility
        || apps_1 < APPS1_MIN_RANGE || apps_1 > APPS1_MAX_RANGE             // check for APPS1 out of range
        || raw_apps_2 < RAW_APPS2_MIN_RANGE || raw_apps_2 > RAW_APPS2_MAX_RANGE     // check for APPS2 out of range
        || brake_1 < BRK1_MIN_RANGE || brake_1 > BRK1_MAX_RANGE             // check for brake_1 out of range
        || brake_2 < BRK2_MIN_RANGE || brake_2 > BRK2_MAX_RANGE)            // check for brake_2 out of range
    {
        // first implausible reading, start TMR1 which will call trigger_100_ms_implausibility() 
        // if a plausible reading is not received after 100 ms
        if (is_plausible)
        {
            is_plausible = false;
            TMR1_Counter16BitSet(0x00);
            TMR1_Start();
        }
    }
    else
    {
        is_plausible = true;
        is_100_ms_plausible = true;
        TMR1_Stop();
        LED3_SetHigh();
        LED2_SetLow();
    }
}

// 25-5 rule check. cut power if accelerator is past 25% and braking hard
void check_25_5_plausibility()
{
    if((brake_1 > BRK1_BRAKING_HARD || brake_2 > BRK2_BRAKING_HARD) && (apps_1 > APPS1_25_PERCENT || raw_apps_2 > RAW_APPS2_25_PERCENT))
    {
        car_data.is_25_5_plausible = false;
        
        // TODO:add CAN message to send this fault to dash
        
        LED3_SetLow();
        LED2_SetHigh();
    }
    
    // allow torque requests when APPSs go below 5% throttle
    if(apps_1 < APPS1_5_PERCENT && raw_apps_2 < RAW_APPS2_5_PERCENT)
    {
        car_data.is_25_5_plausible = true;
        
        //TODO: add CAN message to send fault clear to dash
        
        LED3_SetHigh();
        LED2_SetLow();
    }
}

void set_brake_state()
{
    if (brake_1 > BRK1_BRAKING || brake_2 > BRK2_BRAKING)
    {
        car_data.is_braking = true;
        BRK_CTRL_SetHigh();
    }
    else
    {
        car_data.is_braking = false;
        BRK_CTRL_SetLow();
    }
}

void set_accelerator_state()
{
    if (apps_1 > APPS1_ACCEL_START || raw_apps_2 > RAW_APPS2_ACCEL_START)
    {
        car_data.accelerator_is_pressed = true;
    }
    else
    {
        car_data.accelerator_is_pressed = false;
    }
}

// inverter heartbeat message. called every 20 ms by timer 2
void send_torque_request()
{
    // check if our data is stale (no new message from the ACAN board)
    // check if a new message has been received since last torque_request call
    if (!message_received)
    {
        ++message_not_received_count;
    }
    else
    {
        message_not_received_count = 0;
        message_received = !message_received;
    }
    
    // good to send torque request
    if (message_not_received_count <= 5 && car_data.is_25_5_plausible && is_100_ms_plausible && car_data.ready_to_drive)
    {
        calculate_torque_request();
        inverter_cmd_data[0] = torque_times_ten & 0xFF;         // low order torque request byte
        inverter_cmd_data[1] = (torque_times_ten >> 8) & 0xFF;  // high order torque request byte
        inverter_cmd_data[5] = 0x01;                            // inverter enabled, torque mode, discharge disabled
    }
    else
    {
        // not good to go; send 0 torque request
        inverter_cmd_data[0] = 0;   // low order torque request byte
        inverter_cmd_data[1] = 0;   // high order torque request byte
        inverter_cmd_data[5] = 0;   // inverter disabled, torque mode, discharge disabled
    }
    
    CAN_Msg_Send(CAN_ID_INVERTER_HEARTBEAT, CAN_DLC_INVERTER_HEARTBEAT, inverter_cmd_data);
}

void trigger_100_ms_implausibility()
{
    is_100_ms_plausible = false;
    TMR1_Stop();
    
    LED3_SetLow();
    LED2_SetHigh();
}

// sets the max torque request to a percentage of the overall max torque
void set_torque_limit(uint8_t torque_percent)
{
    scaled_torque_limit = TORQUE_MAX * torque_percent / 100;
}

void calculate_torque_request()
{
    //TODO: regen logic
    if (apps_1 < APPS1_ACCEL_START)
    {
        torque_times_ten = 0;
        return;
    }
    
    if (apps_1 > APPS1_WIDE_OPEN_THROTTLE)
    {
        torque_times_ten = scaled_torque_limit * 10;
        return;
    }
    
    torque_times_ten = scaled_torque_limit * (((double)apps_1 - APPS1_ACCEL_START) / (APPS1_WIDE_OPEN_THROTTLE - APPS1_ACCEL_START)) * 10.0;
}