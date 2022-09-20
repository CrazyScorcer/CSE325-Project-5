/*
 * ColorSenor.c
 *
 *  Created on: Apr 22, 2022
 *      Author: Eric Wu
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL46Z4.h"
#include "fsl_debug_console.h"
#include "ColorSensor.h"
#include "Utilities.h"

void clearStatusFlags() {
   // Clear STOPF and Undocumented STARTF bit 4 in Filter Register
	I2C0->FLT |= (1<<6 | 1<<4);
    // Clear ARBL and IICIF bits in Status Register
	I2C0->S |= (1<<4 | 1<<1);
}

void initI2C() {
    // Enable Clock Gating for I2C module and Port
	SIM->SCGC4 |= (1<<6); //Enable I2C0
	SIM->SCGC5 |= (1<<11); //Enable Port C Clock gate control
    // Setup Pin Mode for I2C
	//Clear Mux
	PORTC->PCR[8] &= ~0x700; //SCL
	PORTC->PCR[9] &= ~0x700; //SDA
	//Set mux
	PORTC->PCR[8] |= 0x200;
	PORTC->PCR[9] |= 0x200;
    // Write 0 to all I2C registers
	I2C0->A1 &= 0;
	I2C0->F &= 0;
	I2C0->C1 &= 0;
	I2C0->D &= 0;
	I2C0->C2 &= 0;
	I2C0->FLT &= 0;
	I2C0->A2 &= 0;
    // Clear status flags
	clearStatusFlags();
    // Set I2C Divider Register (Choose a clock frequency less than 100 KHz)
	I2C0->F |= 0x25; //ICR = 320
    // Set bit to enable I2C Module
	I2C0->C1 |= 1<<7;
}

void TCFWait() {
    // Wait for TCF bit to Set in Status Register
	while (!(I2C0->S & 1<<7)) { __asm volatile ("nop");}

}

void IICIFWait() {
    // Wait for IICIF bit to Set in Status Register
	while (!(I2C0->S & 1<<1)) { __asm volatile ("nop");}
}

void SendStart() {
    // Set MST and TX bits in Control 1 Register
	I2C0->C1 |= 1<<5 | 1<<4;
}

void RepeatStart() {
    // Set MST, TX and RSTA bits in Control 1 Register
	I2C0->C1 |= 1<<5 | 1<<4 | 1<<2;
    // Wait 6 cycles
	int i = 0;
	while(i < 6){
		__asm volatile ("nop");
		i++;
	}
}

void SendStop() {
    // Clear MST, TX and TXAK bits in Control 1 Register
	I2C0->C1 &= ~(1<<5 | 1<<4 | 1<<3);
    // Wait for BUSY bit to go low in Status Register
	while (I2C0->S & 1<<5) { __asm volatile ("nop");}

}

void clearIICIF() {
    // Clear IICIF bit in Status Register
	I2C0->S |= 1<<1;
}

int RxAK() {
    // Return 1 if byte has been ACK'd. (See RXAK in Status Register)
	if(I2C0->S & 1) return 0;
	else return 1;
}

void I2C_WriteByte(uint8_t registerAddress, int data) {
	registerAddress |= 0x80;
    clearStatusFlags();
    TCFWait();
    SendStart();
    //Write Device Address, R/W = 0 to Data Register
    I2C0->D = 0x29 << 1;

    IICIFWait();

    if (!RxAK()){
        printf("NO RESPONSE - Address \n");
        SendStop();
        return;
    }

    clearIICIF();
    //Write Register address to Data Register
    I2C0->D = registerAddress;

    IICIFWait();

    if (!RxAK()){
        printf("NO RESPONSE - Register \n");
        SendStop();
        return;
    }

    TCFWait();
    clearIICIF();

    //Write Data byte to Data Register
    I2C0->D = data;
    IICIFWait();

    if (!RxAK()){
        printf("Incorrect ACK - Data \n");
    }

    clearIICIF();
    SendStop();
}

void Read_Block(uint8_t registerAddress, uint8_t* destinationDataByteArray, int length) {
	registerAddress |= 0x80;
	unsigned char dummy = 0;

    clearStatusFlags();
    TCFWait();
    SendStart();

    dummy++;  // Do something to suppress the warning.

    // Write Device Address, R/W = 0 to Data Register
    I2C0->D = 0x29 << 1;

    IICIFWait();

    if (!RxAK()){
        printf("NO RESPONSE - Address \n");
        SendStop();
        return;
    }

    clearIICIF();

    // Write Register address to Data Register
    I2C0->D = registerAddress;

    IICIFWait();

    if (!RxAK()){
        printf("NO RESPONSE - Register \n");
        SendStop();
        return;
    }

    clearIICIF();
    RepeatStart();

    // Write device address again, R/W = 1 to Data Register
    I2C0->D = (0x29 << 1) | 0x01;

    IICIFWait();

    if (!RxAK()){
        printf("NO RESPONSE - Restart \n");
        SendStop();
        return;
    }

    TCFWait();
    clearIICIF();

    // Switch to RX by clearing TX and TXAK bits in Control 1 register
    I2C0->C1 &= ~(0x18);

    if(length==1){
        // Set TXAK to NACK in Control 1 - No more data!
    	I2C0->C1 |= 0x08;
    }

   dummy = I2C0->D; // Dummy Read

    for(int index=0; index<length; index++){
        IICIFWait();
        clearIICIF();
        if(index == length-2){
            // Set TXAK to NACK in Control 1 - No more data!
        	I2C0->C1 |= 0x08;

        }

       if(index == length-1){
            SendStop();
        }
        // Read Byte from Data Register into Array
       destinationDataByteArray[index] = I2C0->D;
    }
}

void setUpColorSensor(){
	initI2C();
	I2C_WriteByte(0x00, 0x01); //set PON
	delayMS(3);
	I2C_WriteByte(0x01, 0xFF); //set ATime register
}

struct colors readColor(){
	struct colors colorGroup;
	I2C_WriteByte(0x00, 0x03); //set AEN and PON
	delayMS(3);
	uint8_t byteArray[1];

	Read_Block(0x15, byteArray, 1); //Read clear high byte
	colorGroup.clear = (((unsigned short)byteArray[0]) << 8);
	Read_Block(0x14, byteArray, 1); //Ready clear low byte
	colorGroup.clear |= byteArray[0];

	Read_Block(0x17, byteArray, 1); //Read Red high byte
	colorGroup.red = (((unsigned short)byteArray[0]) << 8);
	Read_Block(0x16, byteArray, 1); //Ready Red low byte
	colorGroup.red |= byteArray[0];

	Read_Block(0x19, byteArray, 1); //Read Green high byte
	colorGroup.green = (((unsigned short)byteArray[0]) << 8);
	Read_Block(0x18, byteArray, 1); //Ready Green low byte
	colorGroup.green |= byteArray[0];

	Read_Block(0x1B, byteArray, 1); //Read Blue high byte
	colorGroup.blue = (((unsigned short)byteArray[0]) << 8);
	Read_Block(0x1A, byteArray, 1); //Ready Blue low byte
	colorGroup.blue |= byteArray[0];

	return colorGroup;
}

char* colorParse(){
	struct colors colorGroup = readColor();
	if (colorGroup.clear > 70 && colorGroup.blue > 20) return "White";
	else if (colorGroup.clear > 70 && colorGroup.blue < 20) return "Yellow";
	else if (colorGroup.red > colorGroup.green && colorGroup.red > colorGroup.blue) return "Red";
	else if (colorGroup.green > colorGroup.red && colorGroup.green > colorGroup.blue) return "Green";
	else if (colorGroup.blue > colorGroup.red && colorGroup.blue > colorGroup.green) return "Blue";
	else return "";
}

char* colorSampler(int count){
	int whiteCount = 0;
	int yellowCount = 0;
	int redCount = 0;
	int greenCount = 0;
	int blueCount = 0;
	for(int i = 0; i < count; i++){
		char* color = colorParse();
		if (strcmp(color, "White") == 0) whiteCount++;
		else if (strcmp(color, "Yellow") == 0) yellowCount++;
		else if (strcmp(color, "Red") == 0) redCount++;
		else if (strcmp(color, "Green") == 0) greenCount++;
		else blueCount++;
	}
	if (whiteCount > yellowCount && whiteCount > redCount && whiteCount > greenCount && whiteCount > blueCount) return "White";
	else if (yellowCount > redCount && yellowCount > greenCount && yellowCount > blueCount) return "Yellow";
	else if (redCount > greenCount && redCount > blueCount) return "Red";
	else if ( greenCount > blueCount) return "Green";
	else return "Blue";
}
