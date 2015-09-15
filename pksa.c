/*********************************************************************
* FileName:        pksa.c
* Dependencies:    See INCLUDES section below
* Processor:       
* Compiler:        
* Company:         Microchip Technology, Inc.
*
* Software License Agreement:
*
* The software supplied herewith by Microchip Technology Incorporated
* (the "Company") for its PICmicro® Microcontroller is intended and
* supplied to you, the Company's customer, for use solely and
* exclusively on Microchip PICmicro Microcontroller products. The
* software is owned by the Company and/or its supplier, and is
* protected under applicable copyright laws. All rights are reserved.
* Any use in violation of the foregoing restrictions may subject the
* user to criminal sanctions under applicable laws, as well as to
* civil liability for the breach of the terms and conditions of this
* license.
*
* THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
* TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
* IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
* CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
*********************************************************************
* File Description:
*
* Change History:
* Author               Cristian Toma
********************************************************************/
#include "htc.h"
//#include <pic16F1937.h>
#include "pksa.h"
#include "main.h"
#include "flash_routines.h"
#include <stdint.h>



const	unsigned char mask = 0x25;		//I2C states mask
void I2C_Slave_Init()
{
	TRISC4 = 1;			// SDA input
	TRISC3 = 1;			// SCL input

	SSPBUF = 0x0;
	SSPSTAT = 0x80;			// 100Khz
	SSPADD = SLAVE_ADDR;	
	SSPCON1 = 0x36;			// Slave mode, 7bit addr
        SSPCON2bits.SEN = 1;
	SSPCON3 |= 0b01100000;		// enable start and stop conditions
}
void _WriteData(unsigned char data)
{

	do
	{
		SSP1CON1bits.WCOL = 0;
		SSPBUF = data;
	}
	while(SSP1CON1bits.WCOL);
	SSP1CON1bits.CKP = 1;
}

int do_i2c_tasks()
{
		unsigned int dat =0 ;
		unsigned char stat,temp,idx;

		unsigned char token = 0;		
		
		if (SSPIF)
		{
			token  = SSPSTAT & mask;	//obtain current state

			if(SSPSTATbits.S)
			{
					
					switch (token)
					{
		
						case MWA :								//MASTER WRITES ADDRESS STATE
							temp=SSPBUF;
							pksa_status=I2C_SLAVE_ADDRESS_RECEIVED;
						break;
							
						case MWD : 								//MASTER WRITES DATA STATE
							temp=SSPBUF;
							
							
							if(	pksa_status == I2C_SLAVE_ADDRESS_RECEIVED )
							{   // first time we get the slave address, after that set to word address
								pksa_wd_address = temp;
								pksa_index=0;
								pksa_status = I2C_WORD_ADDRESS_RECEIVED;
							}
							else if ( pksa_status == I2C_WORD_ADDRESS_RECEIVED )
							{	// second time we get the word address, so look into word address 
								if ( pksa_wd_address == 0x01)	// 0x01 is buffer word address
								{
									if (pksa_index == 0)
									{
										flash_addr_pointer.bytes.byte_H= temp;
										pksa_index++;
									}
									else if (pksa_index == 1)
									{
										 flash_addr_pointer.bytes.byte_L= temp;	
										
									}
								}
								if ( pksa_wd_address == 0x02 )	// 0x02 write data word address
								{
									flash_buffer[pksa_index]=temp;
									pksa_index++;
									if (pksa_index == 64)
										pksa_index--;
								}	
							}					

						break;
						
						case MRA :								//MASTER READS ADDRESS STATE
								if (pksa_wd_address == 0x01)			// buffer word address
								{	
									// Send first byte here, next byte will be send at MRD case, see below		
									_WriteData (flash_addr_pointer.bytes.byte_H);
								}
								if (pksa_wd_address == 0x03)	// read data from flash memory
								{
									if (pksa_index == 0)
									{
										//LED_1 = 1;
										// read data into flash_buffer
										for (idx = 0; idx <16; idx+=2)
										{	
											dat = flash_memory_read (flash_addr_pointer.word.address);
											flash_buffer[idx  ] = dat>>8;
											flash_buffer[idx+1] = dat & 0xFF;
											flash_addr_pointer.word.address++;
										}		
										//LED_1 = 0;
										// send first byte, the rest will be sent at MRD, see below							
										_WriteData(flash_buffer[pksa_index]);
										pksa_index++;
										if (pksa_index == 16)
											pksa_index--;	// should never get here....
									}		
								}
								if (pksa_wd_address == 0x04)
								{
									// erase command, erases a row of 32 words
									//LED_2 = 1;
									flash_memory_erase (flash_addr_pointer.word.address);
									flash_addr_pointer.word.address +=32;
									_WriteData(0x00);
									//LED_2 = 0;
								}
								if (pksa_wd_address == 0x05)
								{
									// write command. What's stored into flash_buffer is written 
									// to FLASH memory at the address pointed to by the address pointer.
									// The address pointer automatically increments by 32 units.
									//LED_3 = 1;
									flash_memory_write (flash_addr_pointer.word.address, flash_buffer );
									flash_addr_pointer.word.address +=32;
									_WriteData(0x00);
									//LED_3 = 0;
									
								}	
								if (pksa_wd_address == 0x06)
								{
									// jump to appplication code
									_WriteData(0x00);
									for ( idx =0; idx < 255; idx++ );
									#asm
										RESET;
									#endasm
								}		
						break;
						
						
						case MRD :								//MASTER READS DATA STATE
								if (pksa_wd_address == 0x01)	// buffer word address
								{		
									_WriteData (flash_addr_pointer.bytes.byte_L);
								}
								if (pksa_wd_address == 0x03)
								{
									_WriteData(flash_buffer[pksa_index]);
									pksa_index++;
									if (pksa_index == 64)
										pksa_index--;
								}								
						break;		
					}
			}
			else if(SSPSTATbits.P)
			{	//STOP state	
				asm("nop");
				pksa_status = I2C_NO_TRANSACTION; 
			}	

	
			SSPIF = 0;
			SSPCON1bits.SSPEN = 1;
			SSPCON1bits.CKP = 1;	//release clock
		}
}



