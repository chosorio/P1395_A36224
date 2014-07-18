#ifndef __ETM_CAN_H
#define __ETM_CAN_H


// ----- USER CONFIGURATION PARAMETERS ------- //
#define ETM_CAN_INTERRUPT_PRIORITY   3
#define __USE_CAN_2  //either __USE_CAN_1 or __USE_CAN_2 should be defined.  Defaults to can2 


typedef struct {
  unsigned long sdo_index;
  unsigned char is_upload;
  
  unsigned char RX_LOW_WORD_LOW_BYTE;
  unsigned char RX_LOW_WORD_HIGH_BYTE;
  unsigned char RX_HIGH_WORD_LOW_BYTE;
  unsigned char RX_HIGH_WORD_HIGH_BYTE;
  
  unsigned char TX_LOW_WORD_LOW_BYTE;
  unsigned char TX_LOW_WORD_HIGH_BYTE;
  unsigned char TX_HIGH_WORD_LOW_BYTE;
  unsigned char TX_HIGH_WORD_HIGH_BYTE;
} CanCommandStruct;


void ETMCanInitialize(unsigned char address);
void ETMCanDoReadWriteBuffer(void);
unsigned char ETMCanApplicationDefinedCommands(CanCommandStruct* can_command);
void ETMCanMasterTransmit(unsigned char etm_can_remote_address, unsigned int object_index, unsigned char sub_index, unsigned long data);

#endif
