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
    volatile VEHICLE_MODES mode;
};

void initialize_car_data();

#ifdef	__cplusplus
}
#endif

#endif	/* CAR_DATA_H */

