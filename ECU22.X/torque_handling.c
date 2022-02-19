#include <stdbool.h>
#include "mcc_generated_files/pin_manager.h"
#include "global_constants.h"
#include "torque_handling.h"
#include "configuration_variables.h"
#include "mcc_generated_files/tmr1.h"

static void check_apps_and_brakes_plausibility();
static void check_25_5_plausibility();
static void set_brake_light();
static void send_torque_request();

static const uint16_t APPS1_25_PERCENT = (APPS1_REAL_MAX - APPS1_REAL_MIN) / 4 + APPS1_REAL_MIN;    // used for 25/5 plausibility check
static const uint16_t APPS1_5_PERCENT = (APPS1_REAL_MAX - APPS1_REAL_MIN) / 20 + APPS1_REAL_MIN;    // used for 25/5 plausibility check

static const uint16_t APPS_10PERCENT_PLAUSIBILITY = (APPS1_REAL_MAX - APPS1_REAL_MIN) / 10;

static const uint16_t APPS2_MIN_RANGE = CALC_APPS2_FROM_APPS1(APPS1_MIN_RANGE);         // lowest plausible accelerator position, anything lower indicates an error
static const uint16_t APPS2_REAL_MIN = CALC_APPS2_FROM_APPS1(APPS1_REAL_MIN);           // this is actually what the pedal reading is when it is not pressed
static const uint16_t APPS2_ACCEL_START = CALC_APPS2_FROM_APPS1(APPS1_ACCEL_START);     // this is where the ECU begins to request torque
static const uint16_t APPS2_WOT = CALC_APPS2_FROM_APPS1(APPS1_WOT);                     // point for wide open throttle
static const uint16_t APPS2_REAL_MAX = CALC_APPS2_FROM_APPS1(APPS1_REAL_MAX);           // this is actually what is the pedal reading when it is fully pressed
static const uint16_t APPS2_MAX_RANGE = CALC_APPS2_FROM_APPS1(APPS1_MAX_RANGE);         // highest plausible accelerator position, anything higher indicates an error
static uint16_t APPS2_25_PERCENT;                                                       // used for 25/5 plausibility check
static uint16_t APPS2_5_PERCENT;                                                        // used for 25/5 plausibility check

static bool is_plausible = true;
static bool is_25_5_plausible = true;
static bool is_100_ms_plausible = true;

static uint16_t apps_1;
static uint16_t raw_apps_2;
static uint16_t scaled_apps_2;
static uint16_t brake_1;
static uint16_t brake_2;

void initialize_apps_2()
{
    APPS2_25_PERCENT = (APPS2_REAL_MAX - APPS2_REAL_MIN) / 4 + APPS2_REAL_MIN;
    APPS2_5_PERCENT = (APPS2_REAL_MAX - APPS2_REAL_MIN) / 20 + APPS2_REAL_MIN;
}

void handle_acan_message(uint8_t* message_data)
{
    apps_1 = (message_data[1] << 8) | message_data[0];
    
    raw_apps_2 = (message_data[3] << 8) | message_data[2];
    scaled_apps_2 = CALC_SCALED_APPS2(raw_apps_2);
    
    brake_1 = (message_data[5] << 8) | message_data[4];
    brake_2 = (message_data[7] << 8) | message_data[6];
    
    set_brake_light();
    check_apps_and_brakes_plausibility();
    check_25_5_plausibility();
    send_torque_request();
}

void check_apps_and_brakes_plausibility()
{
    // the below statement should be true when implausible.
    if (apps_1 < (scaled_apps_2 - APPS_10PERCENT_PLAUSIBILITY) || apps_1 > (scaled_apps_2 + APPS_10PERCENT_PLAUSIBILITY) // check for 10% Implausibility
        || apps_1 < APPS1_MIN_RANGE || apps_1 > APPS1_MAX_RANGE             // check for APPS1 out of range
        || raw_apps_2 < APPS2_MIN_RANGE || raw_apps_2 > APPS2_MAX_RANGE     // check for APPS2 out of range
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
    if((brake_1 > BRK1_BRAKING_HARD || brake_2 > BRK2_BRAKING_HARD) && (apps_1 > APPS1_25_PERCENT || raw_apps_2 > APPS2_25_PERCENT))
    {
        is_25_5_plausible = false;
        
        // add CAN message to send this fault to dash
        
        LED3_SetLow();
        LED2_SetHigh();
    }
    
    // allow torque requests when APPSs go below 5% throttle
    if(apps_1 < APPS1_5_PERCENT && raw_apps_2 < APPS2_5_PERCENT)
    {
        is_25_5_plausible = true;
        
        // add CAN message to send fault clear to dash
        
        LED3_SetHigh();
        LED2_SetLow();
    }
}

void set_brake_light()
{
    if (brake_1 > BRK1_BRAKING || brake_2 > BRK2_BRAKING)
    {
        BRK_CTRL_SetHigh();
    }
    else
    {
        BRK_CTRL_SetLow();
    }
}

void send_torque_request()
{
    if (is_25_5_plausible && is_100_ms_plausible) // add drive mode check to this!!
    {
        // send request
    }
    else
    {
        // send 0 torque request
    }
}

void trigger_100_ms_implausibility()
{
    is_100_ms_plausible = false;
    TMR1_Stop();
    
    LED3_SetLow();
    LED2_SetHigh();
}