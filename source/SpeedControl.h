/*
 * SpeedControl.h
 *
 *  Created on: Feb 28, 2022
 *      Author: Eric Wu
 */

#ifndef SPEEDCONTROL_H_
#define SPEEDCONTROL_H_

extern volatile int leftCount,rightCount,leftPrevCount,rightPrevCount;
void init_encoder();
void init_pits();
void setSpeed(char* motor,int speed);

#endif /* SPEEDCONTROL_H_ */
