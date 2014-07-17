#include "ETM_CAN.h"
#include "A36224.h"





// sdo indexes

#define SDO_DAN_TEST_REGISTER         0x1AAA00
#define SDO_DAN_LED_I2_A              0x1CCC00

#define SDO_DAN_DEVICE_TO_DEVICE_TEST 0x100100



unsigned char dan_byte_0 = 0;
unsigned char dan_byte_1 = 0;
unsigned char dan_byte_2 = 0;
unsigned char dan_byte_3 = 0;

unsigned char dan_byte_4 = 0;
unsigned char dan_byte_5 = 0;
unsigned char dan_byte_6 = 0;
unsigned char dan_byte_7 = 0;



// Please remember an UPLOAD is a request for the client (this program) to upload data to the server (whatever is sending the command)
// So in an upload the client (this program) should respond with the value stored in the register (assuming that reading that register is valid)


unsigned char ETMCanApplicationDefinedCommands(CanCommandStruct* can_command) {
  
  switch (can_command->sdo_index) {
    
  case SDO_DAN_TEST_REGISTER:
    if (can_command->is_upload) {
      can_command->TX_LOW_WORD_LOW_BYTE   = dan_byte_0;
      can_command->TX_LOW_WORD_HIGH_BYTE  = dan_byte_1;
      can_command->TX_HIGH_WORD_LOW_BYTE  = dan_byte_2;
      can_command->TX_HIGH_WORD_HIGH_BYTE = dan_byte_3;
    } else {
      dan_byte_0 = can_command->RX_LOW_WORD_LOW_BYTE;
      dan_byte_1 = can_command->RX_LOW_WORD_HIGH_BYTE;
      dan_byte_2 = can_command->RX_HIGH_WORD_LOW_BYTE;
      dan_byte_3 = can_command->RX_HIGH_WORD_HIGH_BYTE;
    } 
    break;
    
  case SDO_DAN_LED_I2_A:
    if (can_command->is_upload) {
      //return the value led state
      if (PIN_LED_I2_A == OLL_LED_ON) {
	can_command->TX_LOW_WORD_LOW_BYTE   = 0xFF;
	can_command->TX_LOW_WORD_HIGH_BYTE  = 0xFF;
	can_command->TX_HIGH_WORD_LOW_BYTE  = 0xFF;
	can_command->TX_HIGH_WORD_HIGH_BYTE = 0xFF;
      } else {
	can_command->TX_LOW_WORD_LOW_BYTE   = 0x00;
	can_command->TX_LOW_WORD_HIGH_BYTE  = 0x00;
	can_command->TX_HIGH_WORD_LOW_BYTE  = 0x00;
	can_command->TX_HIGH_WORD_HIGH_BYTE = 0x00;
      }
    } else {
      if (can_command->RX_LOW_WORD_LOW_BYTE == 0xFF) {
	PIN_LED_I2_A = OLL_LED_ON;
      } else {
	PIN_LED_I2_A = !OLL_LED_ON;
      }
    }
    break;
    
  case SDO_DAN_DEVICE_TO_DEVICE_TEST:
    if (can_command->is_upload) {
      can_command->TX_LOW_WORD_LOW_BYTE   = dan_byte_4;
      can_command->TX_LOW_WORD_HIGH_BYTE  = dan_byte_5;
      can_command->TX_HIGH_WORD_LOW_BYTE  = dan_byte_6;
      can_command->TX_HIGH_WORD_HIGH_BYTE = dan_byte_7;
    } else {
      dan_byte_4 = can_command->RX_LOW_WORD_LOW_BYTE;
      dan_byte_5 = can_command->RX_LOW_WORD_HIGH_BYTE;
      dan_byte_6 = can_command->RX_HIGH_WORD_LOW_BYTE;
      dan_byte_7 = can_command->RX_HIGH_WORD_HIGH_BYTE;
    } 

    break;


   /* 
  case SDO_IDX_RESET_CMD:
    if (is_upload) {
      //txData[4] = sdo_reset_cmd_active;
    } else if (data[4] == 0 || data[4] == 0xff) {
      //  	if (!sdo_reset_cmd_active && data[4])  
      //if (data[4]) { 
      //sdo_logic_reset = 1;	 // only reset fault on rising edge
      //}
      //sdo_reset_cmd_active = data[4];	
      //if (sdo_reset_cmd_active && (system_byte & SYS_BYTE_HV_ON)) {
      //LogHvControl(0); 
      //}
    }
    break;
    */
    
  default:
    can_command->TX_LOW_WORD_LOW_BYTE = 0x06;
    can_command->TX_LOW_WORD_HIGH_BYTE = 0x01;
    can_command->TX_HIGH_WORD_LOW_BYTE = 0x00;
    can_command->TX_HIGH_WORD_HIGH_BYTE = 0x00;
    return 0xFF;
    break;
    
  }
  return 0x00;
}

