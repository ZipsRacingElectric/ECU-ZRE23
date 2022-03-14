#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "car_data.h"
#include "global_constants.h"
#include "macros.h"
#include "mcc_generated_files/tmr1.h"
#include "timer_interrupts.h"
#include "can_driver.h"
#include "torque_handling.h"
#include "on_change_interrupts.h"
#include <libpic30.h>        // __delayXXX() functions

int main(void)
{
    // initialize the device

    SYSTEM_Initialize();
    CAN_Initialize();
    initialize_apps_2();
    initialize_timers();
    initialize_car_data();
    initialize_change_notification_interrupts();
    HV_CTRL_SetHigh(); // allow high voltage to be used
 
    while (1)
    {
        __delay_ms(100);
        //LED4_Toggle();
    }
    return 1; 
}