// Configuration Variables
//   Author: Derek Dunn
//   Created: 21.09.13
//   Updated: 23.02.16
//   Description: This file defines global constants that are reconfigured when tuning systems. ex: APPS plausibility 

#ifndef CONFIGURATION_VARIABLES_H
#define	CONFIGURATION_VARIABLES_H

// Pedal Mappings -----------------------------------------------------------------------------
#define APPS_ABS_MIN                    -10         // APPS Absolute Minimum Percentage
#define APPS_REGEN_START                0           // APPS Regen Start Percentage
#define APPS_THROTTLE_START             20          // APPS Throttle Start / Regen End Percentage
#define APPS_THROTTLE_END               90          // APPS Throttle End Percentage
#define APPS_ABS_MAX                    110         // APPS Absolute Maximum Percentage5

#define BRAKES_ABS_MIN                  -10         // Brake Absolute Minimum Percentage
#define BRAKES_REGEN_START              0           // Brake Regen Start Percentage
#define BRAKES_BRAKE_START              15          // Brake Brake Light Start Percentage
#define BRAKES_REGEN_END                25          // Brake Regen End Percentage
#define BRAKES_BRAKE_HARD               50          // Brake Hard Braking Percentage
#define BRAKES_ABS_MAX                  110         // Brake Absolute Maximum Percentage

// Torque Values ------------------------------------------------------------------------------
#define TORQUE_MAX                      230         // Maximum Requestable Torque
#define REGEN_TORQUE_MAX                230         // Maximum Requestable Regen Torque
#define REGEN_COASTING_PERCENT          10          // Percent of Regen applied when Coasting

#define APPS_REAL_RANGE_TOLERANCE       4           // Percent of Tolerance in APPS Pedal Range
#define BRAKE_REAL_RANGE_TOLERANCE      4           // Percent of Tolerance in Brake Pedal Range

#define APPS1_DEFAULT_REAL_MIN          40          // Default APPS-1 Real Min
#define APPS1_DEFAULT_REAL_MAX          475         // Default APPS-1 Real Max
#define APPS2_DEFAULT_REAL_MIN          90          // Default APPS-2 Real Min
#define APPS2_DEFAULT_REAL_MAX          780         // Default APPS-2 Real Max
#define BRAKE1_DEFAULT_REAL_MIN         120         // Default Brake-1 Real Min
#define BRAKE1_DEFAULT_REAL_MAX         400         // Default Brake-1 Real Max
#define BRAKE2_DEFAULT_REAL_MIN         120         // Default Brake-2 Real Min
#define BRAKE2_DEFAULT_REAL_MAX         400         // Default Brake-2 Real Max

#define LOW_LV_VAL 609  //12 V on the battery is divided down to 2.976 V, ADC produces ~609

#endif // CONFIGURATION_VARIABLES_H
