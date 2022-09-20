/*
 * ColorSensor.h
 *
 *  Created on: Apr 22, 2022
 *      Author: Eric Wu
 */

#ifndef COLORSENSOR_H_
#define COLORSENSOR_H_

struct colors{
	unsigned short clear;
	unsigned short red;
	unsigned short green;
	unsigned short blue;
};

void initI2C();
void clearStatusFlags();
void TCFWait();
void IICIFWait();
void SendStart();
void RepeatStart();
void SendStop();
void clearIICIF();
int RxAK();
void I2C_WriteByte(uint8_t registerAddress, int data);
void Read_Block(uint8_t registerAddress, uint8_t* destinationDataByteArray, int length);
void setUpColorSensor();
struct colors readColor();
char* colorParse();
char* colorSampler(int count);

#endif /* COLORSENSOR_H_ */
