//**************************************************************
//******** FUNCTIONS FOR SERIAL COMMUNICATION USING UART *******
//**************************************************************
//Controller		: ATmega32 (Clock: 8 Mhz-internal)
//Compiler			: AVR-GCC (winAVR with AVRStudio-4)
//Project Version	: DL_1.0
//Author			: CC Dharmani, Chennai (India)
//			  		  www.dharmanitech.com
//Date				: 10 May 2011
//**************************************************************
#define F_CPU 16000000UL
#include "UART_routines.h"
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "stdtypes.h"
//**************************************************
//UART0 initialize
//baud rate: 19200  (for controller clock = 8MHz)
//char size: 8 bit
//parity: Disabled
//**************************************************
void uart0_init(void)
{
/*
 UCSRB = 0x00; //disable while setting baud rate
 UCSRA = 0x00;
 UCSRC = (1 << URSEL) | 0x06;
 UBRRL = 0x19; //set baud rate lo
 UBRRH = 0x00; //set baud rate hi
 UCSRB = 0x18;
 */
 	u16 MYUBRR = (F_CPU/16/9600)-1;
 	/* Setting BAUD-RATE */
 	UBRR0H = (u8)(MYUBRR>>8);
 	UBRR0L = (u8)MYUBRR;
 	/* Enable receiver and transmitter */
 	UCSR0B |= (1<<RXEN0)|(1<<TXEN0);
	 
 	/* Enable receiver interrupt */
 	//UCSR0B |= (1<<RXCIE0);
 	//sei();
}

//**************************************************
//Function to receive a single byte
//*************************************************
unsigned char receiveByte( void )
{
	loop_until_bit_is_set(UCSR0A, RXC0);
	return UDR0;
}

//***************************************************
//Function to transmit a single byte
//***************************************************
void transmitByte( unsigned char data )
{
	 /* Wait for empty transmit buffer */
	 while ( !( UCSR0A & (1<<UDRE0)) );
	 /* Put data into buffer, sends the data */
	 UDR0 = data;
}


//***************************************************
//Function to transmit hex format data
//first argument indicates type: CHAR, INT or LONG
//Second argument is the data to be displayed
//***************************************************
void transmitHex( unsigned char dataType, unsigned long data )
{
unsigned char count, i, temp;
unsigned char dataString[] = "0x        ";

if (dataType == CHAR) count = 2;
if (dataType == INT) count = 4;
if (dataType == LONG) count = 8;

for(i=count; i>0; i--)
{
  temp = data % 16;
  if((temp>=0) && (temp<10)) dataString [i+1] = temp + 0x30;
  else dataString [i+1] = (temp - 10) + 0x41;

  data = data/16;
}

transmitString (dataString);
}

//***************************************************
//Function to transmit a string in Flash
//***************************************************
void transmitString_F(char* string)
{
  while (pgm_read_byte(&(*string)))
   transmitByte(pgm_read_byte(&(*string++)));
}

//***************************************************
//Function to transmit a string in RAM
//***************************************************
void transmitString(unsigned char* string)
{
  while (*string)
   transmitByte(*string++);
}

//************ END ***** www.dharmanitech.com *******
