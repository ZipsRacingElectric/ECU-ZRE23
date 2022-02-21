/* 
 * File:   can_driver.h
 * Author: Derek
 *
 * Created on February 18, 2022, 5:41 PM
 */

#ifndef CAN_DRIVER_H
#define	CAN_DRIVER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "mcc_generated_files/can_types.h"

void CAN_Initialize(void);
void CAN_Msg_Send(uint16_t id, CAN_DLC dlc, uint8_t *tx_data);

#ifdef	__cplusplus
}
#endif

#endif	/* CAN_DRIVER_H */

