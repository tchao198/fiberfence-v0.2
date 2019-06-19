/*
 * File      : onenet_sample.c
 * COPYRIGHT (C) 2006 - 2018, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-04-24     chenyong     first version
 */
#include <stdlib.h>
#include <arpa/inet.h>
#include <onenet.h>
#include "displayInfo.h"

#define DBG_ENABLE
#define DBG_COLOR
#define DBG_SECTION_NAME    "onenet.sample"
#if ONENET_DEBUG
#define DBG_LEVEL           DBG_LOG
#else
#define DBG_LEVEL           DBG_INFO
#endif /* ONENET_DEBUG */

#include <rtdbg.h>

#ifdef FINSH_USING_MSH
#include <finsh.h>

//查询命令
#define CMD_EXIT        0x100f   
#define CMD_CPU_USAGE   0x1001   //查询cpu使用率
#define CMD_OPTIC_POWER 0x1002   //查询光功率
#define CMD_ALARM_COUNT 0x1003   //查询报警计数
#define CMD_ALL         0x1000   //查询所有参数

//控制命令
#define CMD_DAC_GAIN_A        0x2001  //设置dac增益控制a
#define CMD_DAC_GAIN_B        0x2002  //设置dac增益控制b
#define CMD_ADC_SAMPLE_FRQ    0x2003  //设置adc采样频率
#define CMD_ALARM_THRESHOLD_A 0x2004  //设置a防区报警阈值
#define CMD_ALARM_THRESHOLD_B 0x2005  //设置b防区报警阈值
#define CMD_POWER_THRESHOLD   0x2006  //设置光功率阈值
#define CMD_ALARM_INTERVAL    0x2007  //设置报警时间间隔
#define CMD_ALARM_SENSITIVE   0x2008  //设置报警灵敏度
#define CMD_ENABLE_SENDDATA   0x2009  //设置是否发生原始数据
#define CMD_ENABLE_DEFENCE    0x200a  //设置布防撤防

//响应命令
#define CMD_RESPONSE_OK        0x0000    //命令响应成功（查询或者控制成功）
#define CMD_RESPONSE_UNKNOW    0x1111    //接收到未知命令
#define CMD_RESPONSE_MAGIC_ERR 0x2222    //帧校验错误

#define DISABLE 0;
#define ENABLE  1;

/* upload random value to temperature*/
static void onenet_upload_entry(void *parameter)
{
    int value = 0;

    while (1)
    {
//        value = rand() % 100;

//        if (onenet_mqtt_upload_digit("alarm_count_A", info.item7.param1) < 0)
//        {
//            LOG_E("upload has an error, stop uploading");
//            break;
//        }
//        else
//        {
//            LOG_D("buffer : {\"temperature\":%d}", value);
//        }

				//onenet_mqtt_upload_digit("Power_A", info.item5.param1);
				onenet_mqtt_upload_digit("Power_A", rand()%10+2500);
				rt_thread_delay(rt_tick_from_millisecond(1 * 500));
				//onenet_mqtt_upload_digit("Power_B", info.item6.param1);
				onenet_mqtt_upload_digit("Power_B", rand()%10+2300);
				rt_thread_delay(rt_tick_from_millisecond(1 * 500));
				onenet_mqtt_upload_digit("optic_power_th", info.item3.param1);
        rt_thread_delay(rt_tick_from_millisecond(1 * 500));
				onenet_mqtt_upload_digit("alarm_th_A", info.item1.param1);
        rt_thread_delay(rt_tick_from_millisecond(1 * 500));
				onenet_mqtt_upload_digit("alarm_th_B", info.item2.param1);
        rt_thread_delay(rt_tick_from_millisecond(1 * 500));
				onenet_mqtt_upload_digit("alarm_count_A", info.item7.param1);
        rt_thread_delay(rt_tick_from_millisecond(1 * 500));
				onenet_mqtt_upload_digit("alarm_count_B", info.item8.param1);
        rt_thread_delay(rt_tick_from_millisecond(1 * 500));
				onenet_mqtt_upload_digit("enable_fence", RT_TRUE);
        rt_thread_delay(rt_tick_from_millisecond(1 * 500));
    }
}

int onenet_upload_cycle(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("onenet_send",
                           onenet_upload_entry,
                           RT_NULL,
                           2 * 1024,
                           RT_THREAD_PRIORITY_MAX / 3 - 1,
                           5);
    if (tid)
    {
        rt_thread_startup(tid);
    }

    return 0;
}
//INIT_APP_EXPORT(onenet_upload_cycle);
MSH_CMD_EXPORT(onenet_upload_cycle, send data to OneNET cloud cycle);

int onenet_publish_digit(int argc, char **argv)
{
    if (argc != 3)
    {
        LOG_E("onenet_publish [datastream_id]  [value]  - mqtt pulish digit data to OneNET.");
        return -1;
    }

    if (onenet_mqtt_upload_digit(argv[1], atoi(argv[2])) < 0)
    {
        LOG_E("upload digit data has an error!\n");
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(onenet_publish_digit, onenet_mqtt_publish_digit, send digit data to onenet cloud);

int onenet_publish_string(int argc, char **argv)
{
    if (argc != 3)
    {
        LOG_E("onenet_publish [datastream_id]  [string]  - mqtt pulish string data to OneNET.");
        return -1;
    }

    if (onenet_mqtt_upload_string(argv[1], argv[2]) < 0)
    {
        LOG_E("upload string has an error!\n");
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(onenet_publish_string, onenet_mqtt_publish_string, send string data to onenet cloud);


extern int set_dac(rt_uint16_t value, rt_uint16_t chip_id);
extern rt_bool_t enable_defence;
extern uint16_t alarm_sensitive;
/* onenet mqtt command response callback function */
void onenet_cmd_rsp_cb(uint8_t *recv_data, size_t recv_size, uint8_t **resp_data, size_t *resp_size)
{
    char res_buf[] = { "cmd is received!\n" };

    LOG_D("recv data is %.*s\n", recv_size, recv_data);
		
		uint16_t cmd = *(uint16_t *)recv_data;
		uint16_t data = *(uint16_t *)(recv_data+2);
		cmd = htons(cmd);
		data = htons(data);
		rt_kprintf("CMD:0x%04x, Data:%d\n", cmd, data);
		switch(cmd){
			case CMD_DAC_GAIN_A:
				set_dac(data, 2);
				break;
			case CMD_DAC_GAIN_B:
				set_dac(data, 3);
				break;
			case CMD_ALARM_THRESHOLD_A:
				info.item1.param1 = data;
				break;
			case CMD_ALARM_THRESHOLD_B:
				info.item2.param1 = data;
				break;
			case CMD_POWER_THRESHOLD:
				info.item3.param1 = data;
				break;
			case CMD_ALARM_INTERVAL:
				info.item4.param1 = data;
				break;
			case CMD_ALARM_SENSITIVE:
				alarm_sensitive = data;
				break;
			case CMD_ENABLE_DEFENCE:
				enable_defence = data;
				break;
			case CMD_ADC_SAMPLE_FRQ:
				break;
			default:
				break;
		}

    /* user have to malloc memory for response data */
    *resp_data = (uint8_t *) ONENET_MALLOC(strlen(res_buf));

    strncpy((char *)*resp_data, res_buf, strlen(res_buf));

    *resp_size = strlen(res_buf);
}

/* set the onenet mqtt command response callback function */
int onenet_set_cmd_rsp()
{
    onenet_set_cmd_rsp_cb(onenet_cmd_rsp_cb);
    return 0;
}
//INIT_APP_EXPORT(onenet_set_cmd_rsp);
MSH_CMD_EXPORT(onenet_set_cmd_rsp, set cmd response function);

#endif /* FINSH_USING_MSH */
