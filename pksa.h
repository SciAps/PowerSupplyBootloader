/*********************************************************************
* FileName:        pksa.h
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

#include "typedef.h"

// pickit serial communication states
#define I2C_NO_TRANSACTION                              0
#define I2C_SLAVE_ADDRESS_RECEIVED			1
#define I2C_WORD_ADDRESS_RECEIVED			2
#define I2C_READ_ADDRESS                                3
#define I2C_READ_DATA					4
#define I2C_MASTER_NACK					5

// slave address definition
//#define SLAVE_ADDR 0xa0
#define SLAVE_ADDR 0xb0

// define states
#define MWA 0x1			//Master Writes Address
#define MWD 0x21		//Master Writes Data
#define MRA 0x5			//Master Reads Address
#define MRD 0x24		//Master Reads Data

// address typdef
extern ADDRESS	flash_addr_pointer;


// function prototypes
void I2C_Slave_Init(void);
int  do_i2c_tasks(void);

// externs
extern unsigned char nrbytes; 
extern unsigned char flash_buffer[];
extern unsigned char pksa_index;
extern unsigned char pksa_wd_address;
extern unsigned char pksa_status;
extern unsigned char pksa_cmd;



