/*
 * time.h
 *
 *  Created on: 21.02.2018
 *      Author: jhbru
 */

#include "stm32f10x_conf.h"

#ifndef TIME_H_
#define TIME_H_

typedef struct
{
  uint8_t RTC_Hours;    /*!< Specifies the RTC Time Hour.
                        This parameter must be set to a value in the 0-12 range
                        if the RTC_HourFormat_12 is selected or 0-23 range if
                        the RTC_HourFormat_24 is selected. */

  uint8_t RTC_Minutes;  /*!< Specifies the RTC Time Minutes.
                        This parameter must be set to a value in the 0-59 range. */

  uint8_t RTC_Seconds;  /*!< Specifies the RTC Time Seconds.
                        This parameter must be set to a value in the 0-59 range. */
}RTC_TimeTypeDef;

uint8_t RTC_Init();
void RTC_SetTime(RTC_TimeTypeDef* RTC_TimeStruct);
void RTC_GetTime(RTC_TimeTypeDef* RTC_TimeStruct);
void RTC_TimeStructInit(RTC_TimeTypeDef* RTC_TimeStruct);
uint8_t RTC_LoadBackup();

#endif /* TIME_H_ */
