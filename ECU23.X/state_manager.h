// State Manager
//   Author: Derek Dunn
//   Created: 22.02.24
//   Updated: 23.02.16

#ifndef STATE_MANAGER_H
#define	STATE_MANAGER_H

#ifdef	__cplusplus
extern "C" {
#endif

// Primative Libraries
#include <stdint.h>
#include <stdbool.h>
    
// Constants
#include "global_constants.h"

// Functions ----------------------------------------------------------------------------------
// Initialize State Manager
// - Call to initialize the State Manager
// - Sets pin interrupts
void initialize_state_manager();

// Set Ready to Drive
// - Call to enter the Ready to Drive State
// - Will write to car_state if Ready to Drive Plausible
void set_ready_to_drive();

// Exit Ready to Drive
// - Call to exit the Ready to Drive State
// - Will write to car_state and disable the HV Control
void exit_ready_to_drive();

// Check State
// - Call to Read from the Analog and Digital inputs
// - Writes the determined state to car_state and calls any detected interrupts
void check_state();

// Global Data --------------------------------------------------------------------------------
struct state
{
    bool high_voltage_enabled;
    bool ready_to_drive;
    bool regen_enabled;
    
    bool torque_plausible;
    bool pedals_100ms_plausible;
    bool pedals_plausible;
    bool apps_plausible;
    bool apps_calibration_plausible;
    bool brakes_plausible;
    bool brakes_calibration_plausible;
    bool apps_25_5_plausible;
    bool inverter_fault_present;
    bool bspd_fault_present;
    bool imd_fault_present;
    
    bool accelerating;
    bool braking;
    
    uint16_t lv_battery_voltage;
    uint16_t imd_resistance;
};

extern volatile struct state car_state;

#ifdef	__cplusplus
}
#endif

#endif // STATE_MANAGER_H
