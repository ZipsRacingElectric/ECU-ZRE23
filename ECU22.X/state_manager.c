#include <stdbool.h>
#include "mcc_generated_files/pin_manager.h"
#include "state_manager.h"
#include "global_constants.h"
#include "car_data.h"
#include "torque_handling.h"
#include "indicator_driver.h"

extern struct Car_Data car_data;

static uint8_t percent = 0;
static REGEN_MODE prev_regen_mode;

void update_vehicle_mode(VEHICLE_MODES dash_mode)
{
    if(dash_mode == car_data.mode)
    {
        return;
    }
           
    switch (dash_mode)
    {
        case DEBUG:
            car_data.maximum_torque_percent = TORQUE_PERCENT_DEBUG;
            break;

        case DAN:
            car_data.maximum_torque_percent = TORQUE_PERCENT_DAN;
            break;

        case SKIDPAD:
            car_data.maximum_torque_percent = TORQUE_PERCENT_SKIDPAD;
            break;

        case ENDURANCE:
            car_data.maximum_torque_percent = TORQUE_PERCENT_ENDURANCE;
            break;

        case AUTOCROSS:
            car_data.maximum_torque_percent = TORQUE_PERCENT_AUTOCROSS;
            break;

        case ACCEL:
            car_data.maximum_torque_percent = TORQUE_PERCENT_ACCEL;
            break;

        case BEANS:
            car_data.maximum_torque_percent = TORQUE_PERCENT_BEANS;
            break;

        case NAW:
        default:
            // don't change torque limit if an invalid mode was sent
            break;
    }
    
    car_data.mode = dash_mode;
}

void update_regen_torque_mode(REGEN_MODE mode)
{
    if (mode == prev_regen_mode)
    {
        return;
    }
    
    switch (mode)
    {
        case PERCENT_15:
            car_data.regen_percent = 15;
            break;
        case PERCENT_30:
            car_data.regen_percent = 30;
            break;
        case PERCENT_45:
            car_data.regen_percent = 45;
            break;
        case PERCENT_60:
            car_data.regen_percent = 60;
            break;
        case PERCENT_75:
            car_data.regen_percent = 75;
            break;
        case PERCENT_90:
            car_data.regen_percent = 90;
            break;
        case PERCENT_100:
            car_data.regen_percent = 100;
            break;
        case INVALID:
        default:
            break;
    }
    
    set_regen_torque(car_data.regen_percent);
    prev_regen_mode = mode;
}

void set_ready_to_drive()
{
    // enter ready to drive if the below conditions are met
    if (HV_ON_GetValue() && car_data.is_braking && !car_data.ready_to_drive 
            && !car_data.accelerator_is_pressed && car_data.mode != NAW)
    {
        start_RTD_buzzer();
        car_data.ready_to_drive = true;
        PUMP_CTRL_SetHigh();
        LED4_SetHigh();
    }
}

void exit_ready_to_drive()
{
    car_data.ready_to_drive = false;
    PUMP_CTRL_SetLow();
    LED4_SetLow();
}