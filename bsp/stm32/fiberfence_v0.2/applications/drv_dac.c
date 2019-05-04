#include "drv_dac.h"
#include "rtthread.h"
#include <stdlib.h>
//////////////////////////////////////////////////////////////////////////////////	 
//内部DAC驱动				  
////////////////////////////////////////////////////////////////////////////////// 	

DAC_HandleTypeDef DAC_Handler;//DAC句柄

//初始化DAC
int DAC_Init(void)
{
    DAC_ChannelConfTypeDef DAC_Config;
    
    DAC_Handler.Instance=DAC;
    HAL_DAC_Init(&DAC_Handler);                 //初始化DAC
    
    DAC_Config.DAC_Trigger=DAC_TRIGGER_NONE;             //不使用触发功能
    DAC_Config.DAC_OutputBuffer=DAC_OUTPUTBUFFER_DISABLE;//DAC1输出缓冲关闭
    HAL_DAC_ConfigChannel(&DAC_Handler,&DAC_Config,DAC_CHANNEL_1);//DAC通道1配置
		HAL_DAC_ConfigChannel(&DAC_Handler,&DAC_Config,DAC_CHANNEL_2);//DAC通道1配置
    
    HAL_DAC_Start(&DAC_Handler,DAC_CHANNEL_1);  //开启DAC通道1
		HAL_DAC_Start(&DAC_Handler,DAC_CHANNEL_2);  //开启DAC通道1
	
		return 0;
}
INIT_DEVICE_EXPORT(DAC_Init);

//DAC底层驱动，时钟配置，引脚 配置
//此函数会被HAL_DAC_Init()调用
//hdac:DAC句柄
void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac)
{      
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_DAC_CLK_ENABLE();             //使能DAC时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();			//开启GPIOA时钟
	
    GPIO_Initure.Pin=GPIO_PIN_4 | GPIO_PIN_5;            //PA4
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     //模拟
    GPIO_Initure.Pull=GPIO_NOPULL;          //不带上下拉
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
}

//设置通道1输出电压
//vol:0~3300,代表0~3.3V
int DAC1_Set_Vol(int argc, char **argv)
{
	if(argc==2){
		double temp=atoi(argv[1]);
		rt_kprintf("v:%d\n",atoi(argv[1]));
		temp/=1000;
		temp=temp*4096/3.3;
		HAL_DAC_SetValue(&DAC_Handler,DAC_CHANNEL_1,DAC_ALIGN_12B_R,temp);//12位右对齐数据格式设置DAC值
	}
	return 0;
}

//设置通道2输出电压
//vol:0~3300,代表0~3.3V
int DAC2_Set_Vol(int argc, char **argv)
{
	if(argc==2){
		double temp=atoi(argv[1]);
		rt_kprintf("v:%d\n",atoi(argv[1]));
		temp/=1000;
		temp=temp*4096/3.3;
		HAL_DAC_SetValue(&DAC_Handler,DAC_CHANNEL_2,DAC_ALIGN_12B_R,temp);//12位右对齐数据格式设置DAC值
	}
	return 0;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT_ALIAS(DAC1_Set_Vol, __cmd_DAC1_Set_Val,set dac1 value);
FINSH_FUNCTION_EXPORT_ALIAS(DAC2_Set_Vol, __cmd_DAC2_Set_Val,set dac2 value);
#endif
