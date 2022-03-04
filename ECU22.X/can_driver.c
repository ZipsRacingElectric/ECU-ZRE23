#include <stdint.h>
#include "mcc_generated_files/can1.h"
#include "mcc_generated_files/can_types.h"
#include "mcc_generated_files/pin_manager.h"
#include "global_constants.h"
#include "torque_handling.h"
#include "state_manager.h"
#include "torque_handling.h"
#include "can_driver.h"

static void CAN_Handle_Message_Rx(void);
static void handle_dash_msg(uint8_t* message_data);
static void handle_acan_message(uint8_t* message_data);

static CAN_MSG_OBJ message;
static uint8_t can_data[8] = {0,0,0,0,0,0,0,0};

// dash message handler variables
static bool is_start_pressed = false;
static bool is_reset_pressed = false;
static VEHICLE_MODES dash_mode = NAW;

// acan message handler variables
static uint16_t apps_1;
static uint16_t raw_apps_2;
static uint16_t scaled_apps_2;
static uint16_t brake_1;
static uint16_t brake_2;

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

void handle_dash_msg(uint8_t* message_data) 
{
    is_start_pressed = message_data[0] & 0x1;           // get bit 0 of frame 0
    is_reset_pressed = (message_data[0] >> 1) & 0x1;    // get bit 1 of frame 0
    dash_mode = (message_data[0] >> 4) & 0x7;           // get bits 4, 5, and 6 of frame 0

    update_vehicle_mode(dash_mode);
    
    if (is_start_pressed)
    {
        set_ready_to_drive();
    }
}

void handle_acan_message(uint8_t* message_data)
{
    apps_1 = (message_data[1] << 8) | message_data[0];
    
    raw_apps_2 = (message_data[3] << 8) | message_data[2];

    
    brake_1 = (message_data[5] << 8) | message_data[4];
    brake_2 = (message_data[7] << 8) | message_data[6];
    
    set_pedal_position_data(apps_1, raw_apps_2, brake_1, brake_2);
}
