// Header
#include "pid.h"

float pid(float* signal, float kp, float ki, float kd, float time_step, float* signal_prime, float* signal_integral_sum)
{
    float p = *signal;
    float i = integrate(signal, time_step, signal_integral_sum);
    float d = derive(signal, time_step, signal_prime);
    
    *signal_integral_sum += i;
    *signal_prime = *signal;
    
    p *= kp;
    i *= ki;
    d *= kd;
    
    return p + i + d;
}

float integrate(float* signal, float delta_time, float* signal_sum)
{
    return *signal * delta_time + *signal_sum;
}

float derive(float* signal, float delta_time, float* signal_prime)
{
    return (*signal - *signal_prime) / delta_time;
}