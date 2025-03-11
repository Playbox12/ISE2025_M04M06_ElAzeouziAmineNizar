#include "lcd.h"
#include "Arial12x12.h"

extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;
ARM_SPI_STATUS stat;

TIM_HandleTypeDef tim7;

unsigned char buffer [512];

uint16_t positionL1=0;
uint16_t positionL2=0;

void initTIM7(void){
	tim7.Instance = TIM7;
	tim7.Init.Prescaler = 83;
  
	__HAL_RCC_TIM7_CLK_ENABLE();
	
	HAL_TIM_Base_Init(&tim7);	
}

void delay(uint32_t n_microsegundos){
	
	tim7.Init.Period = (n_microsegundos)-1;
	
	HAL_TIM_Base_Init(&tim7);	

	HAL_TIM_Base_Start(&tim7);

	while(__HAL_TIM_GET_FLAG(&tim7, TIM_FLAG_UPDATE) == 0);
	
	HAL_TIM_Base_Stop(&tim7);
	__HAL_TIM_CLEAR_FLAG(&tim7, TIM_FLAG_UPDATE);
	
	__HAL_TIM_SET_COUNTER(&tim7, 0);
}

void LCD_RESET(void){
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	SPIdrv->Initialize(NULL);
	SPIdrv->PowerControl(ARM_POWER_FULL);
	SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8), 20000000);
	
	
	
	__HAL_RCC_GPIOD_CLK_ENABLE();
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; 
	GPIO_InitStruct.Pin = GPIO_PIN_14; //pin CS
	HAL_GPIO_Init(GPIOD,&GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);//pin CS a 1
	
	/*pin A0*/
	__HAL_RCC_GPIOF_CLK_ENABLE();
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 
	GPIO_InitStruct.Pull = GPIO_PULLUP; 
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; 
	GPIO_InitStruct.Pin = GPIO_PIN_13; //pin A0
	HAL_GPIO_Init(GPIOF,&GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);//pin A0 a 1
	
	/*pin reset*/
	__HAL_RCC_GPIOA_CLK_ENABLE(); 
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; 
	GPIO_InitStruct.Pin = GPIO_PIN_6; //pin Reset
	HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);//pin reset a 1
	
	/////pulso de reset de 1us/////
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET); //PIN 0
	delay(1);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	delay(1000);
	
	
}



void LCD_wr_data(unsigned char data){
	
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
	
	SPIdrv->Send(&data,sizeof(data));
	
	do
	{
		stat=SPIdrv->GetStatus();
	}
	while (stat.busy);
	
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}

void LCD_wr_cmd(unsigned char cmd){
	
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);
	
	SPIdrv->Send(&cmd,sizeof(cmd));
	
	do
	{
		stat=SPIdrv->GetStatus();
	}
	while (stat.busy);
	
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}

void LCD_init(void){
	
	LCD_wr_cmd(0xAE);
	LCD_wr_cmd(0xA2);
	LCD_wr_cmd(0xA0);
	LCD_wr_cmd(0xC8);
	LCD_wr_cmd(0x22); 
	LCD_wr_cmd(0x2F);
	LCD_wr_cmd(0x40);
	LCD_wr_cmd(0xAF); 
	LCD_wr_cmd(0x81);
	LCD_wr_cmd(0x17);
	LCD_wr_cmd(0xA4);
	LCD_wr_cmd(0xA6);
	
}

void LCD_update(void)
{
 int i;
 LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
 LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
 LCD_wr_cmd(0xB0); // Página 0

 for(i=0;i<128;i++){
 LCD_wr_data(buffer[i]);
 }

 LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
 LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
 LCD_wr_cmd(0xB1); // Página 1

 for(i=128;i<256;i++){
 LCD_wr_data(buffer[i]);
 }

 LCD_wr_cmd(0x00);
 LCD_wr_cmd(0x10);
 LCD_wr_cmd(0xB2); //Página 2
 for(i=256;i<384;i++){
 LCD_wr_data(buffer[i]);
 }

 LCD_wr_cmd(0x00);
 LCD_wr_cmd(0x10);
 LCD_wr_cmd(0xB3); // Pagina 3


 for(i=384;i<512;i++){
 LCD_wr_data(buffer[i]);
 }
}

void symbolToLocalBuffer_L1(uint8_t symbol){
	uint8_t i, value1, value2;
	uint16_t offset=0;
	
	offset= 25*(symbol - ' ');
	
	for(i=0;i<12;i++){
		value1=Arial12x12[offset+i*2+1];
		value2=Arial12x12[offset+i*2+2];
		
		buffer[i+positionL1]=value1;
		buffer[i+128+positionL1]=value2;
	}
	positionL1=positionL1 + Arial12x12[offset];
}

void symbolToLocalBuffer_L2(uint8_t symbol){
	uint8_t i, value1, value2;
	uint16_t offset=0;
	
	offset= 25*(symbol - ' ');
	
	for(i=0;i<12;i++){
		value1=Arial12x12[offset+i*2+1];
		value2=Arial12x12[offset+i*2+2];
		
		buffer[i+256+positionL2]=value1;
		buffer[i+384+positionL2]=value2;
	}
	positionL2=positionL2 + Arial12x12[offset];
}

void symbolToLocalBuffer(uint8_t line,uint8_t symbol){
	
	if (line==1){
		symbolToLocalBuffer_L1(symbol);
	}
	if(line==2){
		symbolToLocalBuffer_L2(symbol);
	}
	
}

void clean(void){
	
	for(int f1=0;f1<512;f1++){
		buffer[f1]=0x00;
	}
}

void Clean_L1(void){
	for(int xd=0;xd<256;xd++){
		buffer[xd]=0x00;
	}
}

void Clean_L2(void){
	for(int xd=257;xd<512;xd++){
		buffer[xd]=0x00;
	}
}

void writeline1 (char *letras){
  uint8_t p;
	positionL1=0;
	Clean_L1();
  for (p=0; p<strlen(letras); p++) {
		if (positionL1<127){
   symbolToLocalBuffer_L1(letras[p]);  
    }
  }
}

void writeline2 (char *letras2)
{
  uint8_t y;
	positionL2=0;
	Clean_L2();
  for (y=0; y<strlen(letras2); y++) {
		if (positionL2<127){
   symbolToLocalBuffer_L2(letras2[y]);  
    }
  }
}



