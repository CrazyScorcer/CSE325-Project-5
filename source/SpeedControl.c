#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL46Z4.h"
#include "fsl_debug_console.h"
#include "MotorControls.h"

volatile int leftCount = 0, rightCount = 0;
volatile int leftPrevCount = 0, rightPrevCount = 0;
volatile double leftTotalError = 0, rightTotalError = 0;
volatile int leftSetSpeed = 0, rightSetSpeed = 0; //ticks per rotation

volatile double Kp = 3;
volatile double Ki = .2;

void init_encoder(){
	SIM->SCGC5 |= (1<<9); //Enable Port A Clock gate control
	//Clear MUX
	PORTA->PCR[6] &= ~0x700;
	PORTA->PCR[7] &= ~0x700;
	PORTA->PCR[14] &= ~0x700;
	PORTA->PCR[15] &= ~0x700;
	//Set MUX for port A pins, and set interrupt to trigger on rising edge
	PORTA->PCR[6] = (1 << 8) | (0x9 << 16);
	PORTA->PCR[7] = (1 << 8);
	PORTA->PCR[14] = (1 << 8) | (0x9 << 16);
	PORTA->PCR[15] = (1 << 8);
	//Set A pins to input
	GPIOA->PDDR &= ~(1<<6);
	GPIOA->PDDR &= ~(1<<7);
	GPIOA->PDDR &= ~(1<<14);
	GPIOA->PDDR &= ~(1<<15);

	NVIC_EnableIRQ(30); //enables port A interrupt
}

void PORTA_IRQHandler(){
	if (PORTA->PCR[6] & (1<<24)){ //interrupt for left encoder A
		PORTA->PCR[6] |= (1<<24); //clears interrupt flag
		if(GPIOA->PDIR & (1<<7)){
			leftCount--;
		}
		else{
			leftCount++;
		}

	}

	if (PORTA->PCR[14] & (1<<24)){ //interrupt for right encoder A
		PORTA->PCR[14] |= (1<<24); //clears interrupt flag
		if(GPIOA->PDIR & (1<<15)){
			rightCount++;
		}
		else{
			rightCount--;
		}
	}
}

void PIT_IRQHandler(void){
	if(PIT->CHANNEL[0].TFLG & 1){ //Timer 0 Triggered
		PIT->CHANNEL[0].TFLG = 1; //clears interrupt flag
		//PI for left wheel
		if(leftSetSpeed != 0){
			double leftSpeed = (leftCount - leftPrevCount)*4.167; //ticks per sec
			double leftError = leftSetSpeed - leftSpeed;
			leftTotalError += leftError;
			double leftThrottle = Kp*leftError + Ki*leftTotalError;
			if (leftThrottle > 0)
				motorControls("Left", "Forward", abs((int)leftThrottle));
			else
				motorControls("Left", "Reverse", abs((int)leftThrottle));

			leftPrevCount = leftCount;
		}
		//PI for right wheel
		if(rightSetSpeed != 0){
			double rightSpeed = (rightCount - rightPrevCount)*4.167;
			double rightError = rightSetSpeed - rightSpeed;
			rightTotalError += rightError;
			double rightThrottle = Kp*rightError + Ki*rightTotalError;
			if (rightThrottle > 0)
				motorControls("Right", "Forward", abs((int)rightThrottle));
			else
				motorControls("Right", "Reverse", abs((int)rightThrottle));

			rightPrevCount = rightCount;
		}
	}
}

void init_pits(){
	SIM->SCGC6 |= (1<<23);
	PIT->MCR = 0x00;
	PIT->CHANNEL[0].LDVAL = 2400000; // setup timer 0 for 100 millisecond
	NVIC_EnableIRQ(22);
	PIT->CHANNEL[0].TCTRL = 0x3; // enable Timer 0 interrupts and start timer
}

void setSpeed(char* motor,int speed){
	if (strcmp(motor,"Left") == 0){
		leftSetSpeed = speed;
		leftTotalError = 0.0;
	}
	if (strcmp(motor,"Right") == 0){
		rightSetSpeed = speed;
		rightTotalError = 0.0;
	}
}

