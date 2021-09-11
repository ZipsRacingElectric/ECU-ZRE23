#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/can_types.h"
#include "mcc_generated_files/can1.h"
#include "global_constants.h"

//Function Prototypes
void CAN1_DefaultReceiveBufferHandler(void);

//Global Variables

//These are volatile because they could change unexpectedly due to the CAN interrupt
volatile BTN_STATE Dash_ButtonStart = INITIAL;
volatile BTN_STATE Dash_ButtonReset = INITIAL;
volatile VEHICLE_MODES Dash_Mode = NAW;

int main(void)
{
    // initialize the device
    CAN_STBY_SetLow();
    SYSTEM_Initialize();
    CAN1_TransmitEnable();
    CAN1_ReceiveEnable();
    CAN1_OperationModeSet(CAN_NORMAL_2_0_MODE);
    
    while (1)
    {
        
    }
    return 1; 
}

//Use this function for receiving CAN messages
void CAN1_DefaultReceiveBufferHandler(void) 
{
    //initialize CAN Message holder
    uint8_t data[8] = {0,1,2,3,4,5,6,7};
    CAN_MSG_OBJ message;
    message.data = &data;
    
    //Get the CAN message
    if(CAN1_Receive(&message))
    {
        switch(message.msgId)
        {
            case(DASH_MSG_ID):
                Dash_ButtonStart = message.data[0] & 0x1;        //get the first bit of frame 0
                Dash_ButtonReset = (message.data[0] >> 1) & 0x1; //get the second bit of frame 0
                Dash_Mode = (message.data[0] >> 2) & 0x7;        //get bits 3, 4, and 5 of frame 0
                
                switch(Dash_Mode)
                {
                    case(ACCEL):

                    break;
                    case(AUTOCROSS):

                    break;
                    case(SKIDPAD):

                    break;
                    case(ENDURANCE):

                    break;
                    case(STAND):

                    break;
                    case(STATE):

                    break;
                    case(DEBUG):

                    break;
                    case(NAW):

                    break;                
                }
            break;
        }
    }
}