#include <stdbool.h>
#include <stdint.h>
#include "driver_interfacing.h"
#include "global_constants.h"

// dash message handler variables
static bool Dash_ButtonStart = false;
static bool Dash_ButtonReset = false;
static VEHICLE_MODES Dash_Mode = NAW;

void handle_dash_msg(uint8_t* message_data) 
{
    Dash_ButtonStart = message_data[0] & 0x1;        // get the first bit of frame 0
    Dash_ButtonReset = (message_data[0] >> 1) & 0x1; // get the second bit of frame 0
    Dash_Mode = (message_data[0] >> 2) & 0x7;        // get bits 3, 4, and 5 of frame 0

    switch(Dash_Mode)
    {
        case DEBUG:                   
            break;

        case DAN:
            break;

        case SKIDPAD:
            break;

        case ENDURANCE:
            break;

        case AUTOCROSS:
            break;

        case ACCEL:
            break;

        case BEANS:
            break;

        case NAW:
            break;
                
    }
}


