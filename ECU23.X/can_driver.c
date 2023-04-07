// Header
#include "can_driver.h"

// Libraries
#include "mcc_generated_files/can1.h"
#include "mcc_generated_files/pin_manager.h"

// Constants
#include "global_constants.h"

// Includes
#include "torque_handling.h"
#include "state_manager.h"
#include "torque_handling.h"

// Functions ----------------------------------------------------------------------------------
// Handle Message
// - Call to handle to the incoming CAN Message
// - Reads the received CAN Message and calls the appropriate handler
static void handle_message(void);

// Handle Inverter Fault Message
// - Call to handle the Inverter Fault Message
// - Writes to the car_state.inverter_fault_present
static void handle_inverter_fault_message(uint8_t* message_data);

// Handle Calibrate APPS Range Message
// - Call to handle the Calibrate APPS Range Message
// - Writes to and sets the APPS Map
static void handle_calibrate_apps_range(uint8_t* message_data);

// Handle Calibrate Brake Range Message
// - Call to handle the Calibrate Brake Range Message
// - Writes to and sets the Brake Map
static void handle_calibrate_brake_range(uint8_t* message_data);

// Handle Command Drive Configuration
// - Call to handle the Command Drive Configurate Message
// - Writes appropriate values to the Torque Handling globals
static void handle_command_drive_configuration(uint8_t* message_data);

// Handle Command Drive Start
// - Call to handle the Command Drive Start Message
// - Calls the Set Ready to Drive function
static void handle_command_drive_start(uint8_t* message_data);

// Send Message
// - Call to send a CAN Message
// - Sends the requested CAN Message when possible
// * NOTE: The CAN Message ID must be present in the MCC CAN ID filter.
static void send_message(uint16_t id, CAN_DLC dlc, uint8_t *tx_data);

// Objects ------------------------------------------------------------------------------------
static CAN_MSG_OBJ rx_message;                 // CAN Receive Object
static uint8_t rx_data[8] = {0,0,0,0,0,0,0,0}; // CAN Receive Object Data Buffer

static CAN_MSG_OBJ tx_message;                 // CAN Transmit Object
static uint8_t tx_data[8] = {0,0,0,0,0,0,0,0}; // CAN Transmit Object Data Buffer
static CAN_MSG_FIELD tx_field;                 // CAN Transmit Object Field

// Initialization -----------------------------------------------------------------------------
void initialize_CAN_driver(void)
{
    // Initialize TX/RX Objects
    rx_message.data = rx_data;
    tx_message.data = tx_data;
    
    tx_field.idType = CAN_FRAME_STD;
    tx_field.frameType = CAN_FRAME_DATA;
    
    // Initialize CAN 1
    CAN1_TransmitEnable();
    CAN1_ReceiveEnable();
    CAN1_OperationModeSet(CAN_NORMAL_OPERATION_MODE);
    
    // Set Rx Handler
    CAN1_SetRxBufferInterruptHandler(&handle_message);
    
    // Enable CAN 1
    CAN_STBY_SetLow();
}

// Message Handling ---------------------------------------------------------------------------
void handle_message(void)
{
    // Receive Message
    bool message_received = CAN1_Receive(&rx_message);
    if(message_received)
    {
        // Identify Message and Call Handler
        switch(rx_message.msgId)
        {
            case CAN_ID_INVERTER_FAULT:
                handle_inverter_fault_message(rx_message.data);
                return; 
            case CAN_ID_CALIBRATE_APPS:
                handle_calibrate_apps_range(rx_message.data);
                return;
            case CAN_ID_CALIBRATE_BRAKE:
                handle_calibrate_brake_range(rx_message.data);
                return;
            case CAN_ID_COMMAND_DRIVE_CONFIGURATION:
                handle_command_drive_configuration(rx_message.data);
                return;
            case CAN_ID_COMMAND_DRIVE_START:
                handle_command_drive_start(rx_message.data);
                return;
        }
    }
}

void handle_command_drive_start(uint8_t* message_data)
{
    // Byte 0
    if((message_data[0] & 0b1) == 0b1)
    {
        set_ready_to_drive();
    }
    else
    {
        exit_ready_to_drive();
    }
}

void handle_inverter_fault_message(uint8_t* message_data)
{
    car_state.inverter_fault_present = false;
    
    // If any byte is not 0 then a fault is present
    car_state.inverter_fault_present |= (message_data[0] != 0);
    car_state.inverter_fault_present |= (message_data[1] != 0);
    car_state.inverter_fault_present |= (message_data[2] != 0);
    car_state.inverter_fault_present |= (message_data[3] != 0);
    car_state.inverter_fault_present |= (message_data[4] != 0);
    car_state.inverter_fault_present |= (message_data[5] != 0);
    car_state.inverter_fault_present |= (message_data[6] != 0);
    car_state.inverter_fault_present |= (message_data[7] != 0);
}

void handle_calibrate_apps_range(uint8_t* message_data)
{
    // Reject Calibration when in High Voltage
    if(car_state.high_voltage_enabled) return;
    
    // Bytes 0 & 1
    apps1.real_min = (message_data[1] << 8) | message_data[0];
    // Bytes 2 & 3
    apps1.real_max = (message_data[3] << 8) | message_data[2];
    // Bytes 4 & 5
    apps2.real_min = (message_data[5] << 8) | message_data[4];
    // Bytes 5 & 6
    apps2.real_max = (message_data[7] << 8) | message_data[6];
    
    // Update Mapping
    set_apps_mapping();
}

void handle_calibrate_brake_range(uint8_t* message_data)
{
    // Reject Calibration when in High Voltage
    if(car_state.high_voltage_enabled) return;
    
    // Bytes 0 & 1
    brake1.real_min = (message_data[1] << 8) | message_data[0];
    // Bytes 2 & 3
    brake1.real_max = (message_data[3] << 8) | message_data[2];
    // Bytes 4 & 5
    brake2.real_min = (message_data[5] << 8) | message_data[4];
    // Bytes 6 & 7
    brake2.real_max = (message_data[7] << 8) | message_data[6];
    
    // Update Mapping
    set_brake_mapping();
}

static void handle_command_drive_configuration(uint8_t* message_data)
{
    torque_limit = ((message_data[1] << 8) | message_data[0]) / 10;
    regen_limit  = ((message_data[3] << 8) | message_data[2]) / 10;
}

// Message Transmitters -----------------------------------------------------------------------
void send_message(uint16_t id, CAN_DLC dlc, uint8_t *tx_data)
{
    // Set Field Data
    tx_field.dlc = dlc,

    // Set Message Data
    tx_message.msgId = id;
    tx_message.field = tx_field;
    tx_message.data = tx_data;
    
    // Send Message
    CAN1_Transmit(CAN_PRIORITY_MEDIUM, &tx_message);
}

void send_command_inverter(bool inverter_enabled, int16_t torque_x10, uint16_t torque_limit)
{
    if(inverter_enabled)
    {
        // Torque Target LO and HI Bytes
        tx_data[0] =  torque_x10       & 0xFF;
        tx_data[1] = (torque_x10 >> 8) & 0xFF;
        
        // Speed Target LO and HI Bytes (Disabled)
        tx_data[2] = 0;
        tx_data[3] = 0;
        
        // Inverter Direction Bit (0 => Forward, 1 => Reverse)
        tx_data[4] = 0;
        
        // Byte 5
        tx_data[5] = 0;
        // Inverter Enable Bit
        tx_data[5] |= (true  >> 0);
        // Standby Discharge Bit
        tx_data[5] |= (false >> 1);
        // Speed Mode Bit (0 => Torque Mode, 1 => Speed Mode)
        tx_data[5] |= (false >> 2);
        
        // Torque Limit LO and HI Bytes
        tx_data[6] = ( torque_limit * 10)       & 0xFF;
        tx_data[7] = ((torque_limit * 10) >> 8) & 0xFF;
    }
    else
    {
        // Torque Target LO and HI Bytes
        tx_data[0] = 0;
        tx_data[1] = 0;
        
        // Speed Target LO and HI Bytes (Disabled)
        tx_data[2] = 0;
        tx_data[3] = 0;
        
        // Inverter Direction Bit (0 => Forward, 1 => Reverse)
        tx_data[4] = 0;
        
        // Byte 5
        tx_data[5] = 0;
        // Inverter Enable Bit
        tx_data[5] |= (false >> 0);
        // Standby Discharge Bit
        tx_data[5] |= (false >> 1);
        // Speed Mode Bit (0 => Torque Mode, 1 => Speed Mode)
        tx_data[5] |= (false >> 2);
        
        // Torque Limit LO and HI Bytes
        tx_data[6] = 0;
        tx_data[7] = 0;
    }
    
    send_message(CAN_ID_COMMAND_INVERTER, 8, tx_data);
}

void send_status_ecu()
{
    // Byte 0
    tx_data[0] = 0;
    tx_data[0] |= (car_state.ready_to_drive       << 0);
    tx_data[0] |= (car_state.high_voltage_enabled << 1);
    tx_data[0] |= (car_state.regen_enabled        << 2);
    
    // Byte 1
    tx_data[1] = 0;
    tx_data[1] |= (car_state.torque_plausible             << 0);
    tx_data[1] |= (car_state.pedals_100ms_plausible       << 1);
    tx_data[1] |= (car_state.pedals_plausible             << 2);
    tx_data[1] |= (car_state.apps_plausible               << 3);
    tx_data[1] |= (car_state.apps_calibration_plausible   << 4);
    tx_data[1] |= (car_state.brakes_plausible             << 5);
    tx_data[1] |= (car_state.brakes_calibration_plausible << 6);
    tx_data[1] |= (car_state.apps_25_5_plausible          << 7);
    
    // Byte 2
    tx_data[2] = 0;
    tx_data[2] |= (car_state.accelerating << 0);
    tx_data[2] |= (car_state.braking      << 1);
    
    tx_data[3] = 0;
    tx_data[4] = 0;
    tx_data[5] = 0;
    tx_data[6] = 0;
    tx_data[7] = 0;
    
    send_message(CAN_ID_STATUS_ECU, 8, tx_data);
}

void send_pedal_messages()
{
    // Message 0x005 - APPS & Brakes Input
    // APPS-1 LO & HI Bytes
    tx_data[0] =  apps1.value        & 0xFF;
    tx_data[1] = (apps1.value  >> 8) & 0xFF;
    
    // APPS-2 LO & HI Bytes
    tx_data[2] =  apps2.value        & 0xFF;
    tx_data[3] = (apps2.value  >> 8) & 0xFF;
    
    // Brake-1 LO & HI Bytes
    tx_data[4] =  brake1.value       & 0xFF;
    tx_data[5] = (brake1.value >> 8) & 0xFF;
    
    // Brake-2 LO & HI Bytes
    tx_data[6] =  brake2.value       & 0xFF;
    tx_data[7] = (brake2.value >> 8) & 0xFF;
    
    send_message(CAN_ID_INPUT_PEDALS, 8, tx_data);
    
    // Message 0x701 - APPS & Brakes Percentages
    // Using 32-bit representation as to prevent overflow.
    int16_t apps1_percent_x10  = (1000 * ((int32_t)apps1.value  - apps1.real_min ) / (apps1.real_max  - apps1.real_min));
    int16_t apps2_percent_x10  = (1000 * ((int32_t)apps2.value  - apps2.real_min ) / (apps2.real_max  - apps2.real_min));
    int16_t brake1_percent_x10 = (1000 * ((int32_t)brake1.value - brake1.real_min) / (brake1.real_max - brake1.real_min));
    int16_t brake2_percent_x10 = (1000 * ((int32_t)brake2.value - brake2.real_min) / (brake2.real_max - brake2.real_min));
    
    // APPS-1 LO & HI Bytes
    tx_data[0] =  apps1_percent_x10        & 0xFF;
    tx_data[1] = (apps1_percent_x10  >> 8) & 0xFF;
    
    // APPS-2 LO & HI Bytes
    tx_data[2] =  apps2_percent_x10        & 0xFF;
    tx_data[3] = (apps2_percent_x10  >> 8) & 0xFF;
    
    // Brake-1 LO & HI Bytes
    tx_data[4] =  brake1_percent_x10       & 0xFF;
    tx_data[5] = (brake1_percent_x10 >> 8) & 0xFF;
    
    // Brake-2 LO & HI Bytes
    tx_data[6] =  brake2_percent_x10       & 0xFF;
    tx_data[7] = (brake2_percent_x10 >> 8) & 0xFF;
    
    send_message(CAN_ID_DATA_PEDALS, 8, tx_data);
}
