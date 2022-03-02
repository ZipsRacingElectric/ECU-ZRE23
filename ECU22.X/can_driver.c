#include <stdint.h>
#include "mcc_generated_files/can1.h"
#include "mcc_generated_files/can_types.h"
#include "mcc_generated_files/pin_manager.h"
#include "global_constants.h"
#include "torque_handling.h"
#include "state_manager.h"
#include "can_driver.h"

static void CAN_Handle_Message_Rx(void);

static CAN_MSG_OBJ message;
static uint8_t can_data[8] = {0,0,0,0,0,0,0,0};

void CAN_Initialize(void)
{
    message.data = can_data;
    
    CAN1_TransmitEnable();
    CAN1_ReceiveEnable();
    CAN1_OperationModeSet(CAN_NORMAL_OPERATION_MODE);
    CAN1_SetRxBufferInterruptHandler(&CAN_Handle_Message_Rx);
    
    CAN_STBY_SetLow();
}

void CAN_Handle_Message_Rx(void)
{
    //Get the CAN message
    if(CAN1_Receive(&message))
    {
        switch(message.msgId)
        {
            case DASH_MSG_ID:
                handle_dash_msg(message.data);
                break;
                
            case ACAN_MSG_ID:
                handle_acan_message(message.data);
                break;
        }
    }
}

void CAN_Msg_Send(uint16_t id, CAN_DLC dlc, uint8_t *tx_data)
{
    CAN_MSG_FIELD overhead = {
        .idType = CAN_FRAME_STD,
        .frameType = CAN_FRAME_DATA,
        .dlc = dlc,
    };

    CAN_MSG_OBJ msg = {
        .msgId = id,
        .field = overhead,
        .data = tx_data,
    };

    CAN1_Transmit(CAN_PRIORITY_MEDIUM, &msg);
}
