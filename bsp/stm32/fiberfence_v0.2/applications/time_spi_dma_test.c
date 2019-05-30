#include "stm32f4xx_hal.h"

SPI_HandleTypeDef hspi3;

TIM_HandleTypeDef htim3;
DMA_HandleTypeDef hdma_tim3_ch2;
DMA_HandleTypeDef hdma_tim3_ch3;

static void _Error_Handler(char *line, int count)
{
	while(1);
}


/* TIM3 init function */
static void MX_TIM3_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 4500-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 800-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_OC_Init(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 50-1;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfigOC.OCMode = TIM_OCMODE_INACTIVE;
  sConfigOC.Pulse = 100-1;
  if (HAL_TIM_OC_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

	sConfigOC.OCMode = TIM_OCMODE_INACTIVE;
  sConfigOC.Pulse = 600-1;
  if (HAL_TIM_OC_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }


	extern void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
  HAL_TIM_MspPostInit(&htim3);

	HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM3_IRQn);
	
	/* Enable the TIM Capture/Compare 2 DMA request */
	__HAL_TIM_ENABLE_DMA(&htim3, TIM_DMA_CC2);
	/* Enable the TIM Capture/Compare 3 DMA request */
	__HAL_TIM_ENABLE_DMA(&htim3, TIM_DMA_CC3);
	
	/* Enable the TIM Capture/Compare 2 interrupt */
  __HAL_TIM_ENABLE_IT(&htim3, TIM_IT_CC2);
	/* Enable the TIM Capture/Compare 3 interrupt */
  __HAL_TIM_ENABLE_IT(&htim3, TIM_IT_CC3);
	
	/* Enable the Output compare channel */
	TIM_CCxChannelCmd(htim3.Instance, TIM_CHANNEL_2, TIM_CCx_ENABLE);
	TIM_CCxChannelCmd(htim3.Instance, TIM_CHANNEL_3, TIM_CCx_ENABLE);
	
	/* Enable the main output */
  //__HAL_TIM_MOE_ENABLE(&htim3);
	
	/* Enable the Peripheral */
  __HAL_TIM_ENABLE(&htim3);
	
}

/* SPI3 init function */
static void MX_SPI3_Init(void)
{

  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

	__HAL_LINKDMA(&hspi3,hdmarx,hdma_tim3_ch3);
	__HAL_LINKDMA(&hspi3,hdmatx,hdma_tim3_ch2);
	
//	HAL_NVIC_SetPriority(SPI3_IRQn, 0, 0);
//  HAL_NVIC_EnableIRQ(SPI3_IRQn);
}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

	/* TIM3 DMA Init */
    /* TIM3_CH2 Init */
    hdma_tim3_ch2.Instance = DMA1_Stream5;
    hdma_tim3_ch2.Init.Channel = DMA_CHANNEL_5;
    hdma_tim3_ch2.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_tim3_ch2.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim3_ch2.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim3_ch2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_tim3_ch2.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_tim3_ch2.Init.Mode = DMA_CIRCULAR;
    hdma_tim3_ch2.Init.Priority = DMA_PRIORITY_LOW;
    hdma_tim3_ch2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_tim3_ch2) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }

    /* TIM3_CH3 Init */
    hdma_tim3_ch3.Instance = DMA1_Stream7;
    hdma_tim3_ch3.Init.Channel = DMA_CHANNEL_5;
    hdma_tim3_ch3.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_tim3_ch3.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim3_ch3.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim3_ch3.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_tim3_ch3.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_tim3_ch3.Init.Mode = DMA_CIRCULAR;
    hdma_tim3_ch3.Init.Priority = DMA_PRIORITY_LOW;
    hdma_tim3_ch3.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_tim3_ch3) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }
	
  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA1_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream7_IRQn);
}

static int init_dma_timer()
{
	MX_DMA_Init();
	MX_SPI3_Init();
	MX_TIM3_Init();
	return 0;
}
//INIT_APP_EXPORT(init_dma_timer);

static uint8_t pTxData[200], pRxData[200];
static void timer_spi_dma_test()
{
	HAL_SPI_TransmitReceive_DMA(&hspi3, pTxData, pRxData, 50); //size <= sizeof(pRxData)/Data_With, Data_With=8bit or 16bit, 否则导致内存越界
	rt_kprintf("test\n");
}

#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT_ALIAS(timer_spi_dma_test, __cmd_test_dma,);
#endif

void DMA1_Stream5_IRQHandler(void)
{
	rt_interrupt_enter();
  HAL_DMA_IRQHandler(&hdma_tim3_ch2);
	rt_interrupt_leave();
}

void DMA1_Stream7_IRQHandler(void)
{
	rt_interrupt_enter();
  HAL_DMA_IRQHandler(&hdma_tim3_ch3);
	rt_interrupt_leave();
}

void telnet_printf(char *str);
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance==SPI3)
		telnet_printf("HAL_SPI_RxCpltCallback\n");
}

void HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance==SPI3)
		telnet_printf("HAL_SPI_RxHalfCpltCallback\n");
}

void SPI3_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_SPI_IRQHandler(&hspi3);

    /* leave interrupt */
    rt_interrupt_leave();
}

void TIM3_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_TIM_IRQHandler(&htim3);

    /* leave interrupt */
    rt_interrupt_leave();
}

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
		;//telnet_printf("HAL_TIM_IC_CaptureCallback3\n");
	else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		;//telnet_printf("HAL_TIM_IC_CaptureCallback2\n");
}

/******************************************/
//实现telnet 在中断中使用rt_kprintf
/******************************************/
rt_thread_t telnet_printf_id;
rt_mailbox_t telnet_printf_mb;
static char print_str[500];

void telnet_printf_sever_entry(void *parameter)
{
	char *str;
	telnet_printf_mb = rt_mb_create("telnet_mb_print", 5, RT_IPC_FLAG_FIFO);
	
	while(1){
		if (rt_mb_recv(telnet_printf_mb, (rt_ubase_t*)&str, RT_WAITING_FOREVER)== RT_EOK)
		{
			rt_kprintf("%s", print_str);
		}
	}
}

void telnet_printf(char *str)
{
	int len = strlen(str);
	rt_memcpy(print_str, str, len);
	print_str[len]='\0';
	rt_mb_send(telnet_printf_mb, (rt_uint32_t)&print_str);
}

int telnet_printf_sever()
{
		telnet_printf_id = rt_thread_create("telnet_printf",
        telnet_printf_sever_entry, RT_NULL,
        2048, RT_THREAD_PRIORITY_MAX/3-1, 20);

    if (telnet_printf_id != RT_NULL)
        rt_thread_startup(telnet_printf_id);
		
		return 0;
}
INIT_APP_EXPORT(telnet_printf_sever);

/********************************************/

