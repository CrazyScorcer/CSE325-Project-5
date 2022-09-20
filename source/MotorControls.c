/*
 * MotorControls.c
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
#include "Utilities.h"
#include <string.h>

void init_motor(){
	init_left();
	init_right();

	SIM->SCGC6 |= (1 << 26); //Enable TPM2
	SIM->SOPT2 &= ~(0x3 << 24); //clears TPM CLock Source
	SIM->SOPT2 |= (0x2 << 24); // set TPMSRC to OSCERCLK

	//Clear MUX for PWM pins
	PORTB->PCR[2] &= ~0x700;
	PORTB->PCR[3] &= ~0x700;

	//Set MUX for PWM pins
	PORTB->PCR[2] |= (3 << 8);
	PORTB->PCR[3] |= (3 << 8);

	//Edge Align PWM
	TPM2->CONTROLS[0].CnSC  |= (0x1 << 3) | (0x1 << 5);
	TPM2->CONTROLS[1].CnSC |= (0x1 << 3) | (0x1 << 5);
	//Set MOD
	TPM2->MOD = 7999;
	//enable Debug
	TPM2->CONF |= (0x3 << 6);
	//set default cycle
	TPM2->CONTROLS[0].CnV = 7999;
	TPM2->CONTROLS[1].CnV = 7999;
	// Starts clock
	TPM2->SC |= (1 << 3);
}

void init_left(){
	SIM->SCGC5 |= (1<<10); //Enable Port B Clock gate control
	//Clear MUX for port B pins
	PORTB->PCR[0] &= ~0x700;
	PORTB->PCR[1] &= ~0x700;
	//Set MUX for port B pins
	PORTB->PCR[0] |= (1 << 8);
	PORTB->PCR[1] |= (1 << 8);
	//Set B pins to output
	GPIOB->PDDR |= (1<<0);
	GPIOB->PDDR |= (1<<1);
}

void init_right(){
	SIM->SCGC5 |= (1<<11); //Enable Port C Clock gate control
	//Clear MUX fort port C pins
	PORTC->PCR[1] &= ~0x700;
	PORTC->PCR[2] &= ~0x700;
	//Set MUX for port C pins
	PORTC->PCR[1] |= (1 << 8);
	PORTC->PCR[2] |= (1 << 8);
	//Set C pins to output
	GPIOC->PDDR |= (1<<1);
	GPIOC->PDDR |= (1<<2);
}

void motorControls(char* motor, char* direction, int speed){
	speed = speed*80-1;
	if (strcmp(motor, "Left") == 0){
		TPM2->CONTROLS[0].CnV = speed;
		if (strcmp(direction, "Forward") == 0){
			GPIOB->PSOR |= 1<<0; //Ai2
			GPIOB->PCOR |= 1<<1; //Ai1
		}
		else if(strcmp(direction, "Reverse") == 0){
			GPIOB->PCOR |= 1<<0; //Ai2
			GPIOB->PSOR |= 1<<1; //Ai1
		}
		else if(strcmp(direction, "Stop") == 0){
			GPIOB->PCOR |= 1<<0; //Ai2
			GPIOB->PCOR |= 1<<1; //Ai1
		}
	}
	else if (strcmp(motor, "Right") == 0){
		TPM2->CONTROLS[1].CnV = speed = speed;
		if (strcmp(direction, "Forward") == 0){
			GPIOC->PCOR |= 1<<1; //Bi1
			GPIOC->PSOR |= 1<<2; //Bi2
		}
		else if(strcmp(direction, "Reverse") == 0){
			GPIOC->PSOR |= 1<<1; //Bi1
			GPIOC->PCOR |= 1<<2; //Bi2
		}
		else if(strcmp(direction, "Stop") == 0){
			GPIOC->PCOR |= 1<<1; //Bi1
			GPIOC->PCOR |= 1<<2; //Bi2
		}
	}
}

void movementStyle(char* movement){
	int speed = 70;
	if (strcmp(movement,"Forward") == 0){
		setSpeed("Left", speed);
		setSpeed("Right", speed);
	}
	else if (strcmp(movement, "Stop") == 0){
		setSpeed("Left", 0);
		setSpeed("Right", 0);
		motorControls("Left", "Stop", 0);
		motorControls("Right", "Stop", 0);
	}
	else if (strcmp(movement, "LeftTurn") == 0){
		setSpeed("Left", -speed);
		setSpeed("Right", speed);
	}
	else if(strcmp(movement, "RightTurn") == 0){
		setSpeed("Left", speed);
		setSpeed("Right", -speed);
	}
	else if(strcmp(movement, "Rotate") == 0){
		motorControls("Left", "Forward", speed);
		motorControls("Right", "Reverse", speed);
	}
}


