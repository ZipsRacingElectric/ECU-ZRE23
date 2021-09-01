/* 
 * File:   global_constants.h
 * Author: Derek Dunn
 *
 * Created on August 30, 2021, 3:45 PM
 */

#ifndef GLOBAL_CONSTANTS_H
#define	GLOBAL_CONSTANTS_H

//CAN Constants
#define ACAN_MSG_ID             0x05
#define DASH_MSG_ID             0xFF

#define IMD_MEAS_ID             0x651   //id of message broadcasting IMD measured resistance
#define IMD_MEAS_MSG_SIZE       2       //2 bytes of data

#define LED_ST_ID               0x321   //id of message from ECU to dash containing LED states
#define LED_ST_MSG_SIZE         2       //LED message to dash is 2 bytes

#define LV_BATT_ID              0x541   //id of message broadcasting LV voltage
#define LV_BATT_MSG_SIZE        2       //2 bytes of data

#endif	/* GLOBAL_CONSTANTS_H */

