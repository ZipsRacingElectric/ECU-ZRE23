#include <stdbool.h>
#include "mcc_generated_files/pin_manager.h"
#include "state_manager.h"
#include "global_constants.h"
#include "car_data.h"
#include "torque_handling.h"
#include "indicator_driver.h"

extern struct Car_Data car_data;

void update_vehicle_mode(VEHICLE_MODES dash_mode)
{
    if(dash_mode == car_data.mode)
    {
        return;
    }
           
    switch (dash_mode)
    {
        case DEBUG:
            set_torque_limit(TORQUE_PERCENT_DEBUG);
            break;

        case DAN:
            set_torque_limit(TORQUE_PERCENT_DAN);
            break;

        case SKIDPAD:
            set_torque_limit(TORQUE_PERCENT_SKIDPAD);
            break;

        case ENDURANCE:
            set_torque_limit(TORQUE_PERCENT_ENDURANCE);
            break;

        case AUTOCROSS:
            set_torque_limit(TORQUE_PERCENT_AUTOCROSS);
            break;

        case ACCEL:
            set_torque_limit(TORQUE_PERCENT_ACCEL);
            break;

        case BEANS:
            set_torque_limit(TORQUE_PERCENT_BEANS);
            break;

        case NAW:
        default:
            // don't change torque limit if an invalid mode was sent
            break;
    }
    
    car_data.mode = dash_mode;
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