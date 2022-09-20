/*
 * Utilities.c
 *
 *  Created on: Feb 17, 2022
 *      Author: Eric Wu
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL46Z4.h"
#include "fsl_debug_console.h"
#include "MotorControls.h"
#include "SpeedControl.h"
#include "ServoControl.h"
#include "Utilities.h"

void PORTC_PORTD_IRQHandler(void){
	if (PORTC->PCR[3] & (1<<24)){//SW1
		PORTC->PCR[3] |= 1<<24; //clears interrupt flag
	}
	if (PORTC->PCR[12] & (1<<24)){//SW2
		PORTC->PCR[12] |= 1<<24; //clears interrupt flag
		movementStyle("Stop");

	}
}

void init_switch(){
	SIM->SCGC5 |= 1<<11;  // Enable Port C Clock
	//SW1
	PORTC->PCR[3] &= 0xF0703; // Clear First
	PORTC->PCR[3] |= 0xF0703 & ((0xA << 16) | (1 << 8) | 0x3 ); // Set MUX bits, enable pullups, interrupt on falling edge
	GPIOC->PDDR &= ~(1 << 3); // Setup Pin 3 Port C as input
	//SW2
	PORTC->PCR[12] &= 0xF0703; // Clear First
	PORTC->PCR[12] |= 0xF0703 & ((0xA << 16) | (1 << 8) | 0x3 ); // Set MUX bits, enable pullups, interrupt on falling edge
	GPIOC->PDDR &= ~(1 << 12); // Setup Pin 12 Port C as input

	NVIC_EnableIRQ(31); //enables PortC and PortD interrupt
}

void delayMS(int n){ //delay in milliseconds
	SIM->SCGC6 |= (1 << 24); //Enable TMP0
	SIM ->SOPT2 |= (0x2 << 24);
	TPM0->CONF |= (1 << 17);
	TPM0->SC = (1 << 7) | (0x07);

	TPM0-> MOD = n * 61 + n/2;

	TPM0->SC |= (1 << 3);//start clock

	while(!(TPM0->SC & 0x80)){} //wait for overflow
}

void delaySec(int n){
	while(n>0){
		delayMS(1000);
		n--;
	}
}

bool SW1Pressed(){
	return (bool)!(GPIOC->PDIR & (1 << 3));
}
bool SW2Pressed(){
	return (bool)!(GPIOC->PDIR & (1 << 12));
}

