#include "ETM_CAN.h"
#include <p30fxxxx.h>
#include <can.h>


#ifdef __USE_CAN_1

#define _CXIE                       _C1IE
#define _CXIF                       _C1IF
#define _CXIP                       _C1IP

#define CXTX0CON                    C1TX0CON
#define CXINTF                      C1INTF

#define CXCTRLbits                  C1CTRLbits
#define CXTX0CONbits                C1TX0CONbits
#define CXINTFbits                  C1INTFbits
#define CXRX0CONbits                C1RX0CONbits
#define CXINTEbits                  C1INTEbits

#define CANXSetOperationMode        CAN1SetOperationMode
#define CANXInitialize              CAN1Initialize
#define CANXSetFilter               CAN1SetFilter
#define CANXSetMask                 CAN1SetMask
#define CANXSetTXMode               CAN1SetTXMode
#define CANXSetRXMode               CAN1SetRXMode
#define CANXIsTXReady               CAN1IsTXReady
#define CANXSendMessage             CAN1SendMessage
#define CANXReceiveMessage          CAN1ReceiveMessage



#else

#define _CXIE                       _C2IE
#define _CXIF                       _C2IF
#define _CXIP                       _C2IP

#define CXTX0CON                    C2TX0CON
#define CXINTF                      C2INTF

#define CXCTRLbits                  C2CTRLbits
#define CXTX0CONbits                C2TX0CONbits
#define CXINTFbits                  C2INTFbits
#define CXRX0CONbits                C2RX0CONbits
#define CXINTEbits                  C2INTEbits

#define CANXSetOperationMode        CAN2SetOperationMode
#define CANXInitialize              CAN2Initialize
#define CANXSetFilter               CAN2SetFilter
#define CANXSetMask                 CAN2SetMask
#define CANXSetTXMode               CAN2SetTXMode
#define CANXSetRXMode               CAN2SetRXMode
#define CANXIsTXReady               CAN2IsTXReady
#define CANXSendMessage             CAN2SendMessage
#define CANXReceiveMessage          CAN2ReceiveMessage


#endif


void ETMCanPutResponseToBuffer(unsigned char length, unsigned char * data);
void ETMCanProcessCommand(unsigned char * data);

unsigned char etm_can_address;

unsigned char can_input_message_buffer[16][8];
unsigned char can_input_message_write_index = 0;
unsigned char can_input_message_read_index = 0;

unsigned char can_output_message_buffer[16][8];
unsigned char can_output_message_write_index = 0;
unsigned char can_output_message_read_index = 0;


#define XMIT_BASE_SID    0x580
#define RECV_BASE_SID    0x600

/*
  Can Command processing:

  Use input queue and output queue to handle can traffic
*/

#define CAN_BITRATE 	500000			 			// 500 kbps
#define CAN_NTQ 	10							// Number of Tq cycles which will make the 
												    //CAN Bit Timing .
#define BRP_VAL		0                           /* ((FCY_CLK/(2*CAN_NTQ*CAN_BITRATE))-1)  //Formulae used for C1CFG1bits.BRP */
												    // segments: sync 1Tq, prop 2Tq, phase1 3Tq, phase2 4Tq, SJW 1Tq
                                                    
/////////////////////////////////////////////////////////////////////////
// InitCan() initializes can1, optionally enabling interrupts
// so it can be used as the main console I/O port.
//
void ETMCanInitialize(void) {
  char x;  // temp number holder


  _CXIE = 0;               // disable interrupt
  CXINTF = 0;              // individual flag register cleared */
  
  _CXIF = 0;               // Clear combined IRQ C2IF 
  CXINTEbits.RXB0IE = 1;   // Enable RXB0 and RXB1 interrupt
  CXINTEbits.RXB1IE = 1;   // Enable RXB0 and RXB1 interrupt
  
  _CXIP = ETM_CAN_INTERRUPT_PRIORITY;      	   // set interrupt priority 
  
  can_input_message_write_index  = 0;
  can_input_message_read_index   = 0;
  can_output_message_write_index = 0;
  can_output_message_read_index  = 0;
  
  CANXSetOperationMode(CAN_IDLE_CON & CAN_MASTERCLOCK_1 & CAN_REQ_OPERMODE_CONFIG & CAN_CAPTURE_DIS);
  
  while(CXCTRLbits.OPMODE <= 3);
  
  /* Load configuration register */
  CANXInitialize(CAN_SYNC_JUMP_WIDTH1 & CAN_BAUD_PRE_SCALE(1), CAN_WAKEUP_BY_FILTER_DIS & CAN_PHASE_SEG2_TQ(4) & CAN_PHASE_SEG1_TQ(3) & CAN_PROPAGATIONTIME_SEG_TQ(2) & CAN_SEG2_FREE_PROG & CAN_SAMPLE1TIME);
  
  /* Load Acceptance filter register, filter0 and filter1 for RXB0, filter2-5 for RXB1 */
  for (x = 0; x <= 5; x++) {
    if (x == 1 || x == 5) {
      CANXSetFilter(x, CAN_FILTER_SID((XMIT_BASE_SID + etm_can_address)) & CAN_RX_EID_DIS, CAN_FILTER_EID(0));
    } else {
      CANXSetFilter(x, CAN_FILTER_SID((RECV_BASE_SID + etm_can_address)) & CAN_RX_EID_DIS, CAN_FILTER_EID(0));
    }
  }
  /* Load mask filter register */
  for (x = 0; x <= 1; x++) {
    CANXSetMask(x, CAN_MASK_SID(0x7ff) & CAN_MATCH_FILTER_TYPE, CAN_MASK_EID(0));
  }
  
  /* Set transmitter mode */
  for (x = 0; x <= 2; x++) {
    CANXSetTXMode(x, CAN_TX_STOP_REQ & CAN_TX_PRIORITY_HIGH );
  }
  
  /* Set receiver mode */    
  CANXSetRXMode(0,  CAN_RXFUL_CLEAR & CAN_BUF0_DBLBUFFER_EN);	 // enable double buffer, clear	buffer0 full bit
  CANXSetRXMode(1,  0);	 									 // clear buffer1 full bit
  
  
  /* Set request for Loopback mode */
  CANXSetOperationMode(CAN_IDLE_CON & CAN_CAPTURE_DIS & CAN_MASTERCLOCK_1 & CAN_REQ_OPERMODE_NOR);
  while(CXCTRLbits.OPMODE != 0);
  _CXIE = 1; // enable interrupt
  
} // InitCan()
  




void ETMCanPutResponseToBuffer(unsigned char length, unsigned char * data) {
  unsigned char i;
  
  for (i = 0; i < 8; i++) {
    can_output_message_buffer[can_output_message_write_index][i] = *(data +i); // DPARKER change this back at some point to data[i]
  }
  can_output_message_write_index++;
  can_output_message_write_index &= 0x0F;
}



/////////////////////////////////////////////////////////////////////////
// can_process_command 
// process and answer canopen commands
//

void ETMCanProcessCommand(unsigned char * data) {
  unsigned char txData[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  CanCommandStruct this_can_cmd;
  
  this_can_cmd.sdo_index = ((unsigned long)data[2] << 16) + ((unsigned long)data[1] << 8) + (unsigned long)data[3];

  if (data[0] == 0x40) {
    this_can_cmd.is_upload = 1;
    txData[0] = 0x42;
  } else {
    this_can_cmd.is_upload = 0;
    txData[0] = 0x60;
  }

  txData[1] = data[1];
  txData[2] = data[2];
  txData[3] = data[3];

  this_can_cmd.RX_0 = data[4];
  this_can_cmd.RX_1 = data[5];
  this_can_cmd.RX_2 = data[6];
  this_can_cmd.RX_3 = data[7];

  ETMCanApplicationDefinedCommands(&this_can_cmd);
  
  txData[4] = this_can_cmd.TX_0;
  txData[5] = this_can_cmd.TX_1;
  txData[6] = this_can_cmd.TX_2;
  txData[7] = this_can_cmd.TX_3;

   
  ETMCanPutResponseToBuffer(8, &txData[0]);													    
  
}



																							  
/////////////////////////////////////////////////////////////////////////					  
// ETMCanDoReadWriteBuffer 																		  
// check and process canopen commands														    
//


void ETMCanDoReadWriteBuffer(void) {

  // Look for an input command and process it.
  if (can_input_message_read_index != can_input_message_write_index) {
    // There is at least one command in the input buffer
    ETMCanProcessCommand(&can_input_message_buffer[can_input_message_read_index][0]);
    can_input_message_read_index++;
    can_input_message_read_index &= 0x0F;
  }
  
  if (can_output_message_read_index != can_output_message_write_index)   {
    if (CANXIsTXReady(0)) {
      CANXSendMessage((CAN_TX_SID((XMIT_BASE_SID + etm_can_address)) & CAN_TX_EID_DIS & CAN_SUB_NOR_TX_REQ), 0, &can_output_message_buffer[can_output_message_read_index][0], 8, 0);        
      can_output_message_read_index++;
      can_output_message_read_index &= 0x0F;
    } else  {
      // check tx error and clear the status, make buffer available
      if (CXTX0CON & 0x070) {
	CXTX0CONbits.TXREQ = 0;
      }
    }
  }
}


/////////////////////////////////////////////////////////////////////////
		//Interrupt handler for CAN2
/////////////////////////////////////////////////////////////////////////

void __attribute__((interrupt, no_auto_psv)) _C2Interrupt(void) {
  _CXIF = 0;         //Clear interrupt flag
  if(CXINTFbits.RX0IF) {      
    CXINTFbits.RX0IF = 0; 	//If the Interrupt is due to Receive0 of CAN2 Clear the Interrupt
    CANXReceiveMessage(&can_input_message_buffer[can_input_message_write_index][0], 8, 0);
    CXRX0CONbits.RXFUL = 0;
    can_input_message_write_index++;
    can_input_message_write_index &= 0x0F;
    if (can_input_message_write_index == can_input_message_read_index) {
      can_input_message_read_index++;
      can_input_message_read_index &= 0x0F;
    }
  }
}
