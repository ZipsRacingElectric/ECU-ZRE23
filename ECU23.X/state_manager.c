// Libraries
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/tmr4.h"

// Constants
#include "global_constants.h"

// Libraries
#include <libpic30.h>

// Includes
#include "state_manager.h"
#include "torque_handling.h"
#include "indicator_driver.h"
#include "adc_driver.h"

// Prototypes ---------------------------------------------------------------------------------
// On Pin Change
// - Interrupt for the CN Interrupt Handler
// - Manages the high_voltage_enabled state
static void on_pin_change();

// Get Start Button State
// - Call to Check the State of the Start Button
// - Will call Set Ready to Drive if the Start Button is Pressed
void get_start_button_state();

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

// Variables ----------------------------------------------------------------------------------
uint16_t imd_pwm_high_samples = 0;
uint16_t imd_pwm_low_samples = 0;
bool imd_previously_high = false;

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
    HV_CTRL_SetLow();
    PUMP_CTRL_SetLow();
    LED4_SetLow();
}

void get_start_button_state()
{
    // Ignore if Ready to Drive
    if(car_state.ready_to_drive) return; 
    
    // ADC Delay
    __delay_us(1);
    
    // Get Button Input
    bool button_pressed = (get_ADC_value(BUTTON_START) < 16); // Active Low signal
    if(button_pressed) set_ready_to_drive();
}

void get_bspd_state()
{   
    // ADC Delay
    __delay_us(1);
    
    car_state.bspd_fault_present = (get_ADC_value(BSPD_FAULT) > 512); // Active High Signal
}

void get_imd_state()
{
    // ADC Delay
    __delay_us(1);
    
    // Sample IMD Fault
    car_state.imd_fault_present = (get_ADC_value(IMD_FAULT) > 512); // Active High Signal
    
    // ADC Delay
    __delay_us(1);
    
    // Sample IMD PWM Data
    bool imd_reading_high = (get_ADC_value(IMD_PWM_DATA) > 512);
    
    car_state.imd_resistance = imd_pwm_high_samples;
    
    if(imd_reading_high)
    {
        ++imd_pwm_high_samples;
        imd_previously_high = true;
    }
    else
    {
        ++imd_pwm_low_samples;
        
        // Falling Edge. Calculate Duty Cycle
        if(imd_previously_high)
        {
            // IMD Resistance (kilo-ohms) = (1200k * 90%) / (Duty Cycle - 5%) - 1200k
//            car_state.imd_resistance = (1080 / ((float)imd_pwm_high_samples / (imd_pwm_high_samples + imd_pwm_low_samples) - 0.05f)) - 1200;
            
            imd_pwm_high_samples = 0;
            imd_pwm_low_samples = 0;
        }
        
        imd_previously_high = false;
    }
}

void check_state()
{
    get_start_button_state();
    get_bspd_state();
    get_imd_state();
}