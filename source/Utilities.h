/*
 * Utilities.h
 *
 *  Created on: Feb 17, 2022
 *      Author: Eric Wu
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <stdio.h>
void init_switch();
void init_timer();
bool SW1Pressed();
bool SW2Pressed();
void delayMS(int n);
void delaySec(int n);
#endif /* UTILITIES_H_ */
