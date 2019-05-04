#include "pin_init.h"

int pin_init(void)
{
	//LED
	rt_pin_mode(SYSTEM_LED_PIN_NUM, PIN_MODE_OUTPUT);
	rt_pin_mode(SYS_RUN_LED_PIN_NUM, PIN_MODE_OUTPUT);
	rt_pin_mode(ALARM_A_LED_PIN_NUM, PIN_MODE_OUTPUT);
	rt_pin_mode(ALARM_B_LED_PIN_NUM, PIN_MODE_OUTPUT);
	rt_pin_mode(NETLINK_LED_PIN_NUM, PIN_MODE_OUTPUT);
	rt_pin_mode(POWER_A_LED_PIN_NUM, PIN_MODE_OUTPUT);
	rt_pin_mode(POWER_B_LED_PIN_NUM, PIN_MODE_OUTPUT);
	
	rt_pin_write(SYSTEM_LED_PIN_NUM, PIN_HIGH);
	rt_pin_write(SYS_RUN_LED_PIN_NUM, PIN_HIGH);
	rt_pin_write(ALARM_A_LED_PIN_NUM, PIN_HIGH);
	rt_pin_write(ALARM_B_LED_PIN_NUM, PIN_HIGH);
	rt_pin_write(NETLINK_LED_PIN_NUM, PIN_HIGH);
	rt_pin_write(POWER_A_LED_PIN_NUM, PIN_HIGH);
	rt_pin_write(POWER_B_LED_PIN_NUM, PIN_HIGH);

	//KEY
	rt_pin_mode(KEY0_PIN_NUM, PIN_MODE_INPUT);
	rt_pin_mode(KEY1_PIN_NUM, PIN_MODE_INPUT);
	rt_pin_mode(KEY2_PIN_NUM, PIN_MODE_INPUT);
	rt_pin_mode(KEY3_PIN_NUM, PIN_MODE_INPUT);
	rt_pin_mode(KEY4_PIN_NUM, PIN_MODE_INPUT);
	rt_pin_mode(KEY5_PIN_NUM, PIN_MODE_INPUT);
	
	return 0;
}
INIT_DEVICE_EXPORT(pin_init);
