/*
 * MotorControls.h
 *
 *  Created on: Feb 17, 2022
 *      Author: Eric Wu
 */

#ifndef MOTORCONTROLS_H_
#define MOTORCONTROLS_H_

#include <stdio.h>

void init_motor();
void init_left();
void init_right();
void motorControls(char* motor, char* direction, int speed);
void movementStyle(char* movement);
#endif /* MOTORCONTROLS_H_ */
