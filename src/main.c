/* Includes */
#include <stddef.h>
#include <stdio.h>
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "i2c.h"
#include "oled.h"
#include "font.h"
#include "time.h"
#include "tel.h"
#include "keypad.h"

//extern int sprintf(char *buf, const char *fmt, ...);

int main(void) {
 	SystemInit();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	I2C_LowLevel_Init(500000, 0); // Standard is 400000 for fast mode, let's overclock!

	Keys_Init();

	OLED_Init();
	OLED_Fill(0x00);
	//OLED_DrawLine(0, 0, 128, 32, 0xFF);
	OLED_Puts("Peda B", &Font_7x10, 0xFF);
	OLED_PutPen(0, 10);
	OLED_Puts("Johannes M", &Font_7x10, 0xFF);
	OLED_PutPen(0, 20);
	OLED_Puts("Andy W", &Font_7x10, 0xFF);
	OLED_Display();

	RTC_TimeTypeDef time;
	if (!RTC_Init()) { // Time was not set yet, we will set it now
		RTC_TimeStructInit(&time);

		time.RTC_Hours = 13;
		time.RTC_Minutes = 30;
		time.RTC_Seconds = 15;
		RTC_SetTime(&time);
	}

	//Tel_Init();


	while (1) {
		RTC_GetTime(&time);

		OLED_Fill(0);
		OLED_PutPen(0, 10);
		char buffer[10];
		sprintf(buffer, "%02d:%02d:%02d", time.RTC_Hours, time.RTC_Minutes,
				time.RTC_Seconds);

		OLED_Puts(buffer, &Font_7x10, 0xFF);

		char key = Keys_GetKey();
		if(key != NO_KEY) {
			OLED_Putc(key, &Font_7x10, 0xFF);
		}

		OLED_Display();
		for (int i = 0; i < 100000; i++)
			;
	}

}

/*
 * Minimal __assert_func used by the assert() macro
 * */
void __assert_func(const char *file, int line, const char *func,
		const char *failedexpr) {
	while (1) {
	}
}

/*
 * Minimal __assert() uses __assert__func()
 * */
void __assert(const char *file, int line, const char *failedexpr) {
	__assert_func(file, line, NULL, failedexpr);
}
