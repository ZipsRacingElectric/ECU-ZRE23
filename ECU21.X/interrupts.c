#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/can1.h"
#include "global_constants.h"

//Use this function for receiving CAN messages
void CAN1_DefaultReceiveBufferHandler(void) 
{
    //innitialize CAN Message holder
    uint8_t data[8] = {0,1,2,3,4,5,6,7};
    CAN_MSG_OBJ message;
    message.data = &data;
    
    CAN1_Receive(&message);
    
    switch(DASH_MSG_ID)
    {
        case(0xFF):
            LED1_Toggle();
            break;
    }
}
