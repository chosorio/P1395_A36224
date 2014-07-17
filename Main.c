#include <p30F6014a.h>
#include <libpic30.h>

#include "A36224.h"
#include "MCP4822.h"
#include "ETM_CAN.h"

/* Configuration Bit Settings */
_FOSC(ECIO & CSW_FSCM_OFF); // Primary Oscillator with PLL and Startup with User Selected Oscillator Source
                            // This is for use with 40MHz ext clock
_FWDT(WDT_ON & WDTPSA_1 & WDTPSB_16);  // Watchdog Timer is enabled, 32ms TIMEOUT
_FBORPOR(PWRT_OFF & BORV_45 & PBOR_OFF & MCLR_EN); // Brown out and Power on Timer settings
_FBS(WR_PROTECT_BOOT_OFF & NO_BOOT_CODE & NO_BOOT_EEPROM & NO_BOOT_RAM);
_FSS(WR_PROT_SEC_OFF & NO_SEC_CODE & NO_SEC_EEPROM & NO_SEC_RAM);
_FGS(CODE_PROT_OFF);
_FICD(PGD);

unsigned long input_capture_2_period;
unsigned long input_capture_3_period;


unsigned int  ic2_tmr2_loops = 0;
unsigned int  ic3_tmr2_loops = 0;

unsigned int  ic2_previous_reading;
unsigned int  ic3_previous_reading;

#define IC_PERIOD_240HZ          2604



unsigned int flow_0;
unsigned int flow_1;
unsigned int flow_2;
unsigned int flow_3;
unsigned int flow_4;
unsigned int flow_5;

unsigned int analog_0;
unsigned int analog_1;
unsigned int analog_2;
unsigned int analog_3;


unsigned int refresh_enabled;
unsigned int timer_roll;
unsigned int dac_ramp = 0;

MCP4822 U42_MCP4822;
MCP4822 U44_MCP4822;

int main(void) {

  unsigned long flow_long;
  unsigned int flow_int;


  // Configure ADC Interrupt
  _ADIE = 0;
  _ADIF = 0;
  _ADIP = 6;

  // Configure T1 Interrupt
  _T1IE = 0;
  _T1IF = 0;
  _T1IP = 7;

  // Initialize IO Ports
  TRISA = TRISA_VALUE;
  TRISB = TRISB_VALUE;
  TRISC = TRISC_VALUE;
  TRISD = TRISD_VALUE;
  TRISF = TRISF_VALUE;  
  TRISG = TRISG_VALUE;


  // Initialize ADC Convert
  ADPCFG = A36224_ADPCFG_VALUE;
  ADCSSL = A36224_ADCSSL_VALUE;
  ADCHS  = A36224_ADCHS_VALUE;
  ADCON1 = A36224_ADCON1_VALUE;
  ADCON2 = A36224_ADCON2_VALUE;
  ADCON3 = A36224_ADCON3_VALUE;
  ADCON1bits.ADON = 1;
  _ADIF = 0;
  _ADIE = 1;


  // Configure TMR2
  PR1 = 0xFFFF;
  _T2IF = 0;  
  _T2IE = 1;
  _T2IP = 4;
  T2CON = T2_CONFIG_VALUE;       // Will roll at 1.677722 Second Intervals
  



  // Configure Input Capture Module
  IC3CON = INPUT_CAPTURE_CONFIG;
  IC2CON = INPUT_CAPTURE_CONFIG;

  _IC2IP = 3;
  _IC3IP = 3;

  _IC2IF = 0;
  _IC3IF = 0;
  
  _IC3IE = 1;
  _IC2IE = 1;

  
  // Initialize Timer 1 (100ms Interrupt)
  PR1 = T1_PERIOD_VALUE;
  T1CON = T1_CONFIG_VALUE;
  _T1IF = 0;
  _T1IE = 1;
  
  // DPARKER Initialize DACs
  U42_MCP4822.pin_chip_select_not = _PIN_RD14;
  U42_MCP4822.pin_load_dac_not = _PIN_RF5;
  U42_MCP4822.spi_port = ETM_SPI_PORT_2;
  U42_MCP4822.spi_con1_value = MCP4822_SPI_CON_VALUE;
  U42_MCP4822.spi_con2_value = MCP4822_SPI_CON2_VALUE;
  U42_MCP4822.spi_stat_value = MCP4822_SPI_STAT_VALUE;
  U42_MCP4822.spi_bit_rate = 100000;
  U42_MCP4822.fcy_clk = FCY;
  SetupMCP4822(&U42_MCP4822);

  U44_MCP4822.pin_chip_select_not = _PIN_RD15;
  U44_MCP4822.pin_load_dac_not = _PIN_RF5;
  U44_MCP4822.spi_port = ETM_SPI_PORT_2;
  U44_MCP4822.spi_con1_value = MCP4822_SPI_CON_VALUE;
  U44_MCP4822.spi_con2_value = MCP4822_SPI_CON2_VALUE;
  U44_MCP4822.spi_stat_value = MCP4822_SPI_STAT_VALUE;
  U44_MCP4822.spi_bit_rate = 100000;
  U44_MCP4822.fcy_clk = FCY;
  SetupMCP4822(&U44_MCP4822);



  ETMCanInitialize(73);

  while (1) {
    ETMCanDoReadWriteBuffer();
    
    
    ClrWdt();
    // Indicate the PIC is powered by flashing the power LED
    if (timer_roll) {
      timer_roll = 0;
      if (PIN_LED_I1_A == OLL_LED_ON) {
	PIN_LED_I1_A = !OLL_LED_ON;
      } else {
	PIN_LED_I1_A = OLL_LED_ON;
      }
    }
    
    // Test digital inputs
    if (PIN_DIGITAL_IN_0 == ILL_DIGITAL_INPUT_ACTIVE) {
      //PIN_LED_I2_A = OLL_LED_ON;
    } else {
      //PIN_LED_I2_A = !OLL_LED_ON;
    }
    

    if (PIN_DIGITAL_IN_1 == ILL_DIGITAL_INPUT_ACTIVE) {
      PIN_LED_I2_B = OLL_LED_ON;
    } else {
      PIN_LED_I2_B = !OLL_LED_ON;
    }

    
    if (PIN_DIGITAL_IN_2 == ILL_DIGITAL_INPUT_ACTIVE) {
      PIN_LED_I2_C = OLL_LED_ON;
    } else {
      PIN_LED_I2_C = !OLL_LED_ON;
    }


    if (PIN_DIGITAL_IN_3 == ILL_DIGITAL_INPUT_ACTIVE) {
      PIN_LED_I2_D = OLL_LED_ON;
    } else {
      PIN_LED_I2_D = !OLL_LED_ON;
    }


    if (PIN_DIGITAL_IN_4 == ILL_DIGITAL_INPUT_ACTIVE) {
      PIN_CAN2_TX = 0;
    } else {
      PIN_CAN2_TX = 1;
    }


    if (PIN_DIGITAL_IN_5 == ILL_DIGITAL_INPUT_ACTIVE) {
      PIN_OPTICAL_OUT = 1;
    } else {
      PIN_OPTICAL_OUT = 0;
    }

    

    // Test Digital Outputs
    if (PIN_DIGITAL_IN_0 == ILL_DIGITAL_INPUT_ACTIVE) {
      //PIN_DIGITAL_OUT_0 = 1;
    } else {
      //PIN_DIGITAL_OUT_0 = 0;
    }

    if (PIN_DIGITAL_IN_1 == ILL_DIGITAL_INPUT_ACTIVE) {
      PIN_DIGITAL_OUT_1 = 1;
    } else {
      PIN_DIGITAL_OUT_1 = 0;
    }

    if (PIN_DIGITAL_IN_2 == ILL_DIGITAL_INPUT_ACTIVE) {
      PIN_DIGITAL_OUT_2 = 1;
    } else {
      PIN_DIGITAL_OUT_2 = 0;
    }

    if (PIN_DIGITAL_IN_3 == ILL_DIGITAL_INPUT_ACTIVE) {
      PIN_DIGITAL_OUT_3 = 1;
    } else {
      PIN_DIGITAL_OUT_3 = 0;
    }

    if ((PIN_DIGITAL_IN_4 == ILL_DIGITAL_INPUT_ACTIVE) && (PIN_DIGITAL_IN_5 == ILL_DIGITAL_INPUT_ACTIVE)) {
      PIN_LED_I1_B = OLL_LED_ON;
      refresh_enabled = 1;
    } else {
      PIN_LED_I1_B = !OLL_LED_ON;
      refresh_enabled = 0;
    }

    if (PIN_CAN2_RX == 1) {
      PIN_LED_I1_C = OLL_LED_ON;
    } else {
      PIN_LED_I1_C = !OLL_LED_ON;
    }

    if (PIN_OPTICAL_IN == 1) {
      PIN_LED_I1_D = OLL_LED_ON;
    } else {
      PIN_LED_I1_D = !OLL_LED_ON;
    }


    /*

    // Test A/D and D/A Conversions
#define MCP4822_A_ON        (MCP4822_OUTPUT_A | MCP4822_GAIN_2X | MCP4822_OUTPUT_ENABLED) 
#define MCP4822_B_ON        (MCP4822_OUTPUT_B | MCP4822_GAIN_2X | MCP4822_OUTPUT_ENABLED) 
    
   
    WriteMCP4822(&U42_MCP4822, MCP4822_A_ON, analog_0);

    flow_long = IC_PERIOD_240HZ;
    flow_long <<= 12;
    flow_long /= input_capture_3_period;
    if (flow_long >= 0x0FFF) {
      flow_long = 0x0FFF;
    }
    flow_int = flow_long;
    WriteMCP4822(&U42_MCP4822, MCP4822_B_ON, flow_int);


    WriteMCP4822(&U44_MCP4822, MCP4822_A_ON, flow_0);  

    WriteMCP4822(&U44_MCP4822, MCP4822_B_ON, dac_ramp);
    */
  }
}


// This is set up for .1 Second Timer
void _ISRNOPSV _T1Interrupt(void) {
  // Send out refresh pulse
  _T1IF = 0;
  if (refresh_enabled) {
    PIN_REFRESH = OLL_REFRESH_ACTIVE;
    __delay32(100);
    PIN_REFRESH = !OLL_REFRESH_ACTIVE;
  }
  dac_ramp += 16;
  dac_ramp &= 0x0FFF;
  
  timer_roll = 1;
}


void _ISRNOPSV _IC2Interrupt(void) {
  unsigned int timer_reading;

  _IC2IF = 0;

  timer_reading = IC2BUF;
  input_capture_2_period = ic2_tmr2_loops;
  input_capture_2_period *= PR2;
  input_capture_2_period += timer_reading;
  if (input_capture_2_period > ic2_previous_reading) {
    input_capture_2_period -= ic2_previous_reading;
  }

  ic2_previous_reading = timer_reading;
  ic2_tmr2_loops = 0;
}

void _ISRNOPSV _IC3Interrupt(void) {
  unsigned int timer_reading;

  _IC3IF = 0;

  timer_reading = IC3BUF;
  input_capture_3_period = ic3_tmr2_loops;
  input_capture_3_period *= PR2;
  input_capture_3_period += timer_reading;
  if (input_capture_3_period > ic3_previous_reading) {
    input_capture_3_period -= ic3_previous_reading;
  }

  ic3_previous_reading = timer_reading;
  ic3_tmr2_loops = 0;
}

void _ISRNOPSV _T2Interrupt(void) {		
  unsigned long compare;
  _T2IF = 0;
  

  compare = ic2_tmr2_loops;
  compare *= PR2;
  if (input_capture_2_period <= compare) {
    input_capture_2_period = compare;
  }

  compare = ic3_tmr2_loops;
  compare *= PR2;
  if (input_capture_3_period <= compare) {
    input_capture_3_period = compare;
  }



  ic2_tmr2_loops++;
  if (ic2_tmr2_loops > 0xF0) {
    ic2_tmr2_loops = 0xF0;
  }

  ic3_tmr2_loops++;
  if (ic3_tmr2_loops > 0xF0) {
    ic3_tmr2_loops = 0xF0;
  }

  if (IC2CONbits.ICOV) {
    IC2CON = 0x0000;
    IC2CON = INPUT_CAPTURE_CONFIG;
  }

  if (IC3CONbits.ICOV) {
    IC3CON = 0x0000;
    IC3CON = INPUT_CAPTURE_CONFIG;
  }

}  


void _ISRNOPSV _ADCInterrupt(void) {
  _ASAM = 0; // Stop Auto Sampling
  _ADIF = 0;
  
  // Copy Data From Buffer to RAM
  flow_0 = ADCBUF0;
  flow_1 = ADCBUF1;
  flow_2 = ADCBUF2;
  flow_3 = ADCBUF3;
  flow_4 = ADCBUF4;
  flow_5 = ADCBUF5;  

  analog_0 = ADCBUF6;
  analog_1 = ADCBUF7;
  analog_2 = ADCBUF8;
  analog_3 = ADCBUF9;
  

  _ASAM = 1; // Re-Start Auto Sampling
}

