#include <rtthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <dfs_posix.h>
#include "displayInfo.h"

#define DRV_DEBUG
#define LOG_TAG             "UDP_SERVER"
#include <drv_log.h>

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

extern void cpu_usage_get(rt_uint8_t *major, rt_uint8_t *minor);
extern int set_dac(rt_uint16_t value, rt_uint16_t chip_id);

struct Cmd_Data{
    uint16_t magic;
    uint16_t cmd;
    uint16_t p1;
    uint16_t p2;
};

static void response(int sock, int cmd, struct sockaddr *client_addr, int addr_len)
{
	struct Cmd_Data r;
	r.magic = 0x1234;
	r.cmd = cmd;
	r.p1 = 0xffff;
	r.p2 = 0xffff;
	sendto(sock, (char *)&r, sizeof(struct Cmd_Data), 0, client_addr, addr_len);
}

/*********************************************************
* 函数名：cmd_process
* 
* 功  能：处理接收到的命令
*********************************************************/
static void cmd_process(int sock, struct Cmd_Data *cd, struct sockaddr *client_addr, int addr_len)
{
	switch(cd->cmd)
	{
		//查询CPU使用率
		case CMD_CPU_USAGE:
		{
			uint8_t major, minor;
			cpu_usage_get(&major, &minor);
			cd->p1 = major;
			cd->p2 = minor;
			sendto(sock, (char *)cd, sizeof(struct Cmd_Data), 0, client_addr, addr_len);
			LOG_I("Query cpu usage: %d.%d%%", major, minor);
			break;
		}
		//查询光功率
		case CMD_OPTIC_POWER:
		{
			cd->p1 = info.item5.param1;
			cd->p2 = info.item6.param1;
			sendto(sock, (char *)cd, sizeof(struct Cmd_Data), 0, client_addr, addr_len);
			LOG_I("Query optic power: %d, %d", info.item5.param1, info.item6.param1);
			break;
		}
		//查询报警数目
		case CMD_ALARM_COUNT:
		{
			cd->p1 = info.item7.param1;
			cd->p2 = info.item8.param1;
			sendto(sock, (char *)cd, sizeof(struct Cmd_Data), 0, client_addr, addr_len);
			LOG_I("Query alarm count: %d, %d", info.item7.param1,info.item8.param1);
			break;
		}
		//设置防区A信号增益
		case CMD_DAC_GAIN_A:
		{
			set_dac(cd->p1, 2);
			sendto(sock, (char *)cd, sizeof(struct Cmd_Data), 0, client_addr, addr_len);
			LOG_I("Set dac gain(A): %d", cd->p1);
			break;
		}
		//设置防区B信号增益
		case CMD_DAC_GAIN_B:
		{
			set_dac(cd->p1, 3);
			sendto(sock, (char *)cd, sizeof(struct Cmd_Data), 0, client_addr, addr_len);
			LOG_I("Set dac gain(B): %d", cd->p1);
			break;
		}
		//设置AD采样率
		case CMD_ADC_SAMPLE_FRQ:
			response(sock, CMD_RESPONSE_OK, client_addr, addr_len);
			LOG_I("Set adc sample frequency: %d", cd->p1);
			break;
		//设置防区A报警阈值
		case CMD_ALARM_THRESHOLD_A:
		{
			info.item1.param1=cd->p1;
			sendto(sock, (char *)cd, sizeof(struct Cmd_Data), 0, client_addr, addr_len);
			LOG_I("Set alarm threshold(A): %d", cd->p1);
			break;
		}
		//设置防区B报警阈值
		case CMD_ALARM_THRESHOLD_B:
		{
			info.item2.param1=cd->p1;
			sendto(sock, (char *)cd, sizeof(struct Cmd_Data), 0, client_addr, addr_len);
			LOG_I("Set alarm threshold(B): %d", cd->p1);
			break;
		}
		//设置光功率报警阈值
		case CMD_POWER_THRESHOLD:
		{
			info.item3.param1=cd->p1;
			sendto(sock, (char *)cd, sizeof(struct Cmd_Data), 0, client_addr, addr_len);
			LOG_I("Set optic power threshold: %d", cd->p1);
			break;
		}
		//设置报警时间间隔
		case CMD_ALARM_INTERVAL:
		{
			info.item4.param1=cd->p1;
			sendto(sock, (char *)cd, sizeof(struct Cmd_Data), 0, client_addr, addr_len);
			LOG_I("Set alarm interval: %d", cd->p1);
			break;
		}
			
		//未知命令
		default:
			response(sock, CMD_RESPONSE_UNKNOW, client_addr, addr_len);
			LOG_I("unknow CMD");
			break;
	}
}

static void udp_server_entry(void* paramemter)
{
	int sock;
    int bytes_read;
    char *recv_data;
		struct Cmd_Data *cmd_data;
    rt_uint32_t addr_len;
    struct sockaddr_in server_addr, client_addr;

    /* 分配接收用的数据缓冲 */
    recv_data = rt_malloc(1024);
    if (recv_data == RT_NULL)
    {
        /* 分配内存失败，返回 */
        rt_kprintf("No memory\n");
        return;
    }

    /* 创建一个socket，类型是SOCK_DGRAM，UDP类型 */
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        rt_kprintf("Socket error\n");

        /* 释放接收用的数据缓冲 */
        rt_free(recv_data);
        return;
    }

    /* 初始化服务端地址 */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    /* 绑定socket到服务端地址 */
    if (bind(sock, (struct sockaddr *) &server_addr, sizeof(struct sockaddr))
            == -1)
    {
        /* 绑定地址失败 */
        rt_kprintf("Bind error\n");

        /* 释放接收用的数据缓冲 */
        rt_free(recv_data);
        return;
    }

    addr_len = sizeof(struct sockaddr);
    rt_kprintf("UDPServer Waiting for client on port 5000...\n");
	
	while (1)
    {
        /* 从sock中收取最大1024字节数据 */
        bytes_read = recvfrom(sock, recv_data, 1024, 0,
                              (struct sockaddr *) &client_addr, &addr_len);			
			
				if(bytes_read <=0 )
				{
						rt_kprintf("recive error\n");
						continue;
				}

				cmd_data=(struct Cmd_Data *)recv_data;
				
				/* 校验帧头 */
				if(cmd_data->magic!=0x1234)
				{
						rt_kprintf("magic error\n");
						continue;
				}
				/* 处理接收到命令 */
				cmd_process(sock, cmd_data, (struct sockaddr *) &client_addr, addr_len);
    }
		
		close(sock);
		rt_free(recv_data);
		
		return;
}

static int start_udp_server()
{
	rt_thread_t uid;
	uid = rt_thread_create("udp_control_server", udp_server_entry, NULL, 1024, 12, 20);
	if(uid!=NULL){
		rt_thread_startup(uid);
	}
	//rt_kprintf("Udp control Server Waiting for control client on port 5000...\n");
	//rt_kprintf("Udp controld\n");
	//rt_kprintf("Udp controld\n");
	return 0;
}
INIT_APP_EXPORT(start_udp_server);
//#ifdef RT_USING_FINSH
//#include <finsh.h>
//MSH_CMD_EXPORT_ALIAS(start_udp_server, start_udp_server, start_udp_server);
//#endif



