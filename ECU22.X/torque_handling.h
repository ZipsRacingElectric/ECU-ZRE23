// Torque Handling
//   Author: Derek Dunn
//   Created: 22.02.18
//   Updated: 23.02.16

#ifndef TORQUE_HANDLING_H
#define	TORQUE_HANDLING_H

#ifdef	__cplusplus
extern "C" {
#endif
    
// Primative Libraries
#include <stdint.h>
#include <stdbool.h>

// Functions ----------------------------------------------------------------------------------

// Initialize Torque Handler
// - Call to initialize the torque handler
void initialize_torque_handler();
    
// Set APPS Mapping
// - Updates the mapping of APPS-1 & APPS-2 based on Real Min & Real Max values.
void set_apps_mapping();

// Set Brake Mapping
// - Updates the mapping of Brake-1 & Brake-2 based on Real Min & Real Max values.
void set_brake_mapping();

// Get Pedal Values
void get_pedal_values();

// Set Brake State
void set_brake_state();

// Set Accelerator State
void set_accelerator_state();

// Send Torque Request
// - Call to Request Torque
// - Calculates and Sends the Inverter Command Message
void send_torque_request();

// Set Pedal 100ms Implausible
void set_pedal_100_ms_implausible();

// Global Data --------------------------------------------------------------------------------

struct apps_map
{
    // Actual Value
    uint16_t value;
    // Mapping
    uint16_t abs_min;
    uint16_t real_min;
    uint16_t regen_start;
    uint16_t throttle_start;
    uint16_t throttle_end;
    uint16_t real_max;
    uint16_t abs_max;
    // Percents
    uint16_t percent_05;
    uint16_t percent_10;
    uint16_t percent_25;
};

struct brake_map
{
    // Actual Value
    uint16_t value;
    // Mapping
    uint16_t abs_min;
    uint16_t real_min;
    uint16_t regen_start;
    uint16_t regen_end;
    uint16_t brake_start;
    uint16_t brake_hard;
    uint16_t real_max;
    uint16_t abs_max;
};

extern volatile struct apps_map  apps1;
extern volatile struct apps_map  apps2;
extern volatile struct brake_map brake1;
extern volatile struct brake_map brake2;

#ifdef	__cplusplus
}
#endif

#endif	/* TORQUE_HANDLING_H */

