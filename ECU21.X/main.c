#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/can_types.h"
#include "mcc_generated_files/can1.h"
#include "global_constants.h"
#include "macro_functions.h"
#include "mcc_generated_files/tmr1.h"

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
    TMR1_Initialize();
    test
    TMR1_Start();
 
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
            {
                Dash_ButtonStart = message.data[0] & 0x1;        //get the first bit of frame 0
                Dash_ButtonReset = (message.data[0] >> 1) & 0x1; //get the second bit of frame 0
                Dash_Mode = (message.data[0] >> 2) & 0x7;        //get bits 3, 4, and 5 of frame 0
                
                switch(Dash_Mode)
                {
                    case(ACCEL):
                    {                        
                        break;
                    }
                    
                    case(AUTOCROSS):
                    {
                        break;
                    }

                    case(SKIDPAD):
                    {
                        break;
                    }
                    
                    case(ENDURANCE):
                    {
                        break;
                    }
                    case(STAND):
                    {
                        break;
                    }
                    case(STATE):
                    {
                        break;
                    }
                    case(DEBUG):
                    {
                        break;
                    }
                    case(NAW):
                    {
                        break;
                    }
                }
                break;
            }
                
            case(ACAN_MSG_ID):
            {
                //Static, so these variables are not redeclared every time the interrupt is hit
                static bool isPlausible = 1;
                static bool is100MsImplausible = 0;
                static uint16_t apps1;
                static uint16_t rawApps2;
                static uint16_t scaledApps2;
                static uint16_t brake1;
                static uint16_t brake2;
                
                //apps1 = (message.data[1] << 8) | message.data[0];
                rawApps2 = (message.data[3] << 8) | message.data[2];
                scaledApps2 = CALC_SCALED_APPS2(rawApps2);
                apps1 = (message.data[5] << 8) | message.data[4]; //ACTUALLY BRAKE 1
                brake2 = (message.data[7] << 8) | message.data[6];
               
                
                //Delete this its a test
                brake1 = brake2;
                
                //APPS Plausibility check
                //The below statement should be True when implausible.
                if(apps1 < (scaledApps2 - APPS_10PERCENT_PLAUSIBILITY) || apps1 > (scaledApps2 + APPS_10PERCENT_PLAUSIBILITY) //Check for 10% Implausibility
                        || apps1 < APPS1_MIN_RANGE || apps1 > APPS1_MAX_RANGE           //Check for APPS1 out of range
                        || rawApps2 < APPS2_MIN_RANGE || rawApps2 > APPS2_MAX_RANGE)    //Check for APPS2 out of range
                {
                    if(isPlausible)
                    {
                        isPlausible = 0;
                        TMR1 = 0;
                    }
                    if(TMR1 > TMR1_100MS_VAL)
                    {
                        is100MsImplausible = 1;
                        LED1_SetLow();
                        LED2_SetHigh();
                    }
                }
                else
                {
                    isPlausible = 1;
                    is100MsImplausible = 0;
                    LED1_SetHigh();
                    LED2_SetLow();
                }
                
                //Put Torque request logic here
                
                
                break;
            }
        }
    }
}