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

#define FCY 40000000UL          // Instruction cycle frequency, Hz - required for __delayXXX() to work

//CAN Constants
#define ACAN_MSG_ID             0x05
#define DASH_MSG_ID             0xFF

#define IMD_MEAS_ID             0x651   //id of message broadcasting IMD measured resistance
#define IMD_MEAS_MSG_SIZE       2       //2 bytes of data

#define LED_ST_ID               0x321   //id of message from ECU to dash containing LED states
#define LED_ST_MSG_SIZE         2       //LED message to dash is 2 bytes

#define LV_BATT_ID              0x541   //id of message broadcasting LV voltage
#define LV_BATT_MSG_SIZE        2       //2 bytes of data

typedef enum
{
	DEBUG = 0,
    DAN = 1,
    SKIDPAD = 2,
    ENDURANCE = 3,
    AUTOCROSS = 4,
	ACCEL = 5,
    BEANS = 6,
    NAW = 7 //state for when selector switch is between positions (cause switch is break before make)
} VEHICLE_MODES;

#endif	/* GLOBAL_CONSTANTS_H */

