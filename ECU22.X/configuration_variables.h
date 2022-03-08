/* 
 * File:   configuration_variables.h
 * Author: Derek Dunn
 * Description: This file defines global constants that are reconfigured when tuning systems. ex: APPS plausibility 
 * Created on September 13, 2021, 6:48 PM
 */

#ifndef CONFIGURATION_VARIABLES_H
#define	CONFIGURATION_VARIABLES_H

//TODO: calibrate all variables in this file

#define TORQUE_MAX               230        // maximum torque allowed to be requested in newton-meters

#define TORQUE_PERCENT_DEBUG     5
#define TORQUE_PERCENT_DAN       10
#define TORQUE_PERCENT_SKIDPAD   20
#define TORQUE_PERCENT_ENDURANCE 30
#define TORQUE_PERCENT_AUTOCROSS 40
#define TORQUE_PERCENT_ACCEL     50
#define TORQUE_PERCENT_BEANS     100


#define APPS2_OFFSET            99          
#define APPS2_SCALING_FACTOR    2           

#define APPS1_MIN_RANGE			50          //slightly beyond pedal travel, lowest plausible accelerator position, anything lower indicates an error
#define APPS1_REAL_MIN			80          //end of pedal travel, this is actually what is the pedal reading when it is not pressed
#define APPS1_ACCEL_START 		300         //this is where the ECU begins to request torque
#define APPS1_WOT               380         //point for wide open throttle
#define APPS1_REAL_MAX			410         //end of actual pedal travel, this is actually what is the pedal reading when it is fully pressed
#define APPS1_MAX_RANGE 		440         //slightly beyond pedal travel, highest plausible accelerator position, anything higher indicates an error

#define BRK1_MIN_RANGE          50          // brakes outside these ranges = fault condition
#define BRK1_MAX_RANGE          900
#define BRK2_MIN_RANGE          50
#define BRK2_MAX_RANGE          900

#define BRK1_BRAKING            110         // to turn brake light on
#define BRK1_BRAKING_HARD       175         // value of brake input signal when "braking hard" for 25-5 rule
#define BRK2_BRAKING            110         // to turn brake light on
#define BRK2_BRAKING_HARD       175         // value of brake input signal when "braking hard" for 25-5 rule

#define MAX_ACAN_MSG_NOT_RECEIVED_COUNT 5   // how many messages are allowed to be missed before sending 0 torque request

#endif	/* CONFIGURATION_VARIABLES_H */

