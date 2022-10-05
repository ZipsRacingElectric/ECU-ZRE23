#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "car_data.h"
#include "global_constants.h"
#include "macros.h"
#include "mcc_generated_files/tmr1.h"
#include "timer_interrupts.h"
#include "can_driver.h"
#include "torque_handling.h"
#include "mcc_generated_files/adc1.h"
#include "ADC_driver.h"
#include "on_change_interrupts.h"
#include "car_data.h"
#include <libpic30.h>        // __delayXXX() functions

int main(void)
{
    // initialize the device

    SYSTEM_Initialize();
    CAN_Initialize();
    initialize_apps_2();
    initialize_timers();
    initialize_car_data();
    initialize_change_notification_interrupts();
    initialize_inverter_cmd_data();
    initialize_ADC();

    HV_CTRL_SetHigh(); // allow high voltage to be used
 
    static uint16_t counter = 99;
    
    extern struct Car_Data car_data;

    while (1)
    {      
        __delay_ms(100);
        ++counter;
        
        // trigger ADC every 10 seconds
        if (counter == 100)
        {
            car_data.lv_battery_voltage = get_ADC_channel_reading(LV_BATTERY_VOLTAGE);
            __delay_us(1);
            car_data.IMD_resistance = get_ADC_channel_reading(IMD_RESISTANCE);
            
            send_status_message();
            
            counter = 0;
        }
        LED4_Toggle();
    }
    return 1; 
}