// Header
#include "adc_driver.h"

// Constants
#include "global_constants.h"

// Libraries
#include <libpic30.h>

void initialize_ADC()
{
    ADC1_Enable();
}

uint16_t get_ADC_value(ADC1_CHANNEL channel)
{
    ADC1_ChannelSelect(channel);
    ADC1_SoftwareTriggerEnable();
    __delay_us(1);
    ADC1_SoftwareTriggerDisable();
    while(!ADC1_IsConversionComplete(channel));
    return ADC1_ConversionResultGet(channel);
}