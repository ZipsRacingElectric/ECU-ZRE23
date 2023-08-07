// CAN Driver
//   Author: Derek Dunn
//   Created: 22.02.18
//   Updated: 23.04.07

#ifndef CAN_DRIVER_H
#define	CAN_DRIVER_H

#ifdef	__cplusplus
extern "C" {
#endif

// Primative Libraries
#include <stdint.h>
#include <stdbool.h>

// Libraries
#include "mcc_generated_files/can_types.h"

// Initialize CAN Driver
// - Call to Initialize the CAN Drive
// - Creates the Rx/Tx Objects and initializes CAN1
void initialize_CAN_driver(void);

// Send Command Inverter
// - Call to send the Command Inverter Message
// - Sends the CAN Message
void send_command_inverter(bool inverter_enabled, int16_t torque_x10, uint16_t torque_limit_x10);

// Send Status ECU
// - Call to send the Status ECU Message
// - Sends the CAN Message
void send_status_ecu();

// Send Pedal Messages
// - Call to send the Input Pedals Message and the Data Pedals Message
// - Sends both CAN Messages
void send_pedal_messages();

#ifdef	__cplusplus
}
#endif

#endif // CAN_DRIVER_H