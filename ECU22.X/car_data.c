#include <stdbool.h>
#include "car_data.h"
#include "global_constants.h"

volatile struct Car_Data car_data;

void initialize_car_data()
{
    car_data.mode = NAW;
    car_data.regen_mode = INVALID;
    car_data.maximum_torque_percent = 0;
    car_data.regen_percent = 0;
    car_data.is_braking = false;
    car_data.ready_to_drive = false;
    car_data.accelerator_is_pressed = false;
    car_data.inverter_fault_present = false;
    car_data.lv_battery_voltage = 0;
    car_data.IMD_resistance = 0;
    car_data.is_25_5_plausible = true;
    car_data.DRS_enabled = false;
    car_data.regen_enabled = false;
}