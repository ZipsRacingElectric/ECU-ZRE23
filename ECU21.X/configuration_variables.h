/* 
 * File:   configuration_variables.h
 * Author: Derek Dunn
 * Description: This file defines global constants that are reconfigured when tuning systems. ex: APPS plausibility 
 * Created on September 13, 2021, 6:48 PM
 */

#ifndef CONFIGURATION_VARIABLES_H
#define	CONFIGURATION_VARIABLES_H

#define APPS2_OFFSET            99 //temp value
#define APPS2_SCALING_FACTOR    2 //temp value

#define APPS1_MIN_RANGE			50		//slightly beyond pedal travel, lowest plausible accelerator position, anything lower indicates an error
#define APPS1_REAL_MIN			80		//end of pedal travel, this is actually what is the pedal reading when it is not pressed
#define APPS1_ACCEL_START 		100		//this is where the ECU begins to request torque
#define APPS1_WOT               380 	//point for wide open throttle
#define APPS1_REAL_MAX			410		//end of actual pedal travel, this is actually what is the pedal reading when it is fully pressed
#define APPS1_MAX_RANGE 		440 	//slightly beyond pedal travel, highest plausible accelerator position, anything higher indicates an error



#endif	/* CONFIGURATION_VARIABLES_H */

