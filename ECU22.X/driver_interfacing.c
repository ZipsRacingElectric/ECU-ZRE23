#include <stdbool.h>
#include <stdint.h>
#include "driver_interfacing.h"
#include "global_constants.h"
#include "torque_handling.h"
#include "mcc_generated_files/pin_manager.h"

// dash message handler variables
static bool button_start = false;
static bool button_reset = false;
static VEHICLE_MODES prev_dash_mode = NAW;
static VEHICLE_MODES dash_mode = NAW;

void handle_dash_msg(uint8_t* message_data) 
{
    button_start = message_data[0] & 0x1;        // get bit 0 of frame 0
    button_reset = (message_data[0] >> 1) & 0x1; // get bit 1 of frame 0
    dash_mode = (message_data[0] >> 4) & 0x7;        // get bits 4, 5, and 6 of frame 0

    if (dash_mode != prev_dash_mode)
    {
        switch (dash_mode)
        {
            case DEBUG:
                set_torque_limit(TORQUE_PERCENT_DEBUG);
                break;

            case DAN:
                LED3_Toggle();
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
        prev_dash_mode = dash_mode;
    }
}


