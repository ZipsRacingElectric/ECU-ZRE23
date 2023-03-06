// CAN Driver
//   Author: Derek Dunn
//   Created: 22.02.18
//   Updated: 23.02.16

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

// Functions
void initialize_CAN_driver(void);

void send_command_inverter(bool inverter_enabled, int16_t torque_x10, uint16_t torque_limit_x10);

void send_status_ecu();

void send_pedal_messages();

#ifdef	__cplusplus
}
#endif

#endif // CAN_DRIVER_H