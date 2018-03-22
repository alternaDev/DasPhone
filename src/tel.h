/*
 * tel.h
 *
 *  Created on: 21.02.2018
 *      Author: jhbru
 */

#ifndef TEL_H_
#define TEL_H_

void PWR_Init();
void USART1_Init(void);
void Tel_Init();
void TEL_IRQHandler();
void Tel_On();
void Tel_Off();

#endif /* TEL_H_ */
