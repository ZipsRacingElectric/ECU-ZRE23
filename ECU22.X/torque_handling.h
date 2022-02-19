/* 
 * File:   torque_handling.h
 * Author: Derek
 *
 * Created on February 18, 2022, 5:40 PM
 */

#ifndef TORQUE_HANDLING_H
#define	TORQUE_HANDLING_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
    
void initialize_apps_2();
void handle_acan_message(uint8_t* message_data);
void trigger_100_ms_implausibility();
void send_torque_request();


#ifdef	__cplusplus
}
#endif

#endif	/* TORQUE_HANDLING_H */

