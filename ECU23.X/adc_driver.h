// ADC Driver
//   Author: Derek Dunn
//   Created: 22.03.23
//   Updated: 23.02.16

#ifndef ADC_DRIVER_H
#define	ADC_DRIVER_H

#ifdef	__cplusplus
extern "C" {
#endif

// Libraries
#include "mcc_generated_files/adc1.h"

// Functions ----------------------------------------------------------------------------------
// Initialize ADC
// - Call before Reading ADC Values
// - Initializes ADC-1
void initialize_ADC();

// Get ADC Value
// - Call to get the ADC value on a Channel
// - Returns the ADC Reading
uint16_t get_ADC_value(ADC1_CHANNEL channel);

#ifdef	__cplusplus
}
#endif

#endif // ADC_DRIVER_H