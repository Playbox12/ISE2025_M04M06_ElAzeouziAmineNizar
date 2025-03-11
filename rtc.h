#ifndef __RTC_H
#define __RTC_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rtc.h"
#include "stdio.h"
#include "time.h"
#include "rl_net.h" 

#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */



void rtc_config(void);
void RTC_calendar_config(void);
void alarma_config(void);
void Escribir_hora_rtc(void);
void get_time (void);
static void time_cback(uint32_t seconds, uint32_t seconds_fraction);

#endif 