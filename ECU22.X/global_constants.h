// Global Constants
//   Author: Derek Dunn
//   Created: 21.08.30
//   Updated: 23.02.16
//   Description: Global constants that will rarely be changed.

#ifndef GLOBAL_CONSTANTS_H
#define	GLOBAL_CONSTANTS_H

// Primative Libraries
#include <stdint.h>
#include <stdbool.h>

// Includes
#include "configuration_variables.h"
#include "macros.h"

// Processor ------------------------------------------------------------------------------------------------------------------
#define FCY 40000000UL  // Instruction Cycle Frequency (Hz) - Required for <libpic30.h>

// CAN Message IDs ------------------------------------------------------------------------------------------------------------
#define CAN_OFFSET_INVERTER         0xA0 - 0xA0                    // Inverter CAN ID offset (Actual Offset - Default Offset)
#define CAN_ID_COMMAND_INVERTER     0x0C0 + CAN_OFFSET_INVERTER    // ID of Inverter Command Message

#define CAN_ID_INVERTER_FAULT       0xAB  + CAN_OFFSET_INVERTER    // id of inverter fault broadcast message
#define CAN_ID_INVERTER_FAULT_CLEAR 0xC1  + CAN_OFFSET_INVERTER    // id of inverter fault clear message

#define CAN_ID_INPUT_PEDALS         0x005                          // ID of Pedals Raw Message
#define CAN_ID_DATA_PEDALS          0x701                          // ID of Pedals Percent Message

#define CAN_ID_CALIBRATE_APPS       0x533
#define CAN_ID_CALIBRATE_BRAKE      0x534

#endif	/* GLOBAL_CONSTANTS_H */

