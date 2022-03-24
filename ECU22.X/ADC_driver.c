#include "ADC_driver.h"
#include "mcc_generated_files/adc1.h"
#include "global_constants.h"
#include <libpic30.h>        // __delayXXX() functions

void initialize_ADC()
{
    ADC1_Enable();
}

uint16_t get_ADC_channel_reading(ADC1_CHANNEL channel)
{
    ADC1_ChannelSelect(channel);
    ADC1_SoftwareTriggerEnable();
    __delay_us(1);
    ADC1_SoftwareTriggerDisable();
    while(!ADC1_IsConversionComplete(channel));
    return ADC1_ConversionResultGet(channel);
}