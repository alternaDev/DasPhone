/*
 * keypad.c
 *
 *  Created on: 22.02.2018
 *      Author: jhbru
 */

#include "keypad.h"

#define ROWS 4
#define COLS 4

char keys[ROWS][COLS] = { { '1', '2', '3', 'A' }, { '4', '5', '6', 'B' }, { '7',
		'8', '9', 'C' }, { '*', '0', '#', 'D' } };
uint16_t rowPins[ROWS] = { GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3 };
uint16_t colPins[COLS] = { GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_7 };

void Keys_Init() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2
			| GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6
			| GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	for (uint8_t c = 0; c < COLS; c++) {
		GPIO_WriteBit(GPIOA, colPins[c], Bit_SET);
	}
}

char Keys_GetKey() {
	char button = NO_KEY;
	for (uint8_t c = 0; c < COLS; c++) {
		GPIO_WriteBit(GPIOA, colPins[c], Bit_RESET);
		for (uint8_t r = 0; r < ROWS; r++) {
			uint8_t status = GPIO_ReadInputDataBit(GPIOA, rowPins[r]);
			if (status == Bit_RESET) {
				button = keys[r][c];
			}
		}
		GPIO_WriteBit(GPIOA, colPins[c], Bit_SET);
	}

	return button;
}
