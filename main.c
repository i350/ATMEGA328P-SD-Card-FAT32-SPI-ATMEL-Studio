#define F_CPU 16000000UL		//freq 16 MHz
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "SPI_routines.h"
#include "SD_routines.h"
#include "UART_routines.h"
#include "i2c_routines.h"
#include "ADC_routines.h"
#include "FAT32.h"

#define INTERVAL 1000 	//interval in milliseconds, between two measurements
						//interval defined here is approximate only, as the overhead delays
						//are not added, error is more for smaller values (i.e. <100ms)
						//minimum: 10; maximum: 600000 (600 seconds)

#define KEY_PRESSED		(!(PINC & 0x80))
#define GREEN_LED_ON	PORTC |= 0x20
#define RED_LED_ON		PORTC |= 0x40
#define RED_LED_OFF		PORTC &= ~0x40


void port_init(void)
{
  PORTB = 0x00;		  //pull-up on for pushbutton
  DDRB  = 0xef;		  //MISO line i/p, rest o/p
  PORTC = 0x80;		  //pull-up for push-button
  DDRC  = 0x60;		  //PC7- i/p (pushbutton), PC6, PC5 - o/p (two LEDs)
}


//call this routine to initialize all peripherals
void init_devices(void)
{
  cli();  //all interrupts disabled
  port_init();
  spi_init();
  uart0_init();
  //all peripherals are now initialized
}

//function to blink LED in case of any error
void blinkRedLED(void)
{
  while(1)  //blink red LED continuously, if error
  {
    RED_LED_ON;
	_delay_ms(400);
	RED_LED_OFF;
	_delay_ms(400);
  }

}

//*************************** MAIN *******************************//
int main(void)
{
  unsigned char option, error, i, j, data, channel;
  unsigned char fileName[] = "test.txt";
  unsigned int delay, k;

  _delay_ms(100);  //delay for VCC stabilization

  init_devices();

  GREEN_LED_ON;  //turn on green LED to indicate power on
  RED_LED_OFF; 	 //keep red LED off for now

  transmitString_F (PSTR("\n\r\n\r****************************************************"));
  transmitString_F (PSTR("\n\r         microSD ATMEGA328P "));
  transmitString_F (PSTR("\n\r****************************************************\n\r"));

  cardType = 0;

  for (i=0; i<10; i++)
  {
  	error = SD_init();
  	if(!error) break;
  }

  if(error)
  {
  	if(error == 1) transmitString_F(PSTR("SD card not detected.."));
  	if(error == 2) transmitString_F(PSTR("Card Initialization failed.."));
  
  	blinkRedLED();
  }

  switch (cardType)
  {
  	case 1:transmitString_F(PSTR("Standard Capacity Card (Ver 1.x) Detected!"));
  		   break;
  	case 2:transmitString_F(PSTR("High Capacity Card Detected!"));
  		   break;
  	case 3:transmitString_F(PSTR("Standard Capacity Card (Ver 2.x) Detected!"));
  		   break;
  	default:transmitString_F(PSTR("Unknown SD Card Detected!"));
  		   break; 
  }

  error = getBootSectorData (); //read boot sector and keep necessary data in global variables
  if(error) 
  {
   	transmitString_F (PSTR("\n\rFAT32 not found!"));  //FAT32 incompatible drive
   	blinkRedLED();
  }

  SPI_HIGH_SPEED;	//SCK - 4 MHz
  _delay_ms(1);   	//some delay for settling new spi speed

 //For displaying menu on hyper terminal, the key (psh-button) must be kept pressed while 
 //powering ON or while reset. If key is not kept pressed, the program will not display menu and it will 
 //simply wait for start recording command (i.e. pressing of key afterwards)


  while(1)
  {
  	transmitString_F(PSTR("\n\r\n\r> 0 : Exit the Menu"));
  	transmitString_F(PSTR("\n\r> 1 : Get file list"));
  	transmitString_F(PSTR("\n\r> 2 : Write Test File"));
  	transmitString_F(PSTR("\n\r> 3 : Read File"));
  	transmitString_F(PSTR("\n\r> 4 : Delete File"));
  	transmitString_F(PSTR("\n\r> 5 : Display Memory"));

  	transmitString_F(PSTR("\n\r\n\r> Enter the option:"));
  	option = receiveByte();
  	transmitByte(option);


	switch (option)
	{
		case '0':transmitString_F(PSTR("\n\Operation Stopped.."));
		  		 goto STOP;
		case '1':TX_NEWLINE;
		  		 findFiles(GET_LIST,0);
		  		 break;

		case '2':TX_NEWLINE;		
				//From here onwards, gather data by appending strings in dataString
				//dataString is declared in FAT32.h
				//make sure dataString doesn't exceed its MAX_STRING_SIZE, defined in FAT32.h

				for(i=0; i<10; i++) dataString[i] = 'A';
				dataString[i++] = ',';

				for(j=0;j<8; j++)  dataString[i++] = i+'0';
				dataString[i++] = ',';
				transmitString_F(PSTR("Writing ..."));
				error = writeFile("testing.txt");
				if(error) blinkRedLED();
				transmitString_F(PSTR("Done"));
				break;

		case '3':
		case '4':transmitString_F(PSTR("\n\rEnter file name: "));
          		 for(i=0; i<13; i++)
				 fileName[i] = 0x00;   //clearing any previously stored file name
          		 i=0;
          		 while(1)
          		 {
            		data = receiveByte();
            		if(data == 0x0d) break;  //'ENTER' key pressed
					if(data == 0x08)	//'Back Space' key pressed
	 				{ 
	   					if(i != 0)
	   					{ 
	     					transmitByte(data);
							transmitByte(' '); 
	     					transmitByte(data); 
	     					i--; 
	   					} 
	   					continue;     
	 				}
					if(data <0x20 || data > 0x7e) continue;  //check for valid English text character
					transmitByte(data);
            		fileName[i++] = data;
            		if(i==13){transmitString_F(PSTR(" file name too long..")); break;}
          		}
          		if(i>12) break;
       
	      		TX_NEWLINE;
		  		if(option == '3') 
				{ 
					error = readFile( READ, fileName);
					if(error == 1) transmitString_F(PSTR("File does not exist.."));
				}
		  		if(option == '4') deleteFile(fileName);
          		break;
		case '5':
				displayMemory(HIGH,getSetFreeCluster(TOTAL_FREE,GET,0));
				break;
    	default:transmitString_F(PSTR("\n\r\n\r Invalid option!\n\r"));
	}
  }

  
  while(1)
  { 
	while(1)
	{
		
	}//end of while(1)
	STOP: 
	RED_LED_OFF;  //recording stopped

  }//end of while(1)

  return 0;
		
}//end of main

  
//********** END *********** www.dharmanitech.com *************
