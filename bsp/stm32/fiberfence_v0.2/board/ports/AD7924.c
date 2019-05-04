#include "AD7924.h"
#include "myspi.h"
#include "delay.h"
#include "stm32f4xx_hal_gpio.h"

void AD7924_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOG_CLK_ENABLE();           //使能GPIOG时钟
    
    //GPIOG15
    GPIO_Initure.Pin=GPIO_PIN_15;            //GPIOF10
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推免输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;     //快速         
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);     //初始化GPIO
	
	AD7924_CS = 1;                        //ADC片选不选中
	SPI6_Init();
	SPI6_SetSpeed(SPI_BAUDRATEPRESCALER_2);
}

u16 ADC_Conversion(u16 cmd)
{
	u16 result;
	AD7924_CS = 0;
	result = SPI6_ReadWriteWord(cmd);
	AD7924_CS = 1;
	return( result&0xFFFF );
}
