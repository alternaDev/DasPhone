/*
 * keypad.h
 *
 *  Created on: 22.02.2018
 *      Author: jhbru
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_
#include "stm32f10x_conf.h"

#define NO_KEY ' '

void Keys_Init();
char Keys_GetKey();



#endif /* KEYPAD_H_ */
