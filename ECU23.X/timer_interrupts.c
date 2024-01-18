// Header
#include "timer_interrupts.h"

// Libraries
#include "mcc_generated_files/tmr1.h"
#include "mcc_generated_files/tmr2.h"
#include "mcc_generated_files/tmr3.h"
#include "mcc_generated_files/tmr4.h"

// Includes
#include "torque_handling.h"
#include "indicator_driver.h"
#include "state_manager.h"
#include "can_driver.h"

static uint16_t update_20ms_counter = 0;

static void interrupt_20ms_service()
{
    // Executes every 20ms
    // Torque Handling
    get_pedal_values();
    set_brake_state();
    set_accelerator_state();
    send_torque_request();
    
    // CAN Messages
    send_pedal_messages();
    send_status_ecu();
    
    // Increment Counter. Prevent Overflow 
    if(update_20ms_counter == 10000) update_20ms_counter = 0;
    else ++update_20ms_counter;
}

void initialize_timer_interrupts()
{
    // Reset Timers
    TMR1_Stop();
    TMR2_Stop();
    TMR3_Stop();
    TMR4_Stop();

    // Set Interrupts
    TMR1_SetInterruptHandler(set_pedal_100_ms_implausible);    
    TMR2_SetInterruptHandler(interrupt_20ms_service);
    TMR3_SetInterruptHandler(end_RTD_buzzer);
    TMR4_SetInterruptHandler(exit_ready_to_drive);
    
    // Start 20ms Interrupt
    TMR2_Start();
}
