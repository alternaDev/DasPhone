/*
 * delay.h
 *
 *  Created on: 23.02.2018
 *      Author: jhbru
 */

#ifndef __DELAY_H
#define __DELAY_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f10x.h"

void Delay_Init(void);
void Delay_SysTick();
void Delay_Us(uint32_t us);
void Delay_Ms(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif
