#ifndef __A36224_H
#define __A36224_H
#include <adc12.h>


#define FCY 10000000



// ------------ Define I/O Pins ---------- //
// Set up Tris and ADC Settings
#define TRISA_VALUE               0b1111111111111111
#define TRISB_VALUE               0b1111111111111111
#define TRISC_VALUE               0b1011111111100001
#define TRISD_VALUE               0b1111111111100000
#define TRISF_VALUE               0b1111111111111111
#define TRISG_VALUE               0b0000111111111101

// Set up Analog/Digital Inputs on Port B
#define A36224_ADPCFG_VALUE (ENABLE_AN2_ANA & ENABLE_AN3_ANA & ENABLE_AN4_ANA & ENABLE_AN5_ANA & ENABLE_AN6_ANA & ENABLE_AN7_ANA & ENABLE_AN8_ANA & ENABLE_AN9_ANA & ENABLE_AN10_ANA & ENABLE_AN11_ANA)
#define A36224_ADCSSL_VALUE (SKIP_SCAN_AN0 & SKIP_SCAN_AN1 & SKIP_SCAN_AN12 &  SKIP_SCAN_AN13 & SKIP_SCAN_AN14 & SKIP_SCAN_AN15)

// Digital Input Pins
#define PIN_DIGITAL_IN_0              _RB12
#define PIN_DIGITAL_IN_1              _RB13
#define PIN_DIGITAL_IN_2              _RB14
#define PIN_DIGITAL_IN_3              _RB15
#define PIN_DIGITAL_IN_4              _RA12
#define PIN_DIGITAL_IN_5              _RA14
#define ILL_DIGITAL_INPUT_ACTIVE      0

#define PIN_CAN2_RX                   _RG0
#define PIN_OPTICAL_IN                _RF3

// Digital Output Pins
#define PIN_DIGITAL_OUT_0             _LATD0
#define PIN_DIGITAL_OUT_1             _LATD1
#define PIN_DIGITAL_OUT_2             _LATD2
#define PIN_DIGITAL_OUT_3             _LATD3

#define PIN_REFRESH                   _LATD4
#define OLL_REFRESH_ACTIVE            0

#define PIN_CAN2_TX                   _LATG1
#define PIN_OPTICAL_OUT               _LATC14


#define PIN_LED_I1_A                  _LATG14
#define PIN_LED_I1_B                  _LATG12
#define PIN_LED_I1_C                  _LATG13
#define PIN_LED_I1_D                  _LATG15
#define PIN_LED_I2_A                  _LATC1
#define PIN_LED_I2_B                  _LATC2
#define PIN_LED_I2_C                  _LATC3
#define PIN_LED_I2_D                  _LATC4
#define OLL_LED_ON                    0



// configure ADC
#define A36224_ADCHS_VALUE  (ADC_CH0_POS_SAMPLEA_AN2 & ADC_CH0_NEG_SAMPLEA_VREFN & ADC_CH0_POS_SAMPLEB_AN2 & ADC_CH0_NEG_SAMPLEB_VREFN)
#define A36224_ADCON1_VALUE (ADC_MODULE_OFF & ADC_IDLE_STOP & ADC_FORMAT_INTG & ADC_CLK_AUTO & ADC_AUTO_SAMPLING_ON)
#define A36224_ADCON2_VALUE (ADC_VREF_AVDD_AVSS & ADC_SCAN_ON & ADC_SAMPLES_PER_INT_10 & ADC_ALT_BUF_OFF & ADC_ALT_INPUT_OFF)
#define A36224_ADCON3_VALUE (ADC_SAMPLE_TIME_3 & ADC_CONV_CLK_SYSTEM & ADC_CONV_CLK_7Tcy2)


#define T1_PERIOD_VALUE           (unsigned int)(FCY/256/10)
#define T1_CONFIG_VALUE           0b1000000000110000   // Timer On and 256 Prescale  

#define T2_CONFIG_VALUE           0b1000000000110000   // Timer On and 256 Prescale  
#define INPUT_CAPTURE_CONFIG      0b0000000010000101   // TMR2, Interrupt every capture, Capture Every 16th rising edge

#define T3_CONFIG_VALUE           0b1000000000110000   // Timer On and 256 Prescale  

#define _ISRFASTNOPSV __attribute__((interrupt, shadow, no_auto_psv)) 
#define _ISRNOPSV __attribute__((interrupt, no_auto_psv)) 


#endif
