#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL46Z4.h"
#include "fsl_debug_console.h"
#include "ServoControl.h"
void init_servo(){
	SIM->SCGC5 |= (1<<9); //Enable Port A Clock gate control
	SIM->SCGC5 |= (1<<12); //Enable Port D Clock gate control
	SIM->SCGC6 |= (1 << 25); //Enable TPM1

	SIM->SOPT2 &= ~(0x3 << 24); //clears TPM CLock Source
	SIM->SOPT2 |= (0x2 << 24); // set TPMSRC to OSCERCLK

	TPM1->MOD = 7999; //Set MOD
	TPM1->CONF |= (0x3 << 6); //enable Debug
	TPM1->CONTROLS[0].CnSC  |= (1 << 3) | (1 << 5); //Edge Align PWM
	TPM1->CONTROLS[0].CnV = 1450;//set default cycle (90 degree)

	PORTA->PCR[12] &= ~0x700; // reset mux
	PORTA->PCR[12] = (3 << 8); //set mux to Alt 3
	GPIOA->PDDR |= (1<<12); //set P12 as output

	init_sonar();

	TPM1->SC |= (1 << 3) | (0x3); //start clock and set prescaler to 8
}

void setServo(int angle){
	if(angle == 180){//450us is about 180 degrees
		TPM1->CONTROLS[0].CnV = 450;
	}
	else if(angle == 90){ // 1450us is about 90 degrees
		TPM1->CONTROLS[0].CnV = 1450;
	}
	else{ //2500us is about 0 degrees
		TPM1->CONTROLS[0].CnV = 2500;
	}
}

void init_sonar(){
	// reset mux
	PORTA->PCR[13] &= ~0x700;
	PORTD->PCR[2] &= ~0x700;
	//Set Mux GPIO
	PORTA->PCR[13] |= (1 << 8);
	PORTD->PCR[2] |= (1 << 8);

	GPIOA->PDDR &= ~(1 << 13); //set PTA13 input
	GPIOD->PDDR |= (1 << 2); //set PTD2 output
}

int getDistance(){
	//send 10 uSec pulse
	GPIOD->PSOR |= (1 << 2); //set high
	int tenuSecWait = TPM1->CNT + 11;
	while(TPM1->CNT < tenuSecWait){ __asm volatile ("nop");} // wait for tmp cnt to increase by 10
	GPIOD->PCOR |= (1 << 2); //set low
	//wait for echo high
	while(!(GPIOA->PDIR & (1 << 13))){ __asm volatile ("nop");} //wait for Echo to go High
	//measure time for echo to go low
	int tStart = TPM1->CNT;
	int overflows = 0;
	while(GPIOA->PDIR & (1 << 13)){
		if(TPM1->SC & (1 << 7)){
			TPM1->SC |= (1 << 7);
			overflows++;
		}
	}
	int tEnd = TPM1->CNT;
	//caculate distance
	int elapsed = 0;
	int MOD = 7999;
	if (overflows == 0){
		elapsed = tEnd - tStart;
	}
	else {
		elapsed = (overflows * MOD) + (MOD - tStart) + tEnd;
	}
	//590 ticks per cm
	int zeroDistance = 24230;
	int distance = (elapsed-zeroDistance)/48; //48 ticks is about 1 cm
	//distance = elapsed;
	return distance;
}

