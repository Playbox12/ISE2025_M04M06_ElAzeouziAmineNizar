#include "stm32f4xx_hal.h"
#ifndef __ADC_H
	void ADC1_pins_F429ZI_config(void);
	int ADC_Init_Single_Conversion(ADC_HandleTypeDef *, ADC_TypeDef  *);
	int32_t ADC_getVoltage(ADC_HandleTypeDef * , uint32_t );
#endif
