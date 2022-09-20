/*
 * ServoControl.h
 *
 *  Created on: Apr 5, 2022
 *      Author: Eric Wu
 */

#ifndef SERVOCONTROL_H_
#define SERVOCONTROL_H_


void init_servo();
void setServo(int angle);
void init_sonar();
int getDistance();

#endif /* SERVOCONTROL_H_ */
