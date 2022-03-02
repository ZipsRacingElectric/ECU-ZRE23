/* 
 * File:   state_manager.h
 * Author: Derek
 *
 * Created on February 24, 2022, 11:31 AM
 */

#ifndef STATE_MANAGER_H
#define	STATE_MANAGER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "global_constants.h"

void update_vehicle_mode(VEHICLE_MODES dash_mode);
void set_ready_to_drive();
void exit_ready_to_drive();
void handle_dash_msg(uint8_t* message_data);

#ifdef	__cplusplus
}
#endif

#endif	/* STATE_MANAGER_H */

