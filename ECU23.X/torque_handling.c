// Header
#include "torque_handling.h"

// Libraries
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/tmr1.h"

// Constants
#include "global_constants.h"
#include "configuration_variables.h"

// Includes
#include "can_driver.h"
#include "state_manager.h"
#include "adc_driver.h"
#include "pid.h"

// Functions ----------------------------------------------------------------------------------
// Check Torque Plausibility
// - Call to check the plausibility of the Torque request
// - Writes plausibility to car_state
static void check_torque_plausibility();

// Check Pedal Plausibility
// - Call to check the plausibility of the Pedal values
// - Writes plausibility to car_state
static void check_pedal_plausibility();

// Check APPS 25/5 Plausibility
// - Call to check the plausibility of the APPS value based on the 25/5 Rule
// - Writes plausibility to car_state
static void check_apps_25_5_plausibility();

// Get Pedal Torque Request
// - Call to get the Torque value requested by the Pedal inputs.
// - Returns the requested Torque multiplied by 10
static int16_t get_pedal_torque_request();

// Global Data --------------------------------------------------------------------------------
volatile uint16_t torque_limit = 23;
volatile uint16_t regen_limit  = 0;

volatile struct apps_map apps1 =
{
    .value          = 0,
    .percent_x10    = 0,
    .real_min       = APPS1_DEFAULT_REAL_MIN,
    .real_max       = APPS1_DEFAULT_REAL_MAX
};

volatile struct apps_map apps2 =
{
    .value          = 0,
    .percent_x10    = 0,
    .real_min       = APPS2_DEFAULT_REAL_MIN,
    .real_max       = APPS2_DEFAULT_REAL_MAX
};

volatile struct brake_map brake1 =
{
    .value       = 0,
    .percent_x10 = 0,
    .real_min    = BRAKE1_DEFAULT_REAL_MIN,
    .real_max    = BRAKE1_DEFAULT_REAL_MAX
};

volatile struct brake_map brake2 =
{
    .value       = 0,
    .percent_x10 = 0,
    .real_min    = BRAKE2_DEFAULT_REAL_MIN,
    .real_max    = BRAKE2_DEFAULT_REAL_MAX
};

// Initializers -------------------------------------------------------------------------------
void initialize_torque_handler()
{
    set_apps_mapping();
    set_brake_mapping();
}

// Pedal Mapping ------------------------------------------------------------------------------
void set_apps_mapping()
{
    // Invalidate Current Values
    apps1.value = 0;
    apps2.value = 0;
    apps1.percent_x10 = 0;
    apps2.percent_x10 = 0;
    car_state.apps_calibration_plausible = false;
 
    // Apply Tolerance to APPS-1 Real Range
    int32_t apps1_real_min = (int32_t)apps1.real_min - APPS_REAL_RANGE_TOLERANCE * (apps1.real_max - apps1.real_min) / 100;
    int32_t apps1_real_max = (int32_t)apps1.real_max + APPS_REAL_RANGE_TOLERANCE * (apps1.real_max - apps1.real_min) / 100;
    
    if(apps1_real_min < 0)    apps1_real_min = 0;
    if(apps1_real_max > 1023) apps1_real_max = 1023;
    
    if(apps1_real_min < apps1.real_min) apps1.real_min = (uint16_t)apps1_real_min;
    if(apps1_real_max > apps1.real_max) apps1.real_max = (uint16_t)apps1_real_max;
    
    // Calculate APPS-1 Values from Percentages
    //                  9-bit = (10-bit         - 10-bit        ) / 2-bit
    uint16_t apps1_range_half = (apps1.real_max - apps1.real_min) >> 1;
    
    int32_t apps1_abs_min = (int32_t)apps1_range_half * APPS_ABS_MIN     / 50    + apps1.real_min;
    int32_t apps1_abs_max = (int32_t)apps1_range_half * APPS_ABS_MAX     / 50    + apps1.real_min;
    
    if(apps1_abs_min < 0)    apps1_abs_min = 0;
    if(apps1_abs_max > 1023) apps1_abs_max = 1023;
    
    apps1.abs_min        = apps1_abs_min;
    apps1.throttle_start = (int32_t)apps1_range_half * APPS_THROTTLE_START / 50    + apps1.real_min;
    apps1.throttle_end   = (int32_t)apps1_range_half * APPS_THROTTLE_END   / 50    + apps1.real_min;
    apps1.abs_max        = apps1_abs_max;
    
    apps1.percent_05     = (int32_t)(apps1_range_half << 1) / 20    + apps1.real_min;
    apps1.percent_10     = (int32_t)(apps1_range_half << 1) / 10    + apps1.real_min;
    apps1.percent_25     = (int32_t)(apps1_range_half << 1) / 4     + apps1.real_min;
    
    // Apply Tolerance to APPS-2 Real Range
    int32_t apps2_real_min = (int32_t)apps2.real_min - APPS_REAL_RANGE_TOLERANCE * (apps2.real_max - apps2.real_min) / 100;
    int32_t apps2_real_max = (int32_t)apps2.real_max + APPS_REAL_RANGE_TOLERANCE * (apps2.real_max - apps2.real_min) / 100;
    
    if(apps2_real_min < 0)    apps2_real_min = 0;
    if(apps2_real_max > 1023) apps2_real_max = 1023;
    
    if(apps2_real_min < apps2.real_min) apps2.real_min = (uint16_t)apps2_real_min;
    if(apps2_real_max > apps2.real_max) apps2.real_max = (uint16_t)apps2_real_max;
    
    // Calculate APPS-2 Values from Percentages
    //                  9-bit = (10-bit         - 10-bit        ) / 2-bit
    uint16_t apps2_range_half = (apps2.real_max - apps2.real_min) >> 1;
    
    int32_t apps2_abs_min = (int32_t)apps2_range_half * APPS_ABS_MIN     / 50    + apps2.real_min;
    int32_t apps2_abs_max = (int32_t)apps2_range_half * APPS_ABS_MAX     / 50    + apps2.real_min;
    
    if(apps2_abs_min < 0)    apps2_abs_min = 0;
    if(apps2_abs_max > 1023) apps2_abs_max = 1023;
    
    apps2.abs_min        = apps2_abs_min;
    apps2.throttle_start = (int32_t)apps2_range_half * APPS_THROTTLE_START / 50    + apps2.real_min;
    apps2.throttle_end   = (int32_t)apps2_range_half * APPS_THROTTLE_END   / 50    + apps2.real_min;
    apps2.abs_max        = apps2_abs_max;
    
    apps2.percent_05     = (int32_t)(apps2_range_half << 1)  / 20    + apps2.real_min;
    apps2.percent_10     = (int32_t)(apps2_range_half << 1)  / 10    + apps2.real_min;
    apps2.percent_25     = (int32_t)(apps2_range_half << 1)  / 4     + apps2.real_min;
    
    // Check Plausibility
    car_state.apps_calibration_plausible = true;
    
    car_state.apps_calibration_plausible &= apps1.real_min < apps1.real_max;
    car_state.apps_calibration_plausible &= apps2.real_min < apps2.real_max;
    
    car_state.apps_calibration_plausible &= apps1.abs_min <= apps1.real_min;
    car_state.apps_calibration_plausible &= apps2.abs_min <= apps2.real_min;
    car_state.apps_calibration_plausible &= apps1.abs_max >= apps1.real_max;
    car_state.apps_calibration_plausible &= apps2.abs_max >= apps2.real_max;
}

void set_brake_mapping()
{
    // Invalidate Current Values
    brake1.value = 0;
    brake2.value = 0;
    car_state.brakes_calibration_plausible = false;
    
    // Apply Tolerance to Brake-1 Real Range
    int32_t brake1_real_min = (int32_t)brake1.real_min - BRAKE_REAL_RANGE_TOLERANCE * (brake1.real_max - brake1.real_min) / 100;
    int32_t brake1_real_max = (int32_t)brake1.real_max + BRAKE_REAL_RANGE_TOLERANCE * (brake1.real_max - brake1.real_min) / 100;
    
    if(brake1_real_min < 0)    brake1_real_min = 0;
    if(brake1_real_max > 1023) brake1_real_max = 1023;
    
    if(brake1_real_min < brake1.real_min) brake1.real_min = (uint16_t)brake1_real_min;
    if(brake1_real_max > brake1.real_max) brake1.real_max = (uint16_t)brake1_real_max;
    
    // Calculate Brake-1 Values from Percentages
    //                   9-bit = (10-bit          - 10-bit         ) / 2-bit
    uint16_t brake1_range_half = (brake1.real_max - brake1.real_min) >> 1;
    
    int32_t brake1_abs_min = (int32_t)brake1_range_half * BRAKES_ABS_MIN     / 50    + brake1.real_min;
    int32_t brake1_abs_max = (int32_t)brake1_range_half * BRAKES_ABS_MAX     / 50    + brake1.real_min;
    
    if(brake1_abs_min < 0)    brake1_abs_min = 0;
    if(brake1_abs_max > 1023) brake1_abs_max = 1023;
    
    brake1.abs_min     = brake1_abs_min;
    brake1.regen_start = (int32_t)brake1_range_half * BRAKES_REGEN_START / 50    + brake1.real_min;
    brake1.brake_start = (int32_t)brake1_range_half * BRAKES_BRAKE_START / 50    + brake1.real_min;
    brake1.regen_end   = (int32_t)brake1_range_half * BRAKES_REGEN_END   / 50    + brake1.real_min;
    brake1.brake_hard  = (int32_t)brake1_range_half * BRAKES_BRAKE_HARD  / 50    + brake1.real_min;
    brake1.abs_max     = brake1_abs_max;
    
    // Apply Tolerance to Brake-2 Real Range
    int32_t brake2_real_min = (int32_t)brake2.real_min - BRAKE_REAL_RANGE_TOLERANCE * (brake2.real_max - brake1.real_min) / 100;
    int32_t brake2_real_max = (int32_t)brake2.real_max + BRAKE_REAL_RANGE_TOLERANCE * (brake2.real_max - brake1.real_min) / 100;
    
    if(brake2_real_min < 0)    brake2_real_min = 0;
    if(brake2_real_max > 1023) brake2_real_max = 1023;
    
    if(brake2_real_min < brake2.real_min) brake2.real_min = (uint16_t)brake2_real_min;
    if(brake2_real_max > brake2.real_max) brake2.real_max = (uint16_t)brake2_real_max;
    
    // Calculate Brake-2 Values from Percentages
    //                   9-bit = (10-bit          - 10-bit         ) / 2-bit
    uint16_t brake2_range_half = (brake2.real_max - brake2.real_min) >> 1;
    
    int32_t brake2_abs_min = (int32_t)brake2_range_half * BRAKES_ABS_MIN     / 50    + brake2.real_min;
    int32_t brake2_abs_max = (int32_t)brake2_range_half * BRAKES_ABS_MAX     / 50    + brake2.real_min;
    
    if(brake2_abs_min < 0)    brake2_abs_min = 0;
    if(brake2_abs_max > 1023) brake2_abs_max = 1023;
    
    brake2.abs_min     = brake2_abs_min;
    brake2.regen_start = (int32_t)brake2_range_half * BRAKES_REGEN_START / 50    + brake2.real_min;
    brake2.brake_start = (int32_t)brake2_range_half * BRAKES_BRAKE_START / 50    + brake2.real_min;
    brake2.regen_end   = (int32_t)brake2_range_half * BRAKES_REGEN_END   / 50    + brake2.real_min;
    brake2.brake_hard  = (int32_t)brake2_range_half * BRAKES_BRAKE_HARD  / 50    + brake2.real_min;
    brake2.abs_max     = brake2_abs_max;
    
    // Check Plausibility
    car_state.brakes_calibration_plausible = true;
    
    car_state.brakes_calibration_plausible &= brake1.real_min < brake1.real_max;
    car_state.brakes_calibration_plausible &= brake2.real_min < brake2.real_max;
    
    car_state.brakes_calibration_plausible &= brake1.abs_min <= brake1.real_min;
    car_state.brakes_calibration_plausible &= brake2.abs_min <= brake2.real_min;
    car_state.brakes_calibration_plausible &= brake1.abs_max >= brake1.real_max;
    car_state.brakes_calibration_plausible &= brake2.abs_max >= brake2.real_max;
}

// Plausibility Checks ------------------------------------------------------------------------
void check_torque_plausibility()
{
    check_pedal_plausibility();
    check_apps_25_5_plausibility();
    
    car_state.torque_plausible = true;
    
    car_state.torque_plausible &= car_state.high_voltage_enabled;
    car_state.torque_plausible &= car_state.ready_to_drive;
    
    car_state.torque_plausible &= car_state.pedals_100ms_plausible;
    car_state.torque_plausible &= car_state.apps_25_5_plausible;
    car_state.torque_plausible &= car_state.apps_calibration_plausible;
    car_state.torque_plausible &= car_state.brakes_calibration_plausible;
}

void check_pedal_plausibility()
{
    car_state.apps_plausible   = true;
    car_state.brakes_plausible = true;
    
    // Check APPS-1 is in range
    car_state.apps_plausible &= apps1.value < apps1.abs_max;
    car_state.apps_plausible &= apps1.value > apps1.abs_min;
    
    // Check APPS-2 is in range
    car_state.apps_plausible &= apps2.value < apps2.abs_max;
    car_state.apps_plausible &= apps2.value > apps2.abs_min;
    
    // Check APPS-2 is within 10% of APPS-1
    car_state.apps_plausible &= apps2.percent_x10 - apps1.percent_x10 < 100;
    car_state.apps_plausible &= apps1.percent_x10 - apps2.percent_x10 < 100;

    // Check Brake-1 is in range
    car_state.brakes_plausible &= brake1.value < brake1.abs_max;
    car_state.brakes_plausible &= brake1.value > brake1.abs_min;
    
    // Check Brake-2 is in range
    car_state.brakes_plausible &= brake1.value < brake1.abs_max;
    car_state.brakes_plausible &= brake1.value > brake1.abs_min;
    
    // Check Plausibility State
    if(car_state.apps_plausible && car_state.brakes_plausible)
    {
        // Pedals are Plausible
        car_state.pedals_plausible = true;
        car_state.pedals_100ms_plausible = true;
        // Stop Timer 1
        TMR1_Stop();
        LED3_SetHigh();
        LED2_SetLow();
    }
    else
    {
        // Pedals are Implausible
        if(!car_state.pedals_plausible) return;
        
        // Start Timer 1 to trigger 100ms Implausibility
        car_state.pedals_plausible = false;
        TMR1_Counter16BitSet(0x00);
        TMR1_Start();
    }
}

void check_apps_25_5_plausibility()
{
    bool braking_hard = brake1.value > brake1.brake_hard || brake2.value > brake2.brake_hard;
    bool throttle_25_percent = apps1.value > apps1.percent_25 || apps2.value > apps2.percent_25;
    bool throttle_05_percent = apps1.value < apps1.percent_05 && apps2.value < apps2.percent_05;
    
    // Set 25/5 Implausible
    if(braking_hard && throttle_25_percent)
    {
        car_state.apps_25_5_plausible = false;
        
        LED3_SetLow();
        LED2_SetHigh();
    }
    
    // Reset Plausibility
    if(throttle_05_percent)
    {
        car_state.apps_25_5_plausible = true;
        
        LED3_SetHigh();
        LED2_SetLow();
    }
}

void set_pedal_100_ms_implausible()
{
    car_state.pedals_100ms_plausible = false;
    TMR1_Stop();
    
    LED3_SetLow();
    LED2_SetHigh();
}

// Pedals -------------------------------------------------------------------------------------
void get_pedal_values()
{
    apps1.value = get_ADC_value(APPS_1);
    apps2.value = get_ADC_value(APPS_2);
    brake1.value = get_ADC_value(BRAKE_1);
    brake2.value = get_ADC_value(BRAKE_2);
    
    // Using 32-bit representation as to prevent overflow.
    apps1.percent_x10  = (1000 * ((int32_t)apps1.value  - apps1.real_min ) / (apps1.real_max  - apps1.real_min));
    apps2.percent_x10  = (1000 * ((int32_t)apps2.value  - apps2.real_min ) / (apps2.real_max  - apps2.real_min));
    brake1.percent_x10 = (1000 * ((int32_t)brake1.value - brake1.real_min) / (brake1.real_max - brake1.real_min));
    brake2.percent_x10 = (1000 * ((int32_t)brake2.value - brake2.real_min) / (brake2.real_max - brake2.real_min));
}

// Pedal States -------------------------------------------------------------------------------
void set_brake_state()
{
    car_state.braking = brake1.value > brake1.brake_start || brake2.value > brake2.brake_start;
    
    // Set Brake Light
    if(car_state.braking)
    { 
        BRK_CTRL_SetHigh();
    }
    else
    {
        BRK_CTRL_SetLow();
    }
}

void set_accelerator_state()
{
    car_state.accelerating = apps1.value > apps1.throttle_start || apps2.value > apps2.throttle_start;
}

// Torque Request -----------------------------------------------------------------------------
void send_torque_request()
{
    check_torque_plausibility();

    if(!car_state.torque_plausible)
    {
        send_command_inverter(false, 0, 0);
        return;
    }
    
    int16_t torque_x10 = get_pedal_torque_request();
    
    send_command_inverter(true, torque_x10, torque_limit);
}

int16_t get_pedal_torque_request()
{
    // TODO IMPLEMENT regen speed limit
    if(apps1.value < apps1.regen_start)
    {
        // Coasting Regen
        if(!car_state.regen_enabled) return 0;
        
        uint16_t brake_regen_range = brake1.regen_end - brake1.regen_start;
        
        uint16_t brake_value = brake1.value;
        if(brake_value < brake1.regen_start) brake_value = brake1.regen_start;
        if(brake_value > brake1.regen_end)   brake_value = brake1.regen_end;
        
        uint16_t regen_brake_percent = 100 * (brake_value - brake1.regen_start) / brake_regen_range;
        
        uint16_t regen_percent = REGEN_COASTING_PERCENT + (regen_brake_percent * (100 - REGEN_COASTING_PERCENT)) / 100;
        
        return -(regen_percent * regen_limit / 10);
    }
    else if(apps1.value < apps1.throttle_start)
    {
        // Regen Zone
        if(!car_state.regen_enabled) return 0;
        
        uint16_t apps_regen_range = apps1.throttle_start - apps1.regen_start;
        uint16_t brake_regen_range = brake1.regen_end - brake1.regen_start;
        
        uint16_t brake_value = brake1.value;
        if(brake_value < brake1.regen_start) brake_value = brake1.regen_start;
        if(brake_value > brake1.regen_end)   brake_value = brake1.regen_end;
        
        uint16_t regen_throttle_percent = 100 * (apps1.throttle_start - apps1.value) / apps_regen_range; 
        uint16_t regen_brake_percent    = 100 * (brake_value - brake1.regen_start)   / brake_regen_range;
        uint16_t regen_percent = (regen_throttle_percent * REGEN_COASTING_PERCENT) / 100 + (regen_brake_percent * (100 - REGEN_COASTING_PERCENT)) / 100;
        
        return -(regen_percent * regen_limit / 10);
    }
    else if(apps1.value < apps1.throttle_end)
    {   
        // Throttle Zone
        uint16_t apps_throttle_range = apps1.throttle_end - apps1.throttle_start;
        
        uint16_t throttle_percent = 100 * (apps1.value - apps1.throttle_start) / apps_throttle_range;
        
        return throttle_percent * torque_limit / 10;
    }
    else if(apps1.value < apps1.abs_max)
    {
        // Open Throttle Zone
        return torque_limit * 10;
    }
    
    return 0;
}

//// Get Traction Byte
//uint16_t get_traction_control_byte()
//{
//    uint16_t rpm_front = (rpm_front_left + rpm_front_right) >> 1;
//    uint16_t rpm_rear  = (rpm_rear_left  + rpm_rear_right)  >> 1;
//    
//    float slip_ratio_difference = 0.05 - (1 - rpm_front / rpm_rear);
//    
//    float kp = 1;
//    float ki = 1;
//    float kd = 1;
//    
//    float time_step = 20 / 1000;
//    
//    return 255 * pid(&slip_ratio_difference, kp, ki, kd, time_step, &traction_control_prime, &traction_control_integral);
//}