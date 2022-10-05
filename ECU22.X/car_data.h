/* 
 * File:   car_data.h
 * Author: Derek
 *
 * Created on February 23, 2022, 4:27 PM
 */

#ifndef CAR_DATA_H
#define	CAR_DATA_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "global_constants.h"
    
struct Car_Data
{
    bool ready_to_drive;
    bool is_braking;
    bool accelerator_is_pressed;
    bool inverter_fault_present;
    bool is_25_5_plausible;
    bool DRS_enabled;
    bool regen_enabled;
    uint8_t regen_percent;
    volatile VEHICLE_MODES mode;
    volatile uint16_t apps_1;
    volatile uint16_t apps_2;
    volatile uint16_t apps_2_raw;
    volatile uint16_t brake_1;
    volatile uint16_t brake_2;
    volatile bool ACAN_message_received;
    uint16_t lv_battery_voltage;
    uint16_t IMD_resistance;
    uint16_t maximum_torque_percent;
};

void initialize_car_data();

#ifdef	__cplusplus
}
#endif

#endif	/* CAR_DATA_H */

