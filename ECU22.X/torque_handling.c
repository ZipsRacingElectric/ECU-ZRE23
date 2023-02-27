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
#include "ADC_driver.h"

// Functions ----------------------------------------------------------------------------------
// Check Torque Plausibility
// - Call to check the plausibility of the Torque message
// - Writes plausibility to car_state
static void check_torque_plausibility();

// Check Pedal Plausibility
// - Call to check the plausibility of the Pedal values
// - Writes plausibility to car_state
static void check_pedal_plausibility();

// Check APPS 25/5 Plausibility
// - Call to check the plausibility of the APPS based on the 25/5 Rule
// - Writes plausibility to car_state
static void check_apps_25_5_plausibility();

// Calculate Torque Request
// - Call to get the requested torque
// - Returns the Torque x10
int16_t calculate_torque_request();

// Global Data --------------------------------------------------------------------------------
uint16_t torque_limit = 0;
uint16_t regen_limit  = 0;

volatile struct apps_map apps1 =
{
    .value          = 0,
    .real_min       = APPS1_DEFAULT_REAL_MIN,
    .real_max       = APPS1_DEFAULT_REAL_MAX
};

volatile struct apps_map apps2 =
{
    .value          = 0,
    .real_min       = APPS2_DEFAULT_REAL_MIN,
    .real_max       = APPS2_DEFAULT_REAL_MAX
};

volatile struct brake_map brake1 =
{
    .value       = 0,
    .real_min    = BRAKE1_DEFAULT_REAL_MIN,
    .real_max    = BRAKE1_DEFAULT_REAL_MAX
};

volatile struct brake_map brake2 =
{
    .value       = 0,
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
    // Invalidate Current Values.
    apps1.value = 0;
    apps2.value = 0;
    car_state.apps_calibration_plausible = false;
    
    // Calculate APPS-1 Values from Percentages
    //                  9-bit = (10-bit         - 10-bit        ) / 2-bit
    uint16_t apps1_range_half = (apps1.real_max - apps1.real_min) >> 1;
    
    //                     9-bit            *  7-bit                       / 6-bit - 10-bit
    apps1.abs_min        = apps1_range_half * (APPS_ABS_MIN        & 0x7F) / 50    - apps1.real_min;
    apps1.throttle_start = apps1_range_half * (APPS_THROTTLE_START & 0x7F) / 50    - apps1.real_min;
    apps1.throttle_end   = apps1_range_half * (APPS_THROTTLE_END   & 0x7F) / 50    - apps1.real_min;
    apps1.abs_max        = apps1_range_half * (APPS_ABS_MAX        & 0x7F) / 50    - apps1.real_min;
    //                     (9-bit         * 1-bit) / 5-bit - 10-bit
    apps1.percent_05     = (apps1_range_half << 1) / 20    - apps1.real_min;
    apps1.percent_10     = (apps1_range_half << 1) / 10    - apps1.real_min;
    apps1.percent_25     = (apps1_range_half << 1) / 4     - apps1.real_min;
    
    // Calculate APPS-2 Values from Percentages
    //                  9-bit = (10-bit         - 10-bit        ) / 2-bit
    uint16_t apps2_range_half = (apps2.real_max - apps2.real_min) >> 1;
    
    //                     9-bit            *  7-bit                       / 6-bit - 10-bit
    apps2.abs_min        = apps2_range_half * (APPS_ABS_MIN        & 0x7F) / 50    - apps2.real_min;
    apps2.throttle_start = apps2_range_half * (APPS_THROTTLE_START & 0x7F) / 50    - apps2.real_min;
    apps2.throttle_end   = apps2_range_half * (APPS_THROTTLE_END   & 0x7F) / 50    - apps2.real_min;
    apps2.abs_max        = apps2_range_half * (APPS_ABS_MAX        & 0x7F) / 50    - apps2.real_min;
    //                     (9-bit         * 1-bit) / 5-bit - 10-bit
    apps2.percent_05     = (apps2_range_half << 1)  / 20    - apps2.real_min;
    apps2.percent_10     = (apps2_range_half << 1)  / 10    - apps2.real_min;
    apps2.percent_25     = (apps2_range_half << 1)  / 4     - apps2.real_min;
    
    // Check Plausibility
    car_state.apps_calibration_plausible = true;
    
    car_state.apps_calibration_plausible &= apps1.real_min < apps1.real_max;
    car_state.apps_calibration_plausible &= apps2.real_min < apps2.real_max;
    
    car_state.apps_calibration_plausible &= apps1.abs_min < apps1.real_min;
    car_state.apps_calibration_plausible &= apps2.abs_min < apps2.real_min;
    car_state.apps_calibration_plausible &= apps1.abs_max > apps1.real_max;
    car_state.apps_calibration_plausible &= apps2.abs_max > apps2.real_max;
}

void set_brake_mapping()
{
    // Invalidate Current Values.
    brake1.value = 0;
    brake2.value = 0;
    car_state.brakes_calibration_plausible = false;
    
    // Calculate Brake-1 Values from Percentages
    //                   9-bit = (10-bit          - 10-bit         ) / 2-bit
    uint16_t brake1_range_half = (brake1.real_max - brake1.real_min) >> 1;
    
    //                   9-bit             *  7-bit                      / 6-bit - 10-bit
    brake1.abs_min     = brake1_range_half * (BRAKES_ABS_MIN     & 0x7F) / 50    - brake1.real_min;
    brake1.regen_start = brake1_range_half * (BRAKES_REGEN_START & 0x7F) / 50    - brake1.real_min;
    brake1.brake_start = brake1_range_half * (BRAKES_BRAKE_START & 0x7F) / 50    - brake1.real_min;
    brake1.regen_end   = brake1_range_half * (BRAKES_REGEN_END   & 0x7F) / 50    - brake1.real_min;
    brake1.brake_hard  = brake1_range_half * (BRAKES_BRAKE_HARD  & 0x7F) / 50    - brake1.real_min;
    brake1.abs_max     = brake1_range_half * (BRAKES_ABS_MAX     & 0x7F) / 50    - brake1.real_min;
    
    // Calculate Brake-2 Values from Percentages
    //                   9-bit = (10-bit          - 10-bit         ) / 2-bit
    uint16_t brake2_range_half = (brake2.real_max - brake2.real_min) >> 1;
    
    //                   9-bit             *  7-bit                      / 6-bit - 10-bit
    brake2.abs_min     = brake2_range_half * (BRAKES_ABS_MIN     & 0x7F) / 50    - brake2.real_min;
    brake2.regen_start = brake2_range_half * (BRAKES_REGEN_START & 0x7F) / 50    - brake2.real_min;
    brake2.brake_start = brake2_range_half * (BRAKES_BRAKE_START & 0x7F) / 50    - brake2.real_min;
    brake2.regen_end   = brake2_range_half * (BRAKES_REGEN_END   & 0x7F) / 50    - brake2.real_min;
    brake2.brake_hard  = brake2_range_half * (BRAKES_BRAKE_HARD  & 0x7F) / 50    - brake2.real_min;
    brake2.abs_max     = brake2_range_half * (BRAKES_ABS_MAX     & 0x7F) / 50    - brake2.real_min;
    
    // Check Plausibility
    car_state.brakes_calibration_plausible = true;
    
    car_state.brakes_calibration_plausible &= brake1.real_min < brake1.real_max;
    car_state.brakes_calibration_plausible &= brake2.real_min < brake2.real_max;
    
    car_state.brakes_calibration_plausible &= brake1.abs_min < brake1.real_min;
    car_state.brakes_calibration_plausible &= brake2.abs_min < brake2.real_min;
    car_state.brakes_calibration_plausible &= brake1.abs_max > brake1.real_max;
    car_state.brakes_calibration_plausible &= brake2.abs_max > brake2.real_max;
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
    car_state.apps_plausible &= apps2.value < apps1.value + (apps1.percent_10 - apps1.real_min);
    car_state.apps_plausible &= apps2.value > apps1.value - (apps1.percent_10 - apps1.real_min);

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

    if(car_state.torque_plausible)
    {
        int16_t torque_x10 = calculate_torque_request();
        send_command_inverter(true, torque_x10, torque_limit);
    }
    else
    {
        send_command_inverter(false, 0, 0);
    }
}

int16_t calculate_torque_request()
{
    // TODO IMPLEMENT regen speed limit
    if(apps1.value < apps1.regen_start)
    {
        // Coasting Regen
        if(!car_state.regen_enabled) return 0;
        
        return regen_limit * 10;
    }
    else if(apps1.value < apps1.throttle_start)
    {
        // Regen Zone
        if(!car_state.regen_enabled) return 0;
        
        uint16_t apps_regen_range = apps1.throttle_start - apps1.regen_start;
        uint16_t brake_regen_range = brake1.regen_end - brake1.regen_start;
        
        uint16_t regen_throttle_percent = (apps1.throttle_start - apps1.value) / apps_regen_range; 
        uint16_t regen_brake_percent    = (brake1.value - brake1.regen_start)  / brake_regen_range;
        uint16_t regen_percent = (regen_throttle_percent * REGEN_COASTING_PERCENT) / 100 + (regen_brake_percent * (100 - REGEN_COASTING_PERCENT)) / 100;
        
        return -(regen_percent * regen_limit / 10);
    }
    else if(apps1.value < apps1.throttle_end)
    {   
        // Throttle Zone
        uint16_t apps_throttle_range = apps1.throttle_end - apps1.throttle_start;
        
        uint16_t throttle_percent = (apps1.value - apps1.throttle_start) / apps_throttle_range;
        
        return throttle_percent * torque_limit / 10;
    }
    else if(apps1.value < apps1.abs_max)
    {
        // Open Throttle Zone
        return torque_limit * 10;
    }
    
    return 0;
}