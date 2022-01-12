/* 
 * File:   global_constants.h
 * Author: Derek Dunn
 * Description: Global constants that will rarely be changed.
 * Created on August 30, 2021, 3:45 PM
 */

#include "configuration_variables.h"
#include "macro_functions.h"
#include <stdint.h>

#ifndef GLOBAL_CONSTANTS_H
#define	GLOBAL_CONSTANTS_H

//CAN Constants
//replace defines with const variables?
#define ACAN_MSG_ID             0x05
#define DASH_MSG_ID             0xFF

#define IMD_MEAS_ID             0x651   //id of message broadcasting IMD measured resistance
#define IMD_MEAS_MSG_SIZE       2       //2 bytes of data

#define LED_ST_ID               0x321   //id of message from ECU to dash containing LED states
#define LED_ST_MSG_SIZE         2       //LED message to dash is 2 bytes

#define LV_BATT_ID              0x541   //id of message broadcasting LV voltage
#define LV_BATT_MSG_SIZE        2       //2 bytes of data

//Timer Constants

//Fcy = 40MHz. we use a 1:256 prescale on TMR1, so F = 40MHz/256 = 156.25KHz => T = 1/156.25KHz 100ms/T = 15625. Q.E.D.
uint16_t TMR1_100MS_VAL = 15625; 

//Constants that need to be calculated initially.
uint16_t APPS_10PERCENT_PLAUSIBILITY = (APPS1_REAL_MAX - APPS1_REAL_MIN) / 10;

uint16_t APPS2_MIN_RANGE = CALC_APPS2_FROM_APPS1(APPS1_MIN_RANGE);     //lowest plausible accelerator position, anything lower indicates an error
uint16_t APPS2_REAL_MIN = CALC_APPS2_FROM_APPS1(APPS1_REAL_MIN);      //this is actually what is the pedal reading when it is not pressed
uint16_t APPS2_ACCEL_START = CALC_APPS2_FROM_APPS1(APPS1_ACCEL_START);    //this is where the ECU begins to request torque
uint16_t APPS2_WOT = CALC_APPS2_FROM_APPS1(APPS1_WOT);           //point for wide open throttle
uint16_t APPS2_REAL_MAX = CALC_APPS2_FROM_APPS1(APPS1_REAL_MAX);      //this is actually what is the pedal reading when it is fully pressed
uint16_t APPS2_MAX_RANGE = CALC_APPS2_FROM_APPS1(APPS1_MAX_RANGE);     //highest plausible accelerator position, anything higher indicates an error

//Button Constants
typedef enum
{
	INITIAL = -1,
    NOT_PUSHED = 0,
    PUSHED = 1,
}BTN_STATE;

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
}VEHICLE_MODES;


#endif	/* GLOBAL_CONSTANTS_H */

