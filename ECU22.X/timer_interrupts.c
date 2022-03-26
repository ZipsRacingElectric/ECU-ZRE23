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

static uint16_t counter = 0;

// called every 20 ms
static void twenty_ms_interrupt_service()
{
    // run every 20 ms
    send_torque_request();
    send_DRS_command();
    
    // run every 100 ms
    if (counter % 5 == 0)
    {
        send_LED_indicator_state();
    }
    
    counter == 10000 ? counter = 0 : ++counter;
}

void initialize_timers()
{
    TMR1_Stop();
    TMR2_Start();
    TMR3_Stop();
    TMR4_Stop();
    TMR5_Start();

    // control + click on the function parameters to go to the function definitions :)
    
    TMR1_SetInterruptHandler(trigger_100_ms_implausibility);    
    TMR2_SetInterruptHandler(twenty_ms_interrupt_service);
    TMR3_SetInterruptHandler(end_RTD_buzzer);
    TMR4_SetInterruptHandler(exit_ready_to_drive);
}
