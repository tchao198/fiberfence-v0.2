#ifndef __PIN_INIT_H__
#define __PIN_INIT_H__
#include <rtthread.h>
#include "drv_gpio.h"

//LED
#define SYSTEM_LED_PIN_NUM             GET_PIN(G,  8)  /* PG8 */
#define SYS_RUN_LED_PIN_NUM            GET_PIN(G,  10)  /* PG10 */
#define ALARM_A_LED_PIN_NUM            GET_PIN(G,  5)  /* PG5 */
#define ALARM_B_LED_PIN_NUM            GET_PIN(G,  3)  /* PG3 */
#define NETLINK_LED_PIN_NUM            GET_PIN(G,  7)  /* PG7 */
#define POWER_A_LED_PIN_NUM            GET_PIN(G,  6)  /* PG6 */
#define POWER_B_LED_PIN_NUM            GET_PIN(G,  4)  /* PG4 */

//KEY
#define KEY0_PIN_NUM            GET_PIN(D,  10)  /* PD10 */
#define KEY1_PIN_NUM            GET_PIN(D,  11)  /* PD11 */
#define KEY2_PIN_NUM            GET_PIN(D,  12)  /* PD12 */
#define KEY3_PIN_NUM            GET_PIN(D,  13)  /* PD13 */
#define KEY4_PIN_NUM            GET_PIN(D,  14)  /* PD14 */
#define KEY5_PIN_NUM            GET_PIN(D,  15)  /* PD15 */

int pin_init(void);
#endif
