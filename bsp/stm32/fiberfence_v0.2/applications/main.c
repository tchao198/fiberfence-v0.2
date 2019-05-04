/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-12-18     zylx         first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <dfs_posix.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <string.h>
#include <stdlib.h>

#include <fal.h>

#include "AD7924.h"
#include "displayInfo.h"
#include "pin_init.h"
#include "drv_key.h"
#include "key_app.h"
#include "lcd.h"

#ifdef RT_USING_DFS
/* dfs init */

/* dfs filesystem:ELM filesystem init */
#include <dfs_elm.h>
/* dfs Filesystem APIs */
#include <dfs_fs.h>

#endif

#define LOG_TAG              "MAIN"
#define LOG_LVL              LOG_LVL_DBG
#include <ulog.h>

#define CMD_HEARTBEAT   0x3001    //������
#define CMD_ALARM       0x3002  	//������
#define CMD_POWER_LOWER 0x3003    //�⹦�ʹ���

#define DEVICE_NUMBER   1;

typedef struct {
    uint16_t magic;
    uint16_t cmd;
    uint16_t device_id;
    uint16_t zone_id;
		uint16_t power_flag1;
		uint16_t power_flag2;
} Cmd_Data_t;

typedef struct {
	u16 ac1[1000];
	u16 dc1[100];
	u16 ac2[1000];
	u16 dc2[100];
} ADC_data_t;

ADC_data_t adc_data_a, adc_data_b;

rt_rbb_t rbb, rbb_cmd;

rt_thread_t alarmid_A, alarmid_B, udp_send_id, udp_cmd_id, power_id, key_id, sys_led_id;

volatile u16 id = 0;

volatile rt_bool_t data_flag = RT_FALSE;

struct rt_mailbox mb_a, mb_b;

static char mb_a_pool[4], mb_b_pool[4];

static rt_mutex_t dynamic_mutex = RT_NULL;

extern void set_dns(char* dns_server);

static int hwtimer_init(void );

void rt_alarm_process_A_thread_entry(void* parameter);
void rt_alarm_process_B_thread_entry(void* parameter);
void udp_send_data_entry(void* parameter);
void udp_send_cmd_entry(void* parameter);
void rt_key_thread_entry(void* parameter);
void rt_optic_power_thread_entry(void* parameter);
void rt_system_led_thread_entry(void* parameter);

void send_cmd(int cmd, int zone_id);
void send_data(rt_uint16_t *value, rt_uint16_t len, rt_uint16_t chan);
void telnet_server(void);

int main(void)
{
	fal_init();
	set_dns("114.114.114.114");
	
	#ifdef RT_USING_DFS   
	//dfs_init();
        
#ifdef RT_USING_DFS_ELMFAT
	//elm_init();
    
		//dfs_mkfs("elm","W25Q64");
    /* mount sd card fat partition 0 as root directory */
    if (dfs_mount("W25Q64", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("spi flash mount to / succesfull!\n");
    }
    else
    {
        rt_kprintf("spi flash mount to / failed!\n");
    }
    
//    /* mount sd card fat partition 0 as root directory */
//    if (dfs_mount("sd0", "/sdcard", "elm", 0, 0) == 0)
//    {
//        rt_kprintf("sd card mount to /sdcard succesfull!\n");        
//    }
//    else
//    {
//        rt_kprintf("sd card mount to /sdcard failed!\n");
//    }
#endif /* RT_USING_DFS_ELMFAT */
        
#endif /* DFS */
		
		dynamic_mutex = rt_mutex_create("dmutex", RT_IPC_FLAG_FIFO);
    if (dynamic_mutex == RT_NULL)
    {
        rt_kprintf("create dynamic mutex failed.\n");
    }
		
		rbb = rt_rbb_create(10020, 5);
		rbb_cmd = rt_rbb_create(2048, 10);
		
		rt_mb_init(&mb_a,
								"mbt_a", /* name */
								&mb_a_pool[0], /* mail mb_pool */
								sizeof(mb_a_pool)/4, /*size*/
								RT_IPC_FLAG_FIFO);
								
		rt_mb_init(&mb_b,
								"mbt_b", /* name */
								&mb_b_pool[0], /* mail mb_pool */
								sizeof(mb_b_pool)/4, /*size*/
								RT_IPC_FLAG_FIFO);
								
		alarmid_A = rt_thread_create("alarm_process_A",
        rt_alarm_process_A_thread_entry, RT_NULL,
        2048, RT_THREAD_PRIORITY_MAX/3-1, 20);

    if (alarmid_A != RT_NULL)
        rt_thread_startup(alarmid_A);
		
		alarmid_B = rt_thread_create("alarm_process_B",
        rt_alarm_process_B_thread_entry, RT_NULL,
        2048, RT_THREAD_PRIORITY_MAX/3-1, 20);

    if (alarmid_B != RT_NULL)
        rt_thread_startup(alarmid_B);
		
		udp_send_id = rt_thread_create("udp_send_data",
        udp_send_data_entry, RT_NULL,
        4096, RT_THREAD_PRIORITY_MAX/3-1, 20);

    if (udp_send_id != RT_NULL)
        rt_thread_startup(udp_send_id);
		
		udp_cmd_id = rt_thread_create("udp_send_cmd",
        udp_send_cmd_entry, RT_NULL,
        2048, RT_THREAD_PRIORITY_MAX/3, 20);

    if (udp_cmd_id != RT_NULL)
        rt_thread_startup(udp_cmd_id);
		
		key_id = rt_thread_create("key_thread",
        rt_key_thread_entry, RT_NULL,
        2048, RT_THREAD_PRIORITY_MAX/3-3, 20);

    if (key_id != RT_NULL)
        rt_thread_startup(key_id);
		
		power_id = rt_thread_create("optic_power_thread",
        rt_optic_power_thread_entry, RT_NULL,
        2048, RT_THREAD_PRIORITY_MAX/3, 20);

    if (power_id != RT_NULL)
        rt_thread_startup(power_id);
		
		sys_led_id = rt_thread_create("sys_led_thread",
        rt_system_led_thread_entry, RT_NULL,
        2048, RT_THREAD_PRIORITY_MAX/3, 20);

    if (sys_led_id != RT_NULL)
        rt_thread_startup(sys_led_id);
		
		lcd1602_init();
		AD7924_Init();
		hwtimer_init();
		telnet_server();
		
		
		//rt_kprintf("The current version of APP firmware is 1.0.0\n");
		
		extern int phy_register_read(int reg);
		int phy_status;
		while(1){
			//adc_ReadOneSample(0x8310);
			phy_status=phy_register_read(1);
			if((phy_status&0x0004) == 0){
				rt_pin_write(NETLINK_LED_PIN_NUM, PIN_HIGH);
				//rt_kprintf("link down\n");
			}
			else{
				rt_pin_write(NETLINK_LED_PIN_NUM, PIN_LOW);
				//rt_kprintf("link up\n");
			}
			send_cmd(CMD_HEARTBEAT, 0);
			rt_thread_mdelay(1000);
		}
    
    return RT_EOK;
}

void send_cmd(int cmd, int zone_id){
		Cmd_Data_t *cmd_data;
		rt_rbb_blk_t blk;
		blk = rt_rbb_blk_alloc(rbb_cmd, sizeof(Cmd_Data_t));
		if(blk!=0 && blk->size==sizeof(Cmd_Data_t)){
			cmd_data=(Cmd_Data_t *)blk->buf;
			cmd_data->cmd=cmd;
			cmd_data->magic=0x1234;
			cmd_data->device_id= DEVICE_NUMBER;
			cmd_data->zone_id = zone_id;
			rt_rbb_blk_put(blk);
		}
		else{
			rt_kprintf("rt_rbb_blk_alloc err\n");
		}
}

void send_data(rt_uint16_t *value, rt_uint16_t len, rt_uint16_t chan){
		rt_rbb_blk_t blk;
		blk = rt_rbb_blk_alloc(rbb, 2002);
			if(blk!=0 && blk->size==2002){
				*(rt_uint16_t *)blk->buf = chan;
				rt_memcpy((char *)(blk->buf+2), (char *)value, len);
				
				rt_rbb_blk_put(blk);
			}
			else{
				rt_kprintf("rt_rbb_blk_alloc err\n");
			}
}

#define HWTIMER_DEV_NAME   "timer14"


static rt_err_t timeout_cb(rt_device_t dev, rt_size_t size)
{
	
		uint16_t result;
		rt_err_t error;
	
    result = ADC_Conversion(CHAN1_CONVERSION);
		adc_data_a.dc1[id%100] = result&0x0fff;
	
		result = ADC_Conversion(CHAN2_CONVERSION);
		adc_data_a.dc2[id%100] = result&0x0fff;
	
		result = ADC_Conversion(CHAN3_CONVERSION);
		if(data_flag==0)
			adc_data_a.ac2[id] = result&0x0fff;
		else
			adc_data_b.ac2[id] = result&0x0fff;
		
		result = ADC_Conversion(CHAN4_CONVERSION);
		if(data_flag==0)
			adc_data_a.ac1[id] = result&0x0fff;
		else
			adc_data_b.ac1[id] = result&0x0fff;
		
		id++;
		if(id >= 1000){
			id = 0;
			
			if(data_flag==0)
			{
				error = rt_mb_send(&mb_a, (rt_uint32_t)&adc_data_a.ac1);
				error = rt_mb_send(&mb_b, (rt_uint32_t)&adc_data_a.ac2);
			}
			else
			{
				error = rt_mb_send(&mb_a, (rt_uint32_t)&adc_data_b.ac1);
				error = rt_mb_send(&mb_b, (rt_uint32_t)&adc_data_b.ac2);
			}
			
			if(error != RT_EOK)                      //��������Ƿ�����
				rt_kprintf("mailbox full error:%d\n", (int)error);

			data_flag = ~data_flag; 
		}

		
    return 0;
}

static int hwtimer_init(void)
{
    rt_err_t ret = RT_EOK;
    rt_hwtimerval_t timeout_s;
    rt_device_t hw_dev = RT_NULL;
    rt_hwtimer_mode_t mode;
    rt_uint32_t freq = 1000000;

    hw_dev = rt_device_find(HWTIMER_DEV_NAME);
    if (hw_dev == RT_NULL)
    {
        rt_kprintf("hwtimer sample run failed! can't find %s device!\n", HWTIMER_DEV_NAME);
        return RT_ERROR;
    }

    ret = rt_device_open(hw_dev, RT_DEVICE_OFLAG_RDWR);
    if (ret != RT_EOK)
    {
        rt_kprintf("open %s device failed!\n", HWTIMER_DEV_NAME);
        return ret;
    }

    rt_device_set_rx_indicate(hw_dev, timeout_cb);

    ret = rt_device_control(hw_dev, HWTIMER_CTRL_FREQ_SET, &freq);
    if (ret != RT_EOK)
    {
        rt_kprintf("set frequency failed! ret is :%d\n", ret);
        return ret;
    }

    mode = HWTIMER_MODE_PERIOD;
    ret = rt_device_control(hw_dev, HWTIMER_CTRL_MODE_SET, &mode);
    if (ret != RT_EOK)
    {
        rt_kprintf("set mode failed! ret is :%d\n", ret);
        return ret;
    }

    timeout_s.sec = 0;
    timeout_s.usec = 25;

    if (rt_device_write(hw_dev, 0, &timeout_s, sizeof(timeout_s)) != sizeof(timeout_s))
    {
        rt_kprintf("set timeout value failed\n");
        return RT_ERROR;
    }

    return ret;
}

void udp_send_data_entry(void* parameter)
{
		rt_rbb_blk_t blk;
		
    int sock, port=8089;
    struct sockaddr_in server_addr;
		char *tmp;
		struct hostent *host_pc;
		tmp=rt_malloc(2004);

		extern struct hostent *gethostbyname(const char *name);
    host_pc = (struct hostent *) gethostbyname("192.168.1.102");

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1 )
    {
				rt_free(tmp);
        rt_kprintf("Socket error\n");
        return;
    }
		
//		struct timeval tv_out;
//		tv_out.tv_sec = 0;
//		tv_out.tv_usec = 50; 
//		setsockopt(sock,SOL_SOCKET,SO_SNDTIMEO,(char*)&tv_out,sizeof(tv_out));  

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *) host_pc->h_addr);
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));
		
		while(1){
			rt_mutex_take(dynamic_mutex, RT_WAITING_FOREVER);
			blk = rt_rbb_blk_get(rbb);
			if(blk!=0 && blk->size==2002){
				*(rt_uint16_t *)tmp = *(rt_uint16_t *)blk->buf;
				rt_memcpy(tmp+2, blk->buf+2, 1000);
				sendto(sock, tmp, 1002, 0,
					 (struct sockaddr *) &server_addr, sizeof(struct sockaddr));
				
				*(rt_uint16_t *)(tmp+1002) = *(rt_uint16_t *)blk->buf;
				rt_memcpy(tmp+1002+2, blk->buf+1002, 1000);
				sendto(sock, tmp, 1002, 0,
					 (struct sockaddr *) &server_addr, sizeof(struct sockaddr));
				
				//rt_kprintf("send data.\n");
				rt_rbb_blk_free(rbb, blk);
			}
//			else if(blk!=0 && blk->size==sizeof(Cmd_Data_t)){
//				sendto(sock, blk->buf, sizeof(Cmd_Data_t), 0,
//					 (struct sockaddr *) &server_addr, sizeof(struct sockaddr));
//				
//				rt_kprintf("send cmd_data.\n");
//				rt_rbb_blk_free(rbb, blk);
//			}
			else{
				//rt_kprintf("rt_rbb_blk_get failed.\n");
				rt_thread_mdelay(1);
			}
			rt_mutex_release(dynamic_mutex);
		}
		
    close(sock);
		rt_free(tmp);
}

void udp_send_cmd_entry(void* parameter)
{
		rt_rbb_blk_t blk;
		
    int sock, port=5000;
    struct sockaddr_in server_addr;
		char *tmp;
		struct hostent *host_pc;
		tmp=rt_malloc(2004);

    host_pc = (struct hostent *) gethostbyname("192.168.1.200");

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1 )
    {
				rt_free(tmp);
        rt_kprintf("Socket error\n");
        return;
    }
		
//		struct timeval tv_out;
//		tv_out.tv_sec = 0;
//		tv_out.tv_usec = 50; 
//		setsockopt(sock,SOL_SOCKET,SO_SNDTIMEO,(char*)&tv_out,sizeof(tv_out));  

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *) host_pc->h_addr);
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));
		
		while(1){
			blk = rt_rbb_blk_get(rbb_cmd);
			if(blk!=0 && blk->size==sizeof(Cmd_Data_t)){
				sendto(sock, blk->buf, sizeof(Cmd_Data_t), 0,
					 (struct sockaddr *) &server_addr, sizeof(struct sockaddr));
				
				//rt_kprintf("send cmd_data.\n");
				rt_rbb_blk_free(rbb, blk);
			}
			else{
				//rt_kprintf("rt_rbb_blk_get failed.\n");
				rt_thread_mdelay(1);
			}
		}
		
    close(sock);
		rt_free(tmp);
}

/*
*******************����A�����źŴ���**********************
*/
void rt_alarm_process_A_thread_entry(void* parameter)
{
	int i, count=0;
	rt_uint16_t *value;
	float tmp=0, mean_value=2048;

	while(1)
	{
		if (rt_mb_recv(&mb_a, (rt_ubase_t*)&value, RT_WAITING_FOREVER)== RT_EOK)
		{
			
			//�ȴ�һ������ʱ����
			if(count<20*5){
				count++;
				goto send_data;           //����ʱ����δ����ֱ�ӽ����ݷ��͵�PC
			}
			
			//��ʼ�źŴ���
			for(i=0; i<1000; i++)
			{
				if(value[i]>info.item1.param1 || value[i]< (mean_value-(info.item1.param1-mean_value)))
				{
					LOG_E("Alarmed in zone A. value:%d mean:%f th:%d", value[i], mean_value, info.item1.param1);
					count=0;        //����ʱ�������㣬��ʼ����
					send_cmd(CMD_ALARM, 0);
  				break;
				}
				tmp+=value[i];
			}
			mean_value=tmp/i;
			tmp=0;
		}
		
		//�ﵽ����ʱ�������رձ���
		if(count>=20*5)
		{
			;
		}
		
		//�����ݴ�����͵�PC����
		send_data:
			rt_mutex_take(dynamic_mutex, RT_WAITING_FOREVER);
			send_data(value, 2000, 11);
			rt_mutex_release(dynamic_mutex);
	}
}

/*
*******************����B�����źŴ���**********************
*/
void rt_alarm_process_B_thread_entry(void* parameter)
{
	int i, count=0;
	rt_uint16_t *value;
	float tmp=0, mean_value=2048;

	while(1){
		if (rt_mb_recv(&mb_b, (rt_ubase_t*)&value, RT_WAITING_FOREVER)== RT_EOK)
		{
			
			//�ȴ�һ������ʱ����
			if(count<20*5){
				count++;
				goto send_data;           //����ʱ����δ����ֱ�ӽ����ݷ��͵�PC
			}
			
			//��ʼ�źŴ���
			for(i=0; i<1000; i++)
			{
				if( value[i]>info.item1.param1 || value[i]< (mean_value-(info.item1.param1-mean_value)) )
				{
					LOG_E("Alarmed in zone B. value:%d mean:%f th:%d", value[i], mean_value, info.item1.param1);
					count=0;          //����ʱ�������㣬��ʼ����
					send_cmd(CMD_ALARM, 1);
  				break;
				}
				tmp+=value[i];
			}
			mean_value=tmp/i;
			tmp=0;
		}
		
		
		//�ﵽ����ʱ�������رձ���
		if(count>=20*5)
		{
			;
		}
		
		
		//�����ݴ�����͵�PC����
		send_data:
			rt_mutex_take(dynamic_mutex, RT_WAITING_FOREVER);
			send_data(value, 2000, 22);
			rt_mutex_release(dynamic_mutex);
	}
	
}

/*
*******************LEDָʾ���ź�**********************
*/
void rt_system_led_thread_entry(void* parameter)
{ 
	while(1)
	{
		rt_pin_write(SYSTEM_LED_PIN_NUM, PIN_LOW);
		rt_pin_write(SYS_RUN_LED_PIN_NUM, PIN_LOW);
		rt_thread_mdelay(100);
		rt_pin_write(SYSTEM_LED_PIN_NUM, PIN_HIGH);
		rt_pin_write(SYS_RUN_LED_PIN_NUM, PIN_HIGH);
		rt_thread_mdelay(100);
	}
}

/*
*******************�⹦�ʼ��**********************
*/
void rt_optic_power_thread_entry(void* parameter)
{
	while(1){
		info.item5.param1=adc_data_a.dc1[10];
		info.item6.param1=adc_data_a.dc2[10];
		if(info.item5.active)     //���lcd��ǰ��ʾ�⹦��,������ʾ
		{
			lcdDisplayItem(info.item5);
		}
		
		if(info.item6.active)     //���lcd��ǰ��ʾ�⹦��,������ʾ
		{
			lcdDisplayItem(info.item6);
		}
		
		if(adc_data_a.dc1[10]<info.item3.param1)
			rt_pin_write(POWER_A_LED_PIN_NUM, PIN_HIGH);
		else
			rt_pin_write(POWER_A_LED_PIN_NUM, PIN_LOW);
		
		if(adc_data_a.dc2[10]<info.item3.param1)
			rt_pin_write(POWER_A_LED_PIN_NUM, PIN_HIGH);
		else
			rt_pin_write(POWER_A_LED_PIN_NUM, PIN_LOW);
		rt_thread_delay(250);

	}
}

/*
*******************��������**********************
*/
void rt_key_thread_entry(void* parameter)
{	
	int key_value;
	
	while(1)
	{
		key_value = KEY_Scan(1);
		switch(key_value)
		{
			case KEY0_PRES:                //back
				key_back_perss();
				rt_kprintf("KEY0_PRES\n");
				break;
			case KEY1_PRES:                //enter
				key_enter_press();
				rt_kprintf("KEY1_PRES\n");
				break;
			case KEY2_PRES:                //sub
				key_sub_press();
				rt_kprintf("KEY2_PRES\n");
				break;
			case KEY3_PRES:                //add
				key_plus_press();
				rt_kprintf("KEY3_PRES\n");
				break;
			case KEY4_PRES:                //down
				key_down_press();
				rt_kprintf("KEY4_PRES\n");
				break;
			case KEY5_PRES:                //up
				key_up_press();
				rt_kprintf("KEY5_PRES\n");
				break;
			default:
				break;	
		}
		
		if(key_value>0)
			;//rt_timer_start(&fresh_timer);
		
		rt_thread_delay(60);
	}
}

static int ota_app_vtor_reconfig(void)
{
    #define NVIC_VTOR_MASK   0x3FFFFF80
    /* Set the Vector Table base location by user application firmware definition */
    SCB->VTOR = RT_APP_PART_ADDR & NVIC_VTOR_MASK;

    return 0;
}
INIT_BOARD_EXPORT(ota_app_vtor_reconfig);
