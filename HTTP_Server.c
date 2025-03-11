/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server.c
 * Purpose: HTTP Server example
 *----------------------------------------------------------------------------*/

#include <stdio.h>

#include "main.h"

#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE

#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
//#include "Board_LED.h"                  // ::Board Support:LED
#include "lcd.h"												// Modulo que inicializa el LCD 
#include "adc.h"												// Modulo que inicializa el ADC tanto el pot1 como el pot2
#include "rtc.h"												//Modulo que inicializa el RTC y escribe la fecha y hora en el lcd
//#include "timer_sync.h"									//Timer que gestiona la actualizacion del servidor sntp y acciona el led rojo a parpadear
//////#include "Board_Buttons.h"              // ::Board Support:Buttons
//////#include "Board_ADC.h"                  // ::Board Support:A/D Converter
//////#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
//////#include "GLCD_Config.h"                // Keil.MCBSTM32F400::Board Support:Graphic LCD

// Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (1024U)
uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};

//////extern GLCD_FONT GLCD_Font_6x8;
//////extern GLCD_FONT GLCD_Font_16x24;

extern uint16_t AD_in          (uint32_t ch);
extern uint8_t  get_button     (void);
extern void     netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len);

extern bool LEDrun;
extern char lcd_text[2][20+1];
extern char rtc_text[2][20+1];

extern osThreadId_t TID_Display;
extern osThreadId_t TID_Led;
extern osThreadId_t TID_Adc;	
extern osThreadId_t TID_Rtc;
///Practica1/////////
uint8_t led_enciendo_off;
static const uint16_t pin[]= {GPIO_PIN_0, GPIO_PIN_7 ,GPIO_PIN_14};
ADC_HandleTypeDef adchandle; //handler definition
float value_pot1;
/////////////////////
///Practica2/////////
extern RTC_HandleTypeDef RtcHandle;
void Init_joy(void);
extern int Init_Timers (void);
/////////////////////


bool LEDrun;
char lcd_text[2][20+1] = { "LCD line 1",
                           "LCD line 2" };
char rtc_text[2][20+1] = { "RTC line 1",
                           "RTC line 2" };

/* Thread IDs */
osThreadId_t TID_Display;
osThreadId_t TID_Led;
osThreadId_t TID_Adc;	
osThreadId_t TID_Rtc;														 

/* Thread declarations */
static void BlinkLed (void *arg);
static void Display  (void *arg);
static void Adc  (void *arg);
static void Rtc  (void *arg);													 

__NO_RETURN void app_main (void *arg);

/* Read analog inputs */
uint16_t AD_in (uint32_t ch) {
  int32_t val = 0;

  if (ch == 0) {
		
		
//////    ADC_StartConversion();
//////    while (ADC_ConversionDone () < 0);
    val = ADC_getVoltage(&adchandle , 10 );
  }else if(ch == 1){
		val = ADC_getVoltage(&adchandle , 13 );
	}
  return ((uint16_t)val);
}

void Init_Led(void){
	GPIO_InitTypeDef GPIO_Init_Struct;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_Init_Struct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Init_Struct.Pull = GPIO_PULLUP;
	GPIO_Init_Struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
	GPIO_Init_Struct.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOB, &GPIO_Init_Struct);
	
	GPIO_Init_Struct.Pin = GPIO_PIN_7;
	HAL_GPIO_Init(GPIOB, &GPIO_Init_Struct);
	
	GPIO_Init_Struct.Pin = GPIO_PIN_14;
	HAL_GPIO_Init(GPIOB, &GPIO_Init_Struct);
	
}

void Init_joy(void){

	
	GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;	
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
//	GPIO_InitStruct.Pin = GPIO_PIN_11;//Derecho
//	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//	GPIO_InitStruct.Pin = GPIO_PIN_10;//Arriba
//	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
//	GPIO_InitStruct.Pin = GPIO_PIN_12;//Abajo
//	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
//	GPIO_InitStruct.Pin = GPIO_PIN_14;//Izquierda
//	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = GPIO_PIN_15;//Centro
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);		
	
}

///////* Read digital inputs */
//////uint8_t get_button (void) {
//////  return ((uint8_t)Buttons_GetState ());
//////}

/* IP address change notification */
void netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len) {

  (void)if_num;
  (void)val;
  (void)len;

  if (option == NET_DHCP_OPTION_IP_ADDRESS) {
    /* IP address change, trigger LCD update */
    osThreadFlagsSet (TID_Display, 0x01);
  }
}
/*----------------------------------------------------------------------------
  Thread 'ADC': ADC handler
 *---------------------------------------------------------------------------*/
static __NO_RETURN void Adc (void *arg) {
	
	
	ADC1_pins_F429ZI_config(); //specific PINS configuration
	ADC_Init_Single_Conversion(&adchandle , ADC1); //ADC1 configuration

  while(1) {
		
//		value_pot1= ADC_getVoltage(&adchandle , 10 );
		osDelay(100);
  }
}
/*----------------------------------------------------------------------------
  Thread 'Display': LCD display handler
 *---------------------------------------------------------------------------*/
static __NO_RETURN void Display (void *arg) {
	
	initTIM7();
	LCD_RESET();
	LCD_init();
	LCD_update();

  while(1) {
		
		if(osThreadFlagsWait(0x01,osFlagsWaitAny, osWaitForever )== 0x01){
			
			writeline1(lcd_text[0]);
			writeline2(lcd_text[1]);
			LCD_update();
			
		}
		osDelay(100);
  }
}

/*----------------------------------------------------------------------------
  Thread 'RTC': RTC handler
 *---------------------------------------------------------------------------*/
static __NO_RETURN void Rtc (void *arg) {
	RTC_DateTypeDef sdate;
  RTC_TimeTypeDef stime;
  
  int horas,minutos,segundos,dias,meses,anios;
	rtc_config();
//	get_time();
  while(1) {
		Escribir_hora_rtc();
    
    
    
    if(osThreadFlagsWait(0x01,osFlagsWaitAny, 10)== 0x01){
      sscanf(&rtc_text[0][0], "Hora: %02d:%02d:%02d", &horas,&minutos,&segundos);
      sscanf(&rtc_text[1][6], "%02d-%02d-%04d", &dias,&meses,&anios);
		
      stime.Hours=horas;
      stime.Minutes=minutos;
      stime.Seconds=segundos;
      sdate.Date=dias;
      sdate.Month=meses;
      sdate.Year=anios-2000;
		
      HAL_RTC_SetTime(&RtcHandle,&stime,RTC_FORMAT_BIN);
      HAL_RTC_SetDate(&RtcHandle,&sdate,RTC_FORMAT_BIN);
      
      
    }
  }
}

/*----------------------------------------------------------------------------
  Thread 'BlinkLed': Blink the LEDs on an eval board
 *---------------------------------------------------------------------------*/
static __NO_RETURN void BlinkLed (void *arg) {
	
	uint32_t x;

	
  while(1) {
		
		if(osThreadFlagsWait(0x01,osFlagsWaitAny, 10 )== 0x01){

			
//			if(led_enciendo_off & 0x01){
//				HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_0);
//			}
//			if(led_enciendo_off & 0x02){
//				HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_7);
//			}
//			if(led_enciendo_off & 0x04){
//				HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_14);
//			}
	
		for( x=0; x < 3; x++){
			if(led_enciendo_off & (1 << x)){
				HAL_GPIO_WritePin(GPIOB,pin[x],GPIO_PIN_SET);	
			}else{
				HAL_GPIO_WritePin(GPIOB,pin[x],GPIO_PIN_RESET);
			}
		}
			
			led_enciendo_off=0;
			
		}
		
		if(osThreadFlagsWait(0x02,osFlagsWaitAny, 10 )== 0x02){

			for(int i=0;i<12;i++){
				HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_0);
				osDelay(500);
			}
			
		}
		
		if(osThreadFlagsWait(0x03,osFlagsWaitAny, 10 )== 0x03){

			for(int i=0;i<6;i++){
				HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_14);
				osDelay(500);
			}
		
		}
		
		
		
		osDelay(100);
  }
}

/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
__NO_RETURN void app_main (void *arg) {
  (void)arg;

//  LED_Initialize();
	Init_Led();
	Init_joy();
  netInitialize ();
	Init_Timers();


  TID_Led     = osThreadNew (BlinkLed, NULL, NULL);
  TID_Display = osThreadNew (Display,  NULL, NULL);
	TID_Adc = osThreadNew (Adc,  NULL, NULL);
	TID_Rtc     = osThreadNew (Rtc, NULL, NULL);

  osThreadExit();
}
