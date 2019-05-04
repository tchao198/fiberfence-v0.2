#include "lcd.h"

uint16_t DB[] = {GPIO_PIN_5,GPIO_PIN_4,GPIO_PIN_3,GPIO_PIN_7,GPIO_PIN_6,GPIO_PIN_5,GPIO_PIN_4,GPIO_PIN_1};

void LcdCommandWrite(uint8_t value) 
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_8, GPIO_PIN_RESET);
	int i = 0;
	for (i=0; i <= 7; i++) 
	{
		switch(i){
			case 0:
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, (GPIO_PinState) (value & 0x01));
				break;
			case 1:
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, (GPIO_PinState) (value & 0x01));
				break;
			case 2:
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, (GPIO_PinState) (value & 0x01));
				break;
			case 3:
				HAL_GPIO_WritePin(GPIOI, GPIO_PIN_7, (GPIO_PinState) (value & 0x01));
				break;
			case 4:
				HAL_GPIO_WritePin(GPIOI, GPIO_PIN_6, (GPIO_PinState) (value & 0x01));
				break;
			case 5:
				HAL_GPIO_WritePin(GPIOI, GPIO_PIN_5, (GPIO_PinState) (value & 0x01));
				break;
			case 6:
				HAL_GPIO_WritePin(GPIOI, GPIO_PIN_4, (GPIO_PinState) (value & 0x01));
				break;
			case 7:
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, (GPIO_PinState) (value & 0x01));
				break;
			default:
				break;
		}
		 value >>= 1;
	}
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);
	delay_us(1);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);
	delay_us(1);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);
	delay_us(1);
}

void LcdDataWrite(uint8_t value) 
{
	int i = 0;
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_8, GPIO_PIN_RESET);
	for (i=0; i <= 7; i++) 
	{
		switch(i){
			case 0:
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, (GPIO_PinState) (value & 0x01));
				break;
			case 1:
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, (GPIO_PinState) (value & 0x01));
				break;
			case 2:
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, (GPIO_PinState) (value & 0x01));
				break;
			case 3:
				HAL_GPIO_WritePin(GPIOI, GPIO_PIN_7, (GPIO_PinState) (value & 0x01));
				break;
			case 4:
				HAL_GPIO_WritePin(GPIOI, GPIO_PIN_6, (GPIO_PinState) (value & 0x01));
				break;
			case 5:
				HAL_GPIO_WritePin(GPIOI, GPIO_PIN_5, (GPIO_PinState) (value & 0x01));
				break;
			case 6:
				HAL_GPIO_WritePin(GPIOI, GPIO_PIN_4, (GPIO_PinState) (value & 0x01));
				break;
			case 7:
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, (GPIO_PinState) (value & 0x01));
				break;
			default:
				break;
		}
		 value >>= 1;
	}
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);
	delay_us(1);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);
	delay_us(1);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);
	delay_us(1);
}

void lcdStrWrite(char *s)
{
	int i=0;
	while(s[i]!='\0')
	{
		LcdDataWrite(s[i]);
		delay_ms(1);
		i++;
	}
}

void lcdDecimalWrite(int value)
{
	int i=3;
	char s[5] = {'0','0','0','0','\0'};
	s[4] = '\0';
	
	while(value!=0)
	{
		s[i--] = value%10 + 48;
		value /= 10;
	}
	lcdStrWrite(s);
}

void lcd1602_init(void)
{
	 GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOI_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7 
                          |GPIO_PIN_8, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5 
                          |GPIO_PIN_6, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

  /*Configure GPIO pins */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7 
                          |GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
	
	/*Configure GPIO pins */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5 
                          |GPIO_PIN_6;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	/*Configure GPIO pins */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	LcdCommandWrite(0x38);  // ???8-bit??,2???,5x7????                     
	delay_ms(10);                      
	LcdCommandWrite(0x38);  // ???8-bit??,2???,5x7????                        
	delay_ms(100);                      
	LcdCommandWrite(0x38);  // ???8-bit??,2???,5x7????                        
	delay_ms(100);                      
	LcdCommandWrite(0x06);  // ??????
													 // ????,??????
	delay_ms(100);                      
	LcdCommandWrite(0x0C);  // ????
													 // ?????,????,???
	delay_ms(100);                      
	LcdCommandWrite(0x01);  // ????,??????  
	delay_ms(100);                      
	LcdCommandWrite(0x80);  // ????
	delay_ms(100); 
	
	LcdCommandWrite(0x01);  // ????,??????  
  delay_ms(100); 
  LcdCommandWrite(0x80+5); 
  delay_ms(100); 
	lcdStrWrite("Welcome");
	delay_ms(100); 
	LcdCommandWrite(0xC0+3);
	delay_ms(100);    
	lcdStrWrite("Fiber fence");
	delay_ms(100);  
}
