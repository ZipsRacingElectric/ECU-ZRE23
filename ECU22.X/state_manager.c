#include <stdbool.h>
#include "mcc_generated_files/pin_manager.h"
#include "state_manager.h"
#include "global_constants.h"
#include "car_data.h"
#include "torque_handling.h"
#include "indicator_driver.h"

extern struct Car_Data car_data;

// dash message handler variables
static bool is_start_pressed = false;
static bool is_reset_pressed = false;
static VEHICLE_MODES dash_mode = NAW;

void update_vehicle_mode(VEHICLE_MODES dash_mode)
{
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
}

void set_ready_to_drive()
{
    if (HV_ON_GetValue() && car_data.is_braking && !car_data.ready_to_drive 
            && !car_data.accelerator_is_pressed && car_data.mode != NAW)
    {
        start_RTD_buzzer();
        car_data.ready_to_drive = true;
        LED4_SetHigh();
    }
}

void handle_dash_msg(uint8_t* message_data) 
{
    is_start_pressed = message_data[0] & 0x1;           // get bit 0 of frame 0
    is_reset_pressed = (message_data[0] >> 1) & 0x1;    // get bit 1 of frame 0
    dash_mode = (message_data[0] >> 4) & 0x7;           // get bits 4, 5, and 6 of frame 0

    if (dash_mode != car_data.mode)
    {
        update_vehicle_mode(dash_mode);
        car_data.mode = dash_mode;
    }
    
    if (is_start_pressed)
    {
        set_ready_to_drive();
    }
}

void exit_ready_to_drive()
{
    car_data.ready_to_drive = false;
    LED4_SetLow();
}