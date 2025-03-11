#include "rtc.h"
#include "lcd.h"
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2

RTC_HandleTypeDef RtcHandle;

extern osThreadId_t TID_Led;

char hora[80];
char fecha[80];

void rtc_config(void){
	
	/*##-1- Configure the RTC peripheral #######################################*/
  /* Configure RTC prescaler and RTC data registers */
  /* RTC configured as follows:
      - Hour Format    = Format 24
      - Asynch Prediv  = Value according to source clock
      - Synch Prediv   = Value according to source clock
      - OutPut         = Output Disable
      - OutPutPolarity = High Polarity
      - OutPutType     = Open Drain */ 
  RtcHandle.Instance = RTC; 
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  __HAL_RTC_RESET_HANDLE_STATE(&RtcHandle);
	HAL_RTC_Init(&RtcHandle);
	
	RTC_calendar_config();
	alarma_config();

}

void RTC_calendar_config(void){
	
	RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;
	
	sdatestructure.Year = 0x25;
  sdatestructure.Month = RTC_MONTH_DECEMBER;
  sdatestructure.Date = 0x08;
  sdatestructure.WeekDay = RTC_WEEKDAY_SATURDAY;
	HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BCD);
	
	stimestructure.Hours = 12;
  stimestructure.Minutes = 35;
  stimestructure.Seconds = 48;
  stimestructure.TimeFormat = RTC_HOURFORMAT_24;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
	HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BIN);
}

void alarma_config(void){
	
	RTC_AlarmTypeDef salarm;
	
	salarm.Alarm=RTC_ALARM_A;
	salarm.AlarmTime.Hours=0;
	salarm.AlarmTime.Minutes=1;
	salarm.AlarmTime.Seconds=0;
//	salarm.AlarmTime.SecondFraction=20;
	salarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
//	salarm.AlarmTime.SubSeconds = 0x56;
	salarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY | RTC_ALARMMASK_HOURS | RTC_ALARMMASK_MINUTES; //RTC_ALARMMASK_SECONDS | RTC_ALARMMASK_SECONDS RTC_ALARMMASK_DATEWEEKDAY | RTC_ALARMMASK_HOURS | ;
	
	HAL_RTC_SetAlarm_IT(&RtcHandle,&salarm,RTC_FORMAT_BIN);
	
	HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn); /*Habilitacion de la alarma*/
		
}

void Escribir_hora_rtc(void){
	
	uint8_t linea1[80];
	uint8_t linea2[80];

	
	RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;
	
	HAL_RTC_GetDate(&RtcHandle,&sdatestructureget,RTC_FORMAT_BIN);
	sprintf((char *)linea2,"Fecha: %2.2d-%2.2d-%2.2d",sdatestructureget.Date, sdatestructureget.Month, 2000 + sdatestructureget.Year);
	HAL_RTC_GetTime(&RtcHandle,&stimestructureget,RTC_FORMAT_BIN);
	sprintf((char *)linea1,"Hora: %2.2d:%2.2d:%2.2d",stimestructureget.Hours,stimestructureget.Minutes,stimestructureget.Seconds);
	
	writeline1((char *)linea1);
	writeline2((char *)linea2);
	LCD_update();
	
	sprintf(hora,"Hora: %2.2d:%2.2d:%2.2d", stimestructureget.Hours,stimestructureget.Minutes,stimestructureget.Seconds);
	sprintf(fecha,"Fecha: %2.2d-%2.2d-%2.2d",sdatestructureget.Date, sdatestructureget.Month, 2000 + sdatestructureget.Year);
}

void get_time (void) {
	netSNTPc_GetTime (NULL, time_cback);
}
 
static void time_cback (uint32_t seconds, uint32_t seconds_fraction) {
		
		RTC_TimeTypeDef stimestructureweb;
		RTC_DateTypeDef sdatestructureweb;
		
		time_t rawtime = seconds;
    struct tm  ts;
    char buf[80];	

    // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
    ts = *localtime(&rawtime);
    strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
		
		sdatestructureweb.Year=(buf[6] - '0') * 10 + (buf[7] - '0');
		sdatestructureweb.Month=(buf[9] - '0') * 10 + (buf[10] - '0');
		sdatestructureweb.Date=(buf[12] - '0') * 10 + (buf[13] - '0');
	
		stimestructureweb.Hours = ts.tm_hour+1;//(buf[15] - '0') * 10 + (buf[16] - '0');
		stimestructureweb.Minutes = ts.tm_min;//(buf[18] - '0') * 10 + (buf[19] - '0');
		stimestructureweb.Seconds = ts.tm_sec;//(buf[21] - '0') * 10 + (buf[22] - '0');
	
		HAL_RTC_SetTime(&RtcHandle,&stimestructureweb,RTC_FORMAT_BIN);
		HAL_RTC_SetDate(&RtcHandle,&sdatestructureweb,RTC_FORMAT_BIN);
		
		osThreadFlagsSet (TID_Led, 0x03);
		
}



