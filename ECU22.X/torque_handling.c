#include <stdbool.h>
#include "mcc_generated_files/pin_manager.h"
#include "global_constants.h"
#include "torque_handling.h"
#include "configuration_variables.h"
#include "mcc_generated_files/tmr1.h"
#include "car_data.h"
#include "can_driver.h"
#include "mcc_generated_files/adc1.h" // TODO: figure out this circular reference

static void check_apps_and_brakes_plausibility();
static void check_25_5_plausibility();
static void set_brake_state();
static void set_accelerator_state();
static void calculate_torque_request();

extern struct Car_Data car_data;

static double scaled_torque_limit = 0;    // stores the maximum torque for the current drive mode
static double scaled_regen_torque_times_10 = 0;

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

static uint8_t ACAN_missed_messages = 0;

static uint8_t inverter_cmd_data[8];

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

void check_apps_and_brakes_plausibility()
{
    // the below statement should be true when implausible.
    if (car_data.apps_1 < (car_data.apps_2 - APPS_10PERCENT_PLAUSIBILITY) || car_data.apps_1 > (car_data.apps_2 + APPS_10PERCENT_PLAUSIBILITY) // check for 10% Implausibility
        || car_data.apps_1 < APPS1_MIN_RANGE         || car_data.apps_1 > APPS1_MAX_RANGE             // check for APPS1 out of range
        || car_data.apps_2_raw < RAW_APPS2_MIN_RANGE || car_data.apps_2_raw > RAW_APPS2_MAX_RANGE     // check for APPS2 out of range
        || car_data.brake_1 < BRK1_MIN_RANGE         || car_data.brake_1 > BRK1_MAX_RANGE             // check for brake_1 out of range
        || car_data.brake_2 < BRK2_MIN_RANGE         || car_data.brake_2 > BRK2_MAX_RANGE)            // check for brake_2 out of range
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
    if((car_data.brake_1 > BRK1_BRAKING_HARD || car_data.brake_2 > BRK2_BRAKING_HARD) && (car_data.apps_1 > APPS1_25_PERCENT || car_data.apps_2_raw > RAW_APPS2_25_PERCENT))
    {
        car_data.is_25_5_plausible = false;
        
        LED3_SetLow();
        LED2_SetHigh();
    }
    
    // allow torque requests when APPSs go below 5% throttle
    if(car_data.apps_1 < APPS1_5_PERCENT && car_data.apps_2_raw < RAW_APPS2_5_PERCENT)
    {
        car_data.is_25_5_plausible = true;
        
        LED3_SetHigh();
        LED2_SetLow();
    }
}

void set_brake_state()
{
    if (car_data.brake_1 > BRK1_BRAKING || car_data.brake_2 > BRK2_BRAKING)
    {
        car_data.is_braking = true;
        if (car_data.DRS_enabled)
        {
            car_data.DRS_enabled = false;
        }
        
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
    if (car_data.apps_1 > APPS1_ACCEL_START || car_data.apps_2_raw > RAW_APPS2_ACCEL_START)
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
    set_brake_state();
    set_accelerator_state();
    check_apps_and_brakes_plausibility();
    check_25_5_plausibility();
    
    // check if our data is stale (no new message from the ACAN board)
    // check if a new message has been received since last torque_request call
    if (!car_data.ACAN_message_received)
    {
        ++ACAN_missed_messages;
    }
    else
    {
        ACAN_missed_messages = 0;
        car_data.ACAN_message_received = false;
    }
    
    // good to send torque request
    if (ACAN_missed_messages <= 5 && car_data.is_25_5_plausible && is_100_ms_plausible && car_data.ready_to_drive)
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

void set_regen_torque(uint8_t torque_percent)
{
    scaled_regen_torque_times_10 = REGEN_TORQUE_MAX * torque_percent / 10; // 10 * x/100 = x/10
}

void calculate_torque_request()
{
    if (car_data.apps_1 < APPS1_ACCEL_START)
    {
        if (car_data.regen_enabled) 
        {
            torque_times_ten = -scaled_regen_torque_times_10;   // if you forget to put the negative here, we're gonna have a bad time.
        }
        else
        {
            torque_times_ten = 0;
        }
        
        return;
    }
    
    if (car_data.apps_1 > APPS1_WIDE_OPEN_THROTTLE)
    {
        torque_times_ten = scaled_torque_limit * 10;
        return;
    }
    
    torque_times_ten = scaled_torque_limit * (((double)car_data.apps_1 - APPS1_ACCEL_START) / (APPS1_WIDE_OPEN_THROTTLE - APPS1_ACCEL_START)) * 10.0;
}