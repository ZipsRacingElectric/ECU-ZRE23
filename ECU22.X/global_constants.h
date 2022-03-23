/*
 * File:   global_constants.h
 * Author: Derek Dunn
 * Description: Global constants that will rarely be changed.
 * Created on August 30, 2021, 3:45 PM
 */

#ifndef GLOBAL_CONSTANTS_H
#define	GLOBAL_CONSTANTS_H

#include "configuration_variables.h"
#include "macros.h"
#include <stdint.h>

#define FCY 40000000UL          // instruction cycle frequency, Hz - required for __delayXXX() to work

//CAN Constants
#define CAN_ID_ACAN                 0x05
#define CAN_ID_DASH                 0xFF

#define CAN_ID_IMD_RESISTANCE       0x651               // id of message broadcasting IMD measured resistance
#define CAN_DLC_IMD_RESISTANCE      2                   // 2 bytes of data
 
#define CAN_ID_LED_STATE            0x321               // id of message from ECU to dash containing LED states
#define CAN_DLC_LED_STATE           1                   // LED message to dash is 1 byte
 
#define CAN_ID_LV_BATTERY           0x541               // id of message broadcasting LV voltage
#define CAN_DLC_LV_BATTERY          2                   // 2 bytes of data
 
#define INVOFFSET                   0xA0 - 0xA0         // offset for inverter ids for CAN messages, actual offset - default offset
#define CAN_ID_INVERTER_HEARTBEAT   0x0C0 + INVOFFSET   // id of inverter heartbeat message
#define CAN_DLC_INVERTER_HEARTBEAT  8                   // inv heartbeat message is 8 bytes
#define CAN_ID_INVERTER_FAULT       0xAB                // id of inverter fault broadcast message

typedef enum
{
	DEBUG = 0,
    DAN = 1,
    SKIDPAD = 2,
    ENDURANCE = 3,
    AUTOCROSS = 4,
	ACCEL = 5,
    BEANS = 6,
    NAW = 7         // state for when selector switch is between positions (cause switch is break before make)
} VEHICLE_MODES;

#endif	/* GLOBAL_CONSTANTS_H */

