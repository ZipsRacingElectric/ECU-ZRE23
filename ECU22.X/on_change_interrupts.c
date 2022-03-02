#include <stdbool.h>
#include "on_change_interrupts.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/tmr4.h"

static int prev_hv_on = -1;

void on_change()
{
    // if hv_on changed from true to false, start timer that will kick car out of drive mode
    if (prev_hv_on == 1 && !HV_ON_GetValue())
    {
        TMR4_Counter16BitSet(0x00);
        TMR4_Start();
    }
    else if (HV_ON_GetValue())
    {
        TMR4_Stop();
    }
    
    prev_hv_on = HV_ON_GetValue();
}

void initialize_change_notification_interrupts() 
{
    on_change();    // call the handler on startup to get initial values
    CN_SetInterruptHandler(on_change);
}