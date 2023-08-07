// PID
//   Author: Cole Barach
//   Created: 23.03.31
//   Updated: 23.03.31

#ifndef PID_H
#define	PID_H

#ifdef	__cplusplus
extern "C" {
#endif
    
// Primative Libraries
#include <stdint.h>
#include <stdbool.h>

// Functions ----------------------------------------------------------------------------------
float pid(float* signal, float kp, float ki, float kd, float time_step, float* signal_prime, float* signal_integral_sum);

float integrate(float* signal, float delta_time, float* signal_sum);

float derive(float* signal, float delta_time, float* signal_prime);

#ifdef	__cplusplus
}
#endif

#endif // PID_H