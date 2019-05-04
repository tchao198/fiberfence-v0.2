#include <rtthread.h>
#include "key_app.h"
#include <dfs_posix.h>
#include <string.h>

extern struct Display_Info info;
extern struct rt_mailbox mb_a, mb_b;

struct Params{
	rt_uint16_t alarm_A_th;
	rt_uint16_t alarm_B_th;
	rt_uint16_t power_th;
	rt_uint16_t alarm_interval;
	rt_uint16_t addr1;
	rt_uint16_t addr2;
	rt_uint16_t addr3;
	rt_uint16_t addr4;
	rt_uint16_t port;
};

enum KEY_VALUE
{
	KEY_UP=1,
	KEY_DOWN,
	KEY_PLUS,
	KEY_SUB,
	KEY_ENTER,
	KEY_RETURN
};

void key_test(enum KEY_VALUE value)
{
	switch(value)
	{
		case KEY_UP:
			key_up_press();
			break;
		case KEY_DOWN:
			key_down_press();
			break;
		case KEY_PLUS:
			key_plus_press();
			break;
		case KEY_SUB:
			key_sub_press();
			break;
		case KEY_ENTER:
			key_enter_press();
			break;
		case KEY_RETURN:
			key_back_perss();
			break;
		default:
			break;
	}
}

#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT_ALIAS(key_test, __cmd_key_test, key test. e.g: key_test(KEY_UP));
#endif

void list_all_param()
{
	rt_kprintf("--------Fiber Fence System Parameter List Tab-------\n");
	rt_kprintf("alarm threshold A:      %4d\n", info.item1.param1);
	rt_kprintf("alarm threshold B:      %4d\n", info.item2.param1);
	rt_kprintf("optic power threshold:  %4d\n", info.item3.param1);
	rt_kprintf("alarm time interval:    %4d\n", info.item4.param1);
	rt_kprintf("optic power A:          %4d\n", info.item5.param1);
	rt_kprintf("optic power B:          %4d\n", info.item6.param1);
	rt_kprintf("alarm count A:          %4d\n", info.item7.param1);
	rt_kprintf("alarm count B:          %4d\n", info.item8.param1);
	rt_kprintf("mailbox count: %d ,  %d\n", mb_a.entry, mb_b.entry);
	rt_kprintf("---------------------end----------------------------\n");
}

extern int AT24CXX_Write(rt_uint16_t addr,
                        void *buf,
                        size_t len);
extern int AT24CXX_Read(rt_uint16_t addr,
                        void *buf,
                        size_t len);

//void save_parameter()
//{
//	struct Params P = {2200,
//										2200,
//										300,
//										5,
//										192,
//										168,
//										103,
//										85,
//										5900};
//	AT24CXX_Write(0,(char *)&P, sizeof(struct Params));
//}

//void load_parameter()
//{
//	struct Params P;
//	AT24CXX_Read(0,(char *)&P, sizeof(struct Params));
//	rt_kprintf("%d.%d.%d.%d\n", P.addr1,P.addr2,P.addr3,P.addr4);
//}


int get_line(int fd, char *buffer)
{
	int i=0, len;
	while(1){
		len = read(fd, &buffer[i], 1); 
		if(len<1)
			break;
		if(buffer[i]=='\n')
			break;
		i++;
	}
	buffer[i]='\0';
	return i;
}

void test_fgets()
{
	int fd, length;
	char buf[128];
	fd = open("/alarm.txt", O_RDONLY, 0); 
	if (fd < 0) 
	{ 
			rt_kprintf("check: open file for read failed\n"); 
			return; 
	} 
	do{
		length=get_line(fd, buf);
		rt_kprintf("%s(length:%d)\n", buf, length);
	}while(length!=0);
	close(fd);
	
}

void save_config()
{
	char buf[256];
	int fd, len;
	fd = open("/config.ini", O_WRONLY | O_CREAT | O_TRUNC, 0);
	if (fd < 0) 
	{ 
			rt_kprintf("check: open file for read failed\n"); 
			return; 
	} 
	
	len=sprintf(buf, "alarm_threshold_A:%d\nalarm_threshold_B:%d\n"
										"optic_power_threshold:%d\nalarm_time_interval:%d\n",
											info.item1.param1, info.item2.param1,
											info.item3.param1, info.item4.param1);
	write(fd, buf, len);
	close(fd);	
}

void load_config()
{
	int fd, length;
	char buf[128];
	fd = open("/config.ini", O_RDONLY, 0); 
	if (fd < 0) 
	{ 
			rt_kprintf("check: open file for read failed\n"); 
			return; 
	} 
	do{
		length=get_line(fd, buf);
		char *p; 
    p = strtok(buf, ":");
		if(strcmp(p, "alarm_threshold_A")==0){
				p = strtok(NULL, ":");
				info.item1.param1=atoi(p);
		}
		else if(strcmp(p, "alarm_threshold_B")==0){
				p = strtok(NULL, ":");
				info.item2.param1=atoi(p);
		}
		else if(strcmp(p, "optic_power_threshold")==0){
				p = strtok(NULL, ":");
				info.item3.param1=atoi(p);
		}
		else if(strcmp(p, "alarm_time_interval")==0){
				p = strtok(NULL, ":");
				info.item4.param1=atoi(p);
		}	

		//rt_kprintf("%s\n", buf, length);
	}while(length!=0);
	close(fd);
}

void reset_config()
{
	info.item1.param1=ALARM_THRESHOLD_A;
	info.item2.param1=ALARM_THRESHOLD_B;
	info.item3.param1=OPTIC_POWER_THRESHOLD;
	info.item4.param1=ALARM_INTERVAL;
}
RTM_EXPORT(reset_config);

#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT_ALIAS(list_all_param, __cmd_lap, list all parameter);
//FINSH_FUNCTION_EXPORT(save_parameter,);
//FINSH_FUNCTION_EXPORT(load_parameter,);
FINSH_FUNCTION_EXPORT(test_fgets,);
FINSH_FUNCTION_EXPORT_ALIAS(save_config, __cmd_save_config, save configure parameters to sdcard);
FINSH_FUNCTION_EXPORT_ALIAS(load_config, __cmd_load_config, load configure parameters from sdcard);
FINSH_FUNCTION_EXPORT_ALIAS(reset_config, __cmd_reset_config, reaset configure parameters to default);
#endif
