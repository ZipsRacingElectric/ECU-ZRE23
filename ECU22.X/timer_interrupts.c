#include "timer_interrupts.h"
#include "torque_handling.h"
#include "mcc_generated_files/tmr1.h"

void initialize_timers()
{
    TMR1_Stop();
    TMR1_SetInterruptHandler(trigger_100_ms_implausibility);
}
