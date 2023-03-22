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
void initialize_state_manager();

void set_ready_to_drive();

void exit_ready_to_drive();

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
