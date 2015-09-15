/*********************************************************************
* FileName:        flash_routines.c
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

#include <htc.h>
#include <stdint.h>  
//#include <pic16F1937.h>
volatile unsigned char key1 @ 0x070;
volatile unsigned char key2 @ 0x071;

//****************************************************************
//  FLASH MEMORY READ
//  needs 16 bit address pointer in address
//  returns 14 bit value from selected address
//
//****************************************************************
unsigned int flash_memory_read(unsigned int address) {
    PMADRL = ((address)&0xff); // load address
    PMADRH = ((address) >> 8); // load address
    CFGS = 0; // only load latches
    RD = 1;
    asm("NOP");
    asm("NOP");
    return ( (PMDATH) << 8 | (PMDATL));
}

#ifdef OLD_TOOLS
unsigned int flash_memory_read (unsigned int address)
{

	EEADRL=((address)&0xff);
	EEADRH=((address)>>8);	
	CFGS = 0;					// access FLASH program, not config
	LWLO = 0;					// only load latches	

	EEPGD = 1;
	RD = 1;
	#asm
		NOP
		NOP
	#endasm
	return ( (EEDATH)<<8 | (EEDATL) ) ;
}
#endif

//****************************************************************
//  FLASH MEMORY WRITE
//  needs 16 bit address pointer in address, 16 bit data pointer
//
//****************************************************************
void flash_memory_write (unsigned int address, unsigned char *data ) {
    unsigned char wdi, index;

    GIE = 0; // Disable Interrupts

    //PMADRL=((pmDat.pmAddr)&0xff);	// load address
    //PMADRH=((pmDat.pmAddr)>>8);         // load address
    PMADRL = (uint8_t)address;
    PMADRH = (uint16_t)(address) >> 8;

    //key1 = pmDat.pmKey1;                // Keys must be in common memory
    //key2 = pmDat.pmKey2;
     key1 = 0x55;
     key2 = 0xAA;

    FREE = 0;
    WREN = 1;

    for (wdi = 0; wdi < 63; wdi += 2) {

        PMDATH = data[wdi];
        PMDATL = data[wdi+1];

        if (wdi == 62) {
            LWLO = 0; // Start the full flash write cycle
        } else {
            LWLO = 1; // only load flash write latches
        }

        asm("movlb   3");
        asm("movf    _key1 & 0x7F,W");
        asm("movwf   PMCON2 & 0x7F");
        asm("movf    _key2 & 0x7F,W");
        asm("movwf   PMCON2 & 0x7F");
        asm("bsf     PMCON1 & 0x7F,1");
        asm("NOP");
        asm("NOP");

        PMADRL++;
    }

    WREN = 0; // disallow program/erase
    GIE = 1;

    key1 = 0;                       // Destory flash write keys  TEST
    key2 = 0;                       // Destory flash write keys  TEST
}

#ifdef OLD_TOOLS
void flash_memory_write (unsigned int address, unsigned char *data )
{
		unsigned char wdi;
		
		EECON1 = 0;
	
		EEADRL=((address)&0xff);	// load address
		EEADRH=((address)>>8);		// load address
	
		for (wdi=0;wdi<14;wdi+=2)
		{
			EEDATH = data[wdi];
			EEDATL = data[wdi+1];
		
			EEPGD = 1;					// access program space FLASH memory
			CFGS = 0;					// access FLASH program, not config
			WREN = 1;					// allow program/erase
			LWLO = 1;					// only load latches
			EECON2 = 0x55;
			EECON2 = 0xAA;
			
			
			WR = 1;						// set WR to begin write
			#asm
				NOP
				NOP
			#endasm
			
			EEADR++;
		}	

		EEDATH = data[14];
		EEDATL = data[15];
		EEPGD = 1;					// access program space FLASH memory
		CFGS = 0;					// access FLASH program, not config
		WREN = 1;					// allow program/erase
		
		LWLO = 0;					// this time start write
		EECON2 = 0x55;				
		EECON2 = 0xAA;				
		WR = 1;						// set WR to begin write
		#asm
			NOP
			NOP
		#endasm

		
		WREN = 0;					// disallow program/erase
		
}
#endif

//****************************************************************
//  FLASH MEMORY ERASE
//  Program memory can only be erased by rows. 
//  A row consists of 32 words where the EEADRL<4:0> = 0000.
//
//****************************************************************
void flash_memory_erase (unsigned int address)
{
    //GIE = 0; // Disable Interrupts
    PMADRL = (uint8_t)address;
    PMADRH = (uint16_t)(address) >> 8;
    //key1 = pmDat.pmKey1;                // Keys must be in common memory
    //key2 = pmDat.pmKey2;
     key1 = 0x55;
     key2 = 0xAA;

    CFGS = 0; // Select Program Memory
    FREE = 1; // perform an erase on next WR command, cleared by hardware
    WREN = 1; // allow program/erase

    asm("movlb   3");
    asm("movf    _key1 & 0x7F,W");
    asm("movwf   PMCON2 & 0x7F");
    asm("movf    _key2 & 0x7F,W");
    asm("movwf   PMCON2 & 0x7F");
    asm("bsf     PMCON1 & 0x7F,1");
    asm("NOP");
    asm("NOP");

    WREN = 0;
    //GIE = 1; // Disable Interrupts
}

#ifdef OLD_TOOLS
void flash_memory_erase (unsigned int address)
{
		EEADRL=((address)&0xff);	// load address
		EEADRH=((address)>>8);		// load address
		CFGS = 0;					// access FLASH program, not config
		WREN = 1;					// allow program/erase
		EEPGD = 1;					// access program space FLASH memory
		FREE = 1;					// perform an erase on next WR command, cleared by hardware
		EECON2 = 0x55;				// required sequence
		EECON2 = 0xAA;				// required sequence
		WR = 1;						// set WR to begin erase cycle
		WREN = 0;					// disallow program/erase
}
#endif