/*
 * time.c
 *
 *  Created on: 21.02.2018
 *      Author: jhbru
 */

#include "time.h"
#include "stm32f10x_conf.h"

void RTC_Config() {
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);

	/* Reset Backup Domain */
	BKP_DeInit();

	/* Enable LSE */
	RCC_LSEConfig(RCC_LSE_ON);
	/* Wait till LSE is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) {
	}

	/* Select LSE as RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	/* Enable RTC Clock */
	RCC_RTCCLKCmd(ENABLE);

	/* Wait for RTC registers synchronization */
	RTC_WaitForSynchro();

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* Enable the RTC Second */
	RTC_ITConfig(RTC_IT_SEC, ENABLE);

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* Set RTC prescaler: set RTC period to 1sec */
	RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
}

uint8_t RTC_Init() {
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5) {
		RTC_Config();
		return 0;
	}
	/* Check if the Power On Reset flag is set */
	if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET) {
		//printf("\r\n\n Power On Reset occurred....");
	}
	/* Check if the Pin Reset flag is set */
	else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET) {
		//printf("\r\n\n External Reset occurred....");
	}

	/* Wait for RTC registers synchronization */
	RTC_WaitForSynchro();

	/* Enable the RTC Second */
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	return 1;

}

void RTC_GetTime(RTC_TimeTypeDef* RTC_TimeStruct) {
	uint32_t TimeVar = RTC_GetCounter();

	/* Reset RTC Counter when Time is 23:59:59 */
	if (TimeVar == 0x0001517F) {
		RTC_SetCounter(0x0);
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
	}

	/* Compute  hours */
	RTC_TimeStruct->RTC_Hours = TimeVar / 3600;
	/* Compute minutes */
	RTC_TimeStruct->RTC_Minutes = (TimeVar % 3600) / 60;
	/* Compute seconds */
	RTC_TimeStruct->RTC_Seconds = (TimeVar % 3600) % 60;
}

void RTC_SetTime(RTC_TimeTypeDef* RTC_TimeStruct) {
	RTC_SetCounter(
			((RTC_TimeStruct->RTC_Hours * 3600
					+ RTC_TimeStruct->RTC_Minutes * 60
					+ RTC_TimeStruct->RTC_Seconds)));

	RTC_WaitForLastTask();

	BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
	RTC_WaitForLastTask();
}

void RTC_TimeStructInit(RTC_TimeTypeDef* RTC_TimeStruct) {
	/* Time = 00h:00min:00sec */
	RTC_TimeStruct->RTC_Hours = 0;
	RTC_TimeStruct->RTC_Minutes = 0;
	RTC_TimeStruct->RTC_Seconds = 0;
}
