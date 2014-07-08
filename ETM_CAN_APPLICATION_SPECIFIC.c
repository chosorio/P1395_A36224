#include "ETM_CAN.h"
#include "A36224.h"





// sdo indexes

#define SDO_DAN_WRITE_VALUE     0x1AAA00
#define SDO_DAN_READ_VALUE      0x1BBB00

#define SDO_IDX_RESET_CMD       0x600A00


unsigned char dan_byte_0 = 0;
unsigned char dan_byte_1 = 0;
unsigned char dan_byte_2 = 0;
unsigned char dan_byte_3 = 0;




void ETMCanApplicationDefinedCommands(CanCommandStruct* can_command) {
  
  switch (can_command->sdo_index) {
    
  case SDO_DAN_WRITE_VALUE:
    dan_byte_0 = can_command->RX_0;
    dan_byte_1 = can_command->RX_1;
    dan_byte_2 = can_command->RX_2;
    dan_byte_3 = can_command->RX_3;
    if (dan_byte_0 > 0x40) {
      PIN_LED_I2_D = OLL_LED_ON;
    } else {
      PIN_LED_I2_D = !OLL_LED_ON;
    }
    break;
    
  case SDO_DAN_READ_VALUE:
    can_command->TX_0 = dan_byte_0;
    can_command->TX_1 = dan_byte_1;
    can_command->TX_2 = dan_byte_2;
    can_command->TX_3 = dan_byte_3;
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
    break;
    
  }
}
