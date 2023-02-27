// Configuration Variables
//   Author: Derek Dunn
//   Created: 21.09.13
//   Updated: 23.02.16
//   Description: This file defines global constants that are reconfigured when tuning systems. ex: APPS plausibility 

#ifndef CONFIGURATION_VARIABLES_H
#define	CONFIGURATION_VARIABLES_H

// Pedal Mappings -----------------------------------------------------------------------------
#define APPS_ABS_MIN                    -5          // APPS Absolute Minimum Percentage
#define APPS_REGEN_START                2           // APPS Regen Start Percentage
#define APPS_THROTTLE_START             10          // APPS Throttle Start / Regen End Percentage
#define APPS_THROTTLE_END               90          // APPS Throttle End Percentage
#define APPS_ABS_MAX                    105         // APPS Absolute Maximum Percentage5

#define BRAKES_ABS_MIN                  -5          // Brake Absolute Minimum Percentage
#define BRAKES_REGEN_START              2           // Brake Regen Start Percentage
#define BRAKES_BRAKE_START              5           // Brake Brake Light Start Percentage
#define BRAKES_REGEN_END                20          // Brake Regen End Percentage
#define BRAKES_BRAKE_HARD               60          // Brake Hard Braking Percentage
#define BRAKES_ABS_MAX                  105         // Brake Absolute Maximum Percentage

// Torque Values ------------------------------------------------------------------------------
#define TORQUE_MAX                      230         // Maximum Requestable Torque
#define REGEN_TORQUE_MAX                230         // Maximum Requestable Regen Torque
#define REGEN_COASTING_PERCENT          10          // Percent of Regen applied when Coasting

#define APPS1_DEFAULT_REAL_MIN          80
#define APPS1_DEFAULT_REAL_MAX          410
#define APPS2_DEFAULT_REAL_MIN          80
#define APPS2_DEFAULT_REAL_MAX          410
#define BRAKE1_DEFAULT_REAL_MIN         55
#define BRAKE1_DEFAULT_REAL_MAX         895
#define BRAKE2_DEFAULT_REAL_MIN         55
#define BRAKE2_DEFAULT_REAL_MAX         895

#define LOW_LV_VAL 609  //12 V on the battery is divided down to 2.976 V, ADC produces ~609

#endif // CONFIGURATION_VARIABLES_H
