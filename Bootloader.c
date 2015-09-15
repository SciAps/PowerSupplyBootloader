/*********************************************************************
* FileName:        Bootloader.c
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
* CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER
*********************************************************************
* File Description:
*
* Change History:
* Author               Cristian Toma
********************************************************************/


#include <htc.h>
//#include <pic16F1937.h>
//#include <pic16F1527.h>
#include "main.h"
#include "delay.h"
#include "pksa.h"
#include "flash_routines.h"
#include <xc.h>
#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */
#include <stdlib.h>

//__CONFIG ( FOSC_INTOSC & WDTE_OFF & PLLEN_ON & MCLRE_OFF &  WRT_OFF) ;
  __PROG_CONFIG(1,0x2CC4);
  __PROG_CONFIG(2,0x0F12);
  
unsigned char flash_buffer[64];

unsigned char pksa_wd_address;
unsigned char pksa_index;
unsigned char pksa_status;
unsigned int counter = 0;

ADDRESS	flash_addr_pointr;

void APPF(void);


//*****************************************************************************
// 	This project must be compiled with :
//	Optimization settings : SPEED must be set  
// 	ROM Ranges = 0 - 1FF 
//	Additional command line :  -L-pstrings=CODE
//  *All values here are in hex.
//*****************************************************************************



// The bootloader code does not use any interrupts.
// However, the downloaded code may use interrupts.
// The interrupt vector on a PIC16F1937 is located at 
// address 0x0004. The following function will be located 
// at the interrupt vector and will contain a jump to
// 0x0204
void interrupt serrvice_isr()
{
	#asm
		GOTO	0x0204;
	#endasm
}	
	

void main()
{
	Initialize();
	I2C_Slave_Init();
	
	counter = 0;
	
	
	// If button is pressed, then force bootloader mode
	/*if (!BUTT)
	{
		LED_1 = 1;
		while(!BUTT);
		LED_1 = 0;
		goto App;
	}	*/
        if (flash_memory_read (UPDATE_ADDR) == 0x3F44)
        {
            //clear update flag
            flash_memory_erase (UPDATE_BLOCK_ADDR);
            goto App;
        }

	// if we have any application loaded, jump to it
	if (  flash_memory_read (0x3FFF)  == 0x3455)
	{
		#asm
			goto 0x200;
		#endasm
	}

App:

APPF();

}
// main program loop
void APPF(void)
{
    while (1)
                    {
                            do_i2c_tasks();
                            BlinkLED();
                    }
	
	
		
}		
void BlinkLED()
{
	static unsigned int counter = 0;
	counter++;
	if (counter > 0x7FFF)
        {
		PORTD = 0b00001010;
                TRISD = 0b11110001;     //Turn On LEDs
        }
	else
        {
		 PORTD = 0b00000000;
                 TRISD = 0b11110001;
        }
		
	// dummy led test
	/*if (!BUTT)
	{			
		while(!BUTT);

		LED_1 = 1;
		LED_2 = 1;	
		LED_3 = 1;
		LED_4 = 1;
		DelayMs(32);
		LED_1 = 0;
		LED_2 = 0;	
		LED_3 = 0;
		LED_4 = 0;
	}	*/
}				

void Initialize ()
{
	OSCCON = 0xFA;
        ANSELF = 0;
        ANSELF = 0;

        PORTF = 0b00001110;     //Disable Boost
        TRISF = 0b01000001;

        DelayMs(3);

        PORTC = 0b00100001;
        TRISC = 0b11011100;     //Power ON

        DelayMs(3);

        PORTD = 0b00001010;
        TRISD = 0b11110001;     //Turn On LEDs

        ANSELE = 0;
        PORTE = 0b00000100;     //+5V0
        TRISE = 0b11100011;

        /*ANSELD = 0;
	TRISD  = 0xff;
	TRISD2 = 1;
	
	TRISD1 = 0;	
	TRISE0 = 0;
	TRISE1 = 0;	
	TRISE2 = 0;	*/
}
