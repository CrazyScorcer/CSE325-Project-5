/*
 * LineSensor.c
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

unsigned short cal_v = 0;

void init_LineSensor(){
	SIM->SCGC5 |= 1<<13;  // Enable Port E Clock
	SIM->SCGC6 |= 1<<27; // Enable ADC0 Clock

	// Setup ADC Clock ( < 4 MHz)
	ADC0->CFG1 = 0;  // Default everything.

	// Analog Calibrate
	ADC0->SC3 = 0x07; // Enable Maximum Hardware Averaging
	ADC0->SC3 |= 0x80; // Start Calibration

	// Wait for Calibration to Complete (either COCO or CALF)
	while(!(ADC0->SC1[0] & 0x80)){ }

	// Calibration Complete, write calibration registers.
	cal_v = ADC0->CLP0 + ADC0->CLP1 + ADC0->CLP2 + ADC0->CLP3 + ADC0->CLP4 + ADC0->CLPS;
	cal_v = cal_v >> 1 | 0x8000;
	ADC0->PG = cal_v;

	cal_v = 0;
	cal_v = ADC0->CLM0 + ADC0->CLM1 + ADC0->CLM2 + ADC0->CLM3 + ADC0->CLM4 + ADC0->CLMS;
	cal_v = cal_v >> 1 | 0x8000;
	ADC0->MG = cal_v;

	ADC0->SC3 = 0; // Turn off Hardware Averaging
}

int readLineSensor(char* sensor){
	if(strcmp(sensor,"Right")) ADC0->SC1[0] = 0x05; //Set Channel 5
	else  ADC0->SC1[0] = 0x01; //Set Channel 1

	while(!(ADC0->SC1[0] & 1<<7)){} //wait for coco flag
	return ADC0->R[0]; // Reset CoCo and return light Value

}
