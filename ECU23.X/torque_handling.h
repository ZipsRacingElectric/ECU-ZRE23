// Torque Handling
//   Author: Derek Dunn
//   Created: 22.02.18
//   Updated: 23.04.07

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
// - Calculates the APPS and Brake Maps based on default values
void initialize_torque_handler();
    
// Set APPS Mapping
// - Call to update the APPS Maps
// - Updates the Maps of APPS-1 & APPS-2 based on Real Min & Real Max values
void set_apps_mapping();

// Set Brake Mapping
// - Call to update the Brake Maps
// - Updates the Maps of Brake-1 & Brake-2 based on Real Min & Real Max values
void set_brake_mapping();

// Get Pedal Values
// - Call to get the Pedal values
// - Writes pedal values to the APPS and Brake Maps
void get_pedal_values();

// Set Brake State
// - Call to set the State of the Brakes
// - Writes the Brake State to car_state and controls the Brake Light
void set_brake_state();

// Set Accelerator State
// - Call to set the State of the APPS
// - Writes the Accelerating State to car_state
void set_accelerator_state();

// Send Torque Request
// - Call to Request Torque
// - Calculates the requested Torque and Sends the Inverter Command Message
void send_torque_request();

// Set Pedal 100ms Implausible
// - Call to set the Pedal 100ms Plausibility
// - Writes the Pedal 100ms Plausibility to car_state
void set_pedal_100_ms_implausible();

// Objects ------------------------------------------------------------------------------------

// APPS Map
// - Contains the Mapping of APPS States to Values
// - Contains the Value of the APPS Sensor
struct apps_map
{
    // Sensor Value
    uint16_t value;
    int16_t  percent_x10;
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

// Brake Map
// - Contains the Mapping of Brake States to Values
// - Contains the Value of the Brake Sensor
struct brake_map
{
    // Sensor Value
    uint16_t value;
    uint16_t percent_x10;
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

// Global Data --------------------------------------------------------------------------------

extern volatile struct apps_map  apps1;        // APPS-1 Map
extern volatile struct apps_map  apps2;        // APPS-2 Map
extern volatile struct brake_map brake1;       // Brake-1 Map
extern volatile struct brake_map brake2;       // Brake-2 Map

extern volatile uint16_t torque_limit;         // Torque Limit in Nm
extern volatile uint16_t regen_limit;          // Regen Limit in Nm

#ifdef	__cplusplus
}
#endif

#endif // TORQUE_HANDLING_H