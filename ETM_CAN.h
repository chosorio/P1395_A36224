#ifndef __ETM_CAN_H
#define __ETM_CAN_H


// ----- USER CONFIGURATION PARAMETERS ------- //
#define ETM_CAN_INTERRUPT_PRIORITY   3
#define __USE_CAN_2  //either __USE_CAN_1 or __USE_CAN_2 should be defined.  Defaults to can2 


typedef struct {
  unsigned long sdo_index;
  unsigned char is_upload;

  unsigned char RX_0;
  unsigned char RX_1;
  unsigned char RX_2;
  unsigned char RX_3;

  unsigned char TX_0;
  unsigned char TX_1;
  unsigned char TX_2;
  unsigned char TX_3;
} CanCommandStruct;


void ETMCanInitialize(void);
void ETMCanDoReadWriteBuffer(void);
void ETMCanApplicationDefinedCommands(CanCommandStruct* can_command);

extern unsigned char etm_can_address;
#endif
