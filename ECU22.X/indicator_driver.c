#include "indicator_driver.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/tmr3.h"

//TODO: put dash LED indicator logic in here

static int buzz_cycles = 7;
static int buzz_cycle_count = 0;

void start_RTD_buzzer()
{    
    buzz_cycle_count = 0;
    BUZZ_CTRL_SetHigh();
    TMR3_Counter16BitSet(0x00);
    TMR3_Start();
}

void end_RTD_buzzer()
{
    ++buzz_cycle_count;
    
    // allow the buzzer to sound for (timer 3 interrupt period * buzz_cycles) seconds
    if (buzz_cycle_count >= buzz_cycles)
    {
        BUZZ_CTRL_SetLow();
        TMR3_Stop();
    }
}
