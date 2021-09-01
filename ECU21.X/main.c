#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/can_types.h"
#include "mcc_generated_files/can1.h"

//Function Prototypes
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