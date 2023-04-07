// Primative Libraries
#include <stdint.h>
#include <stdbool.h>

// Constants
#include "global_constants.h"
#include "macros.h"

// Libraries
#include <libpic30.h>
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/tmr1.h"

// Includes
#include "timer_interrupts.h"
#include "can_driver.h"
#include "state_manager.h"
#include "adc_driver.h"
#include "torque_handling.h"

int main(void)
{
    // MCC Initializations
    SYSTEM_Initialize();
    
    // Initializations
    initialize_CAN_driver();
    initialize_timer_interrupts();
    initialize_state_manager();
    initialize_ADC();
    initialize_torque_handler();

    // Allow Tractive Systems to Engage
    HV_CTRL_SetHigh();
 
    // Heartbeat Loop
    while(true)
    {
        // Period of 1ms
        __delay_ms(1);
        
//        // Read LV-Battery Voltage
//        car_state.lv_battery_voltage = get_ADC_value(LV_BATTERY_VOLTAGE);
//            
//        // Delay between ADC Readings
//        __delay_us(1);
//            
//        // Read IMD Resistance
//        car_state.imd_resistance = get_ADC_value(IMD_RESISTANCE);
            
        // Toggle Heartbeat LED
        LED4_Toggle();
    }
    
    // On Exit, Return Error Code 1
    return 1; 
}