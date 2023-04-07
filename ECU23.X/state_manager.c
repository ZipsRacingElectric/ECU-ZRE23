// Libraries
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/tmr4.h"

// Constants
#include "global_constants.h"

// Includes
#include "state_manager.h"
#include "torque_handling.h"
#include "indicator_driver.h"
#include "adc_driver.h"

// Prototypes ---------------------------------------------------------------------------------
static void on_pin_change();

// Global Data --------------------------------------------------------------------------------
volatile struct state car_state =
{   
    .high_voltage_enabled         = false,
    .ready_to_drive               = false,
    .regen_enabled                = false,
    .torque_plausible             = false,
    .pedals_100ms_plausible       = false,
    .pedals_plausible             = false,
    .apps_plausible               = false,
    .apps_calibration_plausible   = false,
    .brakes_plausible             = false,
    .brakes_calibration_plausible = false,
    .apps_25_5_plausible          = false,
    .accelerating                 = false,
    .braking                      = false,
    .lv_battery_voltage           = 0,
    .imd_resistance               = 0
};

// Initializers -------------------------------------------------------------------------------
void initialize_state_manager()
{
    on_pin_change();
    CN_SetInterruptHandler(on_pin_change);
}

// Interrupts ---------------------------------------------------------------------------------
void on_pin_change()
{
    bool high_voltage_enabled = HV_ON_GetValue();
    // If HV changes from true to false, exit ready to drive.
    if(!high_voltage_enabled && car_state.high_voltage_enabled)
    {
        TMR4_Counter16BitSet(0x00);
        TMR4_Start();
    }
    else if(high_voltage_enabled)
    {
        TMR4_Stop();
    }
    
    car_state.high_voltage_enabled = HV_ON_GetValue();
}

// Ready to Drive -----------------------------------------------------------------------------
void set_ready_to_drive()
{
    bool ready_to_drive_plausible = true;
    
    ready_to_drive_plausible &= HV_ON_GetValue();
    ready_to_drive_plausible &= car_state.braking;
    ready_to_drive_plausible &= !car_state.accelerating;
    
    ready_to_drive_plausible &= !car_state.ready_to_drive;
    
    if(!ready_to_drive_plausible) return;
    
    car_state.ready_to_drive = true;
    start_RTD_buzzer();
    PUMP_CTRL_SetHigh();
    LED4_SetHigh();
}

void exit_ready_to_drive()
{
    car_state.ready_to_drive = false;
    PUMP_CTRL_SetLow();
    LED4_SetLow();
}

void get_start_button_state()
{
    // Ignore if Ready to Drive
    if(car_state.ready_to_drive) return;
    
    // Get Button Input
    bool button_pressed = (get_ADC_value(BUTTON_START) < 512); // Active LOW
    if(button_pressed) set_ready_to_drive();
}