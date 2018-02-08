//******************************************************
//**** ADC ROUTINES - HEADER FILE **************
//******************************************************
//Controller		: ATmega32 (Clock: 8 Mhz-internal)
//Compiler			: AVR-GCC (winAVR with AVRStudio-4)
//Project Version	: DL_1.0
//Author			: CC Dharmani, Chennai (India)
//			  		  www.dharmanitech.com
//Date				: 10 May 2011
//******************************************************

#ifndef _ADC_ROUTINES_H_
#define _ADC_ROUTINES_H_


#define ADC_ENABLE 					ADCSRA |= (1<<ADEN)
#define ADC_DISABLE 				ADCSRA &= 0x7F
#define ADC_START_CONVERSION		ADCSRA |= (1<<ADSC)

unsigned char temperature[7];
unsigned char voltage[7];	

void ADC_init(void);
unsigned int ADC_read(void);
void readTemperature(unsigned char);
void readVoltage(unsigned char);


#endif
