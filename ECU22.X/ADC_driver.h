/* 
 * File:   ADC_driver.h
 * Author: Derek
 *
 * Created on March 23, 2022, 9:59 PM
 */

#ifndef ADC_DRIVER_H
#define	ADC_DRIVER_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "mcc_generated_files/adc1.h"

void initialize_ADC();
uint16_t get_ADC_channel_reading(ADC1_CHANNEL channel);


#ifdef	__cplusplus
}
#endif

#endif	/* ADC_DRIVER_H */

