#ifndef __DRV_KEY_H
#define __DRV_KEY_H
#include "sys.h"
#include "pin_init.h"
#include <rtthread.h>


//下面的方式是通过直接操作HAL库函数方式读取IO
#define KEY0        rt_pin_read(KEY0_PIN_NUM)  //KEY0按键GPIOD10
#define KEY1        rt_pin_read(KEY1_PIN_NUM)  //KEY1按键GPIOD11
#define KEY2        rt_pin_read(KEY2_PIN_NUM)  //KEY2按键GPIOD12
#define KEY3        rt_pin_read(KEY3_PIN_NUM)  //KEY2按键GPIOD13
#define KEY4        rt_pin_read(KEY4_PIN_NUM)  //KEY2按键GPIOD14
#define KEY5        rt_pin_read(KEY5_PIN_NUM)  //KEY2按键GPIOD15

#define KEY0_PRES 	1
#define KEY1_PRES		2
#define KEY2_PRES		3
#define KEY3_PRES   4
#define KEY4_PRES   5
#define KEY5_PRES   6

void KEY_Init(void);
rt_uint8_t KEY_Scan(rt_uint8_t mode);
#endif
