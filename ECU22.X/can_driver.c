#include <stdint.h>
#include "mcc_generated_files/can1.h"
#include "mcc_generated_files/can_types.h"
#include "mcc_generated_files/pin_manager.h"
#include "global_constants.h"
#include "torque_handling.h"
#include "state_manager.h"
#include "torque_handling.h"
#include "can_driver.h"
#include "car_data.h"

static void CAN_Handle_Message_Rx(void);
static void handle_dash_msg(uint8_t* message_data);
static void handle_acan_message(uint8_t* message_data);
static void handle_inverter_fault_message(uint8_t* message_data);

static CAN_MSG_OBJ message;
static uint8_t can_data[8] = {0,0,0,0,0,0,0,0};

// dash message handler variables
static bool is_start_button_pressed = false;
static bool is_DRS_button_pressed = false;
static bool is_inverter_fault_clear_button_pressed = false;
static bool is_regen_button_pressed = false;
static VEHICLE_MODES dash_mode = NAW;
static REGEN_MODE regen_mode = INVALID;
static uint8_t inverter_fault_clear[8] = {20,0,1,0,0,0,0,0};
static uint8_t zero_torque_request[8] = {0,0,0,0,1,0, (TORQUE_MAX * 10) & 0xFF, ((TORQUE_MAX * 10) >> 8) & 0xFF};
static uint8_t prev_dash_message[2] = {0,0};

// dash led indicator variables
static uint8_t LED_state[1];

// status message variables
static uint8_t status_message[CAN_DLC_STATUS];

static uint8_t apps_and_brakes_message[CAN_DLC_AAB];

void send_apps_and_brakes();

// DRS message variable
static uint8_t DRS_command[CAN_DLC_DRS];

static uint8_t torque_percentage_message[CAN_DLC_DASH_TORQUE_PERCENTAGE];

extern struct Car_Data car_data;

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
            case CAN_ID_DASH:
                handle_dash_msg(message.data);
                break;
                
            case CAN_ID_ACAN:
                handle_acan_message(message.data);
                break;
            case CAN_ID_INVERTER_FAULT:
                handle_inverter_fault_message(message.data);
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
    // only run logic if the message data has changed
    if (message_data[0] == prev_dash_message[0] && message_data[1] == prev_dash_message[1])
    {
        return;
    }
    
    prev_dash_message[0] = message_data[0];
    prev_dash_message[1] = message_data[1];
    
    is_start_button_pressed = message_data[0] & 0x1;                        // get bit 0 of frame 0
    is_inverter_fault_clear_button_pressed = (message_data[0] >> 1) & 0x1;  // get bit 1 of frame 0
    is_DRS_button_pressed = (message_data[0] >> 2) & 0x1;                   // get bit 2 of frame 0
    is_regen_button_pressed = (message_data[0] >> 3) & 0x1;                 // get bit 3 of frame 0
    dash_mode = (message_data[0] >> 4) & 0b111;                             // get bits 4, 5, and 6 of frame 0
    regen_mode = message_data[1] & 0b111;                                // get bits 0, 1, and 2 of frame 1

    update_vehicle_mode(dash_mode);
    //update_regen_torque_mode(regen_mode);
    
    if (is_DRS_button_pressed && !car_data.is_braking)
    {
        car_data.DRS_enabled = !car_data.DRS_enabled;
    }
    
    if (is_regen_button_pressed)
    {
        car_data.regen_enabled = !car_data.regen_enabled;
    }
    
    if (is_start_button_pressed)
    {
        set_ready_to_drive();
    }
    
    if (is_inverter_fault_clear_button_pressed)
    {
        CAN_Msg_Send(CAN_ID_INVERTER_FAULT_CLEAR, CAN_DLC_INVERTER_FAULT_CLEAR, inverter_fault_clear);
        CAN_Msg_Send(CAN_ID_INVERTER_HEARTBEAT, CAN_DLC_INVERTER_HEARTBEAT, zero_torque_request);
    }
}

void handle_acan_message(uint8_t* message_data)
{
    car_data.ACAN_message_received = true;
    
    car_data.apps_1     = (message_data[1] << 8) | message_data[0];
    car_data.apps_2_raw = (message_data[3] << 8) | message_data[2];
    car_data.brake_1    = (message_data[5] << 8) | message_data[4];
    car_data.brake_2    = (message_data[7] << 8) | message_data[6];
    
    car_data.apps_2     = CALC_SCALED_APPS2(car_data.apps_2_raw);
    
    send_apps_and_brakes();
}

void handle_inverter_fault_message(uint8_t* message_data)
{
    // no fault detected
    if (message_data[0] == 0 && message_data[1] == 0 && message_data[2] == 0 && message_data[3] == 0 
            && message_data[4] == 0 && message_data[5] == 0 && message_data[6] == 0 && message_data[7] == 0)
    {
        car_data.inverter_fault_present = false;
        return;
    }
    
    car_data.inverter_fault_present = true;
}

void send_LED_indicator_state()
{
    LED_state[0] = 0;
    
    if (HV_ON_GetValue())
    {
        if (car_data.ready_to_drive)
        {
            LED_state[0] |= 0b11;
            LED_state[0] |= (!car_data.is_25_5_plausible << 4);
        }
        else
        {
            LED_state[0] |= 0b10;
            LED_state[0] |= 0b0 << 4;
        }
    }
    else
    {
        LED_state[0] |= 0b01;
    }
        
    LED_state[0] |= (car_data.inverter_fault_present << 2);
    LED_state[0] |= ((car_data.lv_battery_voltage < LOW_LV_VAL) << 3);
    

    CAN_Msg_Send(CAN_ID_LED_STATE, CAN_DLC_LED_STATE, LED_state);
}

void send_status_message()
{
    status_message[0] = car_data.lv_battery_voltage & 0xFF;
    status_message[1] = (car_data.lv_battery_voltage >> 8) & 0xFF;
    status_message[2] = car_data.IMD_resistance & 0xFF;
    status_message[3] = (car_data.IMD_resistance >> 8) & 0xFF;

    CAN_Msg_Send(CAN_ID_STATUS, CAN_DLC_STATUS, status_message);
}

void send_DRS_command()
{
    DRS_command[0] = car_data.DRS_enabled;
    CAN_Msg_Send(CAN_ID_DRS, CAN_DLC_DRS, DRS_command);
}

void send_torque_percentage_message()
{
    torque_percentage_message[0] = car_data.maximum_torque_percent;
    torque_percentage_message[1] = car_data.regen_enabled ? car_data.regen_percent : 0;
    CAN_Msg_Send(CAN_ID_DASH_TORQUE_PERCENTAGE, CAN_DLC_DASH_TORQUE_PERCENTAGE, torque_percentage_message);
}

void send_apps_and_brakes()
{
    apps_and_brakes_message[0] = 0;
    if(car_data.accelerator_is_pressed)
    {
        apps_and_brakes_message[0] |= 0b01;
    }
    if(car_data.is_braking)
    {
        apps_and_brakes_message[0] |= 0b10;
    }
    apps_and_brakes_message[1] = car_data.apps_2  & 0xFF;
    apps_and_brakes_message[2] = (car_data.apps_2 >> 8) & 0xFF;
    CAN_Msg_Send(CAN_ID_AAB, CAN_DLC_AAB, apps_and_brakes_message);
}
