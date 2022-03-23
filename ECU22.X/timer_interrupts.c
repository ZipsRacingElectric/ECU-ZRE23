#include "timer_interrupts.h"
#include "torque_handling.h"
#include "indicator_driver.h"
#include "state_manager.h"
#include "can_driver.h"
#include "mcc_generated_files/tmr1.h"
#include "mcc_generated_files/tmr2.h"
#include "mcc_generated_files/tmr3.h"
#include "mcc_generated_files/tmr4.h"
#include "mcc_generated_files/tmr5.h"

void initialize_timers()
{
    TMR1_Stop();
    TMR2_Start();
    TMR3_Stop();
    TMR4_Stop();
    TMR5_Start();

    // control + click on the function parameters to go to the function definitions :)
    
    TMR1_SetInterruptHandler(trigger_100_ms_implausibility);    
    TMR2_SetInterruptHandler(send_torque_request);
    TMR3_SetInterruptHandler(end_RTD_buzzer);
    TMR4_SetInterruptHandler(exit_ready_to_drive);
    TMR5_SetInterruptHandler(send_LED_indicator_state);  // combine this timer interrupt service with timer 2 if a timer is needed for something else.
}
