#include "udp_demo.h"

#include "lwip/api.h"
#include "lwip/lwip_sys.h"
//#include "lwip/ip4.h"
#include "lwip/ip_addr.h"
#include "string.h"

#define DRV_DEBUG
#define LOG_TAG             "NET"
#include <drv_log.h>

#include <fal.h>
 
//TCP客户端任务
#define UDP_PRIO		6
//任务堆栈大小
#define UDP_STK_SIZE	300


typedef struct{
    uint16_t magic;
    uint16_t pkgSize;
    uint32_t pkgId;
		uint32_t fileSize;
} Header_t;

//extern struct rt_mailbox mb;
//extern rt_sem_t sem;
//extern volatile uint32_t CT_flag;
static int update_file_cur_size = 0;
int flash_download(uint8_t *buf, int len, int cur_size);
int flash_erase(int size);
void data_process(uint8_t *buf, int len);

uint8_t udp_demo_recvbuf[2048];	//UDP接收数据缓冲区
//UDP发送数据内容
const uint8_t *udp_demo_sendbuf="Apollo STM32F4/F7 NETCONN UDP demo send data\r\n";
uint8_t udp_flag = 0x80;							//UDP数据发送标志位
uint32_t data_len = 0;
struct pbuf *q;

//udp任务函数
void udp_demo_test(void *parameter)
{
	LOG_I("udp_test thread is created.");
	err_t err;
	struct netconn *udpconn;
	struct netbuf  *recvbuf;
//	struct netbuf  *sentbuf;
	ip_addr_t destipaddr, localipaddr;
	
	udpconn = netconn_new(NETCONN_UDP);  //创建一个UDP链接
	udpconn->recv_timeout = 10; 
	
	if(udpconn != NULL)  //创建UDP连接成功
	{
		IP4_ADDR(&localipaddr, 192, 168, 1, 250);
		err = netconn_bind(udpconn,&localipaddr,UDP_DEMO_PORT); 
		
		IP4_ADDR(&destipaddr, 192, 168, 1, 102); //构造目的IP地址
    netconn_connect(udpconn,&destipaddr,8088); 	//连接到远端主机
		if(err == ERR_OK)//绑定完成
		{
			while(1)
			{
//				if((udp_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //有数据要发送
//				{
//					//rt_kprintf("udp_send_data\n");
//					
//					sentbuf = netbuf_new();
//					if(sentbuf==NULL)
//						rt_kprintf("netbuf_new failed\n");
//					netbuf_alloc(sentbuf,strlen((char *)udp_demo_sendbuf));
//					memcpy(sentbuf->p->payload,(void*)udp_demo_sendbuf,strlen((char*)udp_demo_sendbuf));
//					
//					err = netconn_send(udpconn,sentbuf);  	//将netbuf中的数据发送出去
//					if(err != ERR_OK)
//					{
//						//rt_kprintf("发送失败:%d\r\n", err);
//					}
//					//udp_flag &= ~LWIP_SEND_DATA;	//清除数据发送标志
//					netbuf_delete(sentbuf);      	//删除buf
//				}
//				rt_thread_delay(1);  //延时5ms
				
				netconn_recv(udpconn,&recvbuf); //接收数据
				if(recvbuf != NULL)          //接收到数据
				{ 
					//rt_kprintf("recive ok\n");
					//OS_ENTER_CRITICAL(); //关中断
					memset(udp_demo_recvbuf,0,UDP_DEMO_RX_BUFSIZE);  //数据接收缓冲区清零
					for(q=recvbuf->p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
					{
						//判断要拷贝到UDP_DEMO_RX_BUFSIZE中的数据是否大于UDP_DEMO_RX_BUFSIZE的剩余空间，如果大于
						//的话就只拷贝UDP_DEMO_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
						if(q->len > (UDP_DEMO_RX_BUFSIZE-data_len)) memcpy(udp_demo_recvbuf+data_len,q->payload,(UDP_DEMO_RX_BUFSIZE-data_len));//拷贝数据
						else memcpy(udp_demo_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > UDP_DEMO_RX_BUFSIZE) break; //超出TCP客户端接收数组,跳出	
					}
					//OS_EXIT_CRITICAL();  //开中断
					
					//rt_kprintf("%s\r\n",udp_demo_recvbuf);  //打印接收到的数据
					data_process(udp_demo_recvbuf, data_len);
					data_len=0;  //复制完成后data_len要清零。
					netbuf_delete(recvbuf);      //删除buf
					//udp_flag |= 0x80;
				}else rt_thread_delay(5);  //延时5ms
			}
			
			//netbuf_delete(sentbuf);      	//删除buf
			
		}else rt_kprintf("UDP绑定失败\r\n");
	}else rt_kprintf("UDP连接创建失败\r\n");
}

int flash_download(uint8_t *buf, int len, int cur_size)
{
		char *recv_partition = "download";
		const struct fal_partition * dl_part = RT_NULL;
		
		if ((dl_part = fal_partition_find(recv_partition)) == RT_NULL)
		{
				LOG_E("Firmware download failed! Partition (%s) find error!", recv_partition);
				return -1;
		}
		

		if (fal_partition_write(dl_part, cur_size, (uint8_t *)buf, len) < 0)
		{
				LOG_E("Firmware download failed! Partition (%s) write data error!", dl_part->name);
				return -1;
		}
				
		return 0;
}

int flash_erase(int size)
{
		char *recv_partition = "download";
		const struct fal_partition * dl_part = RT_NULL;
	
		if ((dl_part = fal_partition_find(recv_partition)) == RT_NULL)
		{
				LOG_E("Firmware download failed! Partition (%s) find error!", recv_partition);
				return -1;
		}
		
		if (fal_partition_erase(dl_part, 0, size) < 0)
		{
				LOG_E("Firmware download failed! Partition (%s) erase error!", dl_part->name);
				return -1;
		}
		return 0;
}

void data_process(uint8_t *buf, int len)
{
		int ret=0;
		Header_t *header;
		header = (Header_t *)buf;
		if(header->magic == 0x1234){
			if(header->pkgId == 0){
				rt_kprintf("\nStart to erase download partition!\n");
				ret=flash_erase(header->fileSize);
				if(ret==0){
					rt_kprintf("Erase download partition succesfull!\n");
					rt_kprintf("Start to update firmware!\n\n");
				}
				else
					rt_kprintf("Erase download partition failed!\n\n");
				
				return;
			}
			
			flash_download((uint8_t *)(buf+12), header->pkgSize, update_file_cur_size);
			update_file_cur_size += header->pkgSize;
			rt_kprintf("update_file_cur_size:%d, total filezie:%d\n", update_file_cur_size, header->fileSize);
			if(update_file_cur_size == header->fileSize){
				rt_kprintf("update succesfull, update file size: %d\n", update_file_cur_size);
				update_file_cur_size=0;
			}
		}
		else{
			rt_kprintf("%s\r\n",buf);  //打印接收到的数据
		}
		
		
}

void cmd_flash_erase(int argc, char **argv)
{
	flash_erase(262144);
}

static rt_thread_t udpid;
	
void start_update()
{
	udpid = rt_thread_create("udp_test", udp_demo_test, RT_NULL,
													2048, 15, 20);
	if (udpid != RT_NULL)
			rt_thread_startup(udpid);
}

void stop_update()
{
	rt_thread_delete(udpid);
}


#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT_ALIAS(cmd_flash_erase, flash_erase, erase download partition of flash);
MSH_CMD_EXPORT_ALIAS(start_update, start_update,);
MSH_CMD_EXPORT_ALIAS(stop_update, stop_update,);
#endif
