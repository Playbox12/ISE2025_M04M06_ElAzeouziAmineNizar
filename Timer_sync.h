#ifndef __TIMER_SYNC_H
#define __TIMER_SYNC_H


#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "rtc.h" 
#include "timer_sync.h"


osTimerId_t tim_idsync;                            
static uint32_t exec1; 

int Init_Timers (void);
#endif 