//**************************************************************
// ****** FUNCTIONS FOR SPI COMMUNICATION *******
//**************************************************************
//Controller		: ATmega32 (Clock: 8 Mhz-internal)
//Compiler			: AVR-GCC (winAVR with AVRStudio-4)
//Project Version	: DL_1.0
//Author			: CC Dharmani, Chennai (India)
//			  		  www.dharmanitech.com
//Date				: 10 May 2011
//**************************************************************

#include <avr/io.h>
#include "SPI_routines.h"

//SPI initialize for SD card
//clock rate: 125Khz
void spi_init(void)
{
	
	// Enable SPI, Set as Master
	//- Prescaler: Fosc/16, Enable Interrupts
	//The MOSI, SCK pins
	//SPCR=(1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<SPIE);
	//SPR01 
SPCR = 0x52; //setup SPI: Master mode, MSB first, SCK phase low, SCK idle low
SPSR = 0x00;
}

unsigned char SPI_transmit(unsigned char data)
{
// Start transmission
SPDR = data;

// Wait for transmission complete
while(!(SPSR & (1<<SPIF)));
data = SPDR;

return(data);
}

unsigned char SPI_receive(void)
{
unsigned char data;
// Wait for reception complete

SPDR = 0xff;
while(!(SPSR & (1<<SPIF)));
data = SPDR;

// Return data register
return data;
}

//******** END ****** www.dharmanitech.com *****
