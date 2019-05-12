#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <dfs_posix.h>

#include "string.h"

#define DRV_DEBUG
#define LOG_TAG             "UPDATE"
#include <drv_log.h>

#include <fal.h>

static int data_process(uint8_t *buf, int len);

typedef struct{
    uint16_t magic;
    uint16_t pkgSize;
    uint32_t pkgId;
		uint32_t fileSize;
} Header_t;

static int update_file_cur_size = 0;

static void update_server_entry(void* paramemter)
{
    int sock, ret;
    int bytes_read;
    uint8_t *recv_data;
    rt_uint32_t addr_len;
    struct sockaddr_in server_addr, client_addr;

    recv_data = rt_malloc(1500);
    if (recv_data == RT_NULL)
    {
        rt_kprintf("No memory\n");
        return;
    }

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        rt_kprintf("Socket error\n");
        rt_free(recv_data);
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5001);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    if (bind(sock, (struct sockaddr *) &server_addr, sizeof(struct sockaddr))== -1)
    {
        rt_kprintf("Bind error\n");
        rt_free(recv_data);
        return;
    }

    addr_len = sizeof(struct sockaddr);
    rt_kprintf("Update Server Waiting for client on port 5001...\n");

    while (1)
    {
        bytes_read = recvfrom(sock, recv_data, 1500, 0,
                              (struct sockaddr *) &client_addr, &addr_len);			
			
				if(bytes_read <=0 )
				{
						rt_kprintf("recive error\n");
						continue;
				}
				ret = data_process(recv_data, bytes_read);
				if(ret)
					break;
    }
		
		close(sock);
		rt_free(recv_data);

    return;
}

static void start_update()
{
	rt_thread_t uid;
	uid = rt_thread_create("start_update", update_server_entry, NULL, 2048, 15, 20);
	if(uid!=NULL)
		rt_thread_startup(uid);
}

static int flash_download(uint8_t *buf, int len, int cur_size)
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

static int flash_erase(int size)
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

static int data_process(uint8_t *buf, int len)
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
				
				return 0;
			}
			
			if(header->pkgId==0xffffffff){
				if(update_file_cur_size != header->fileSize){
					rt_kprintf("update failed!\n");
				}
				rt_kprintf("update complete!\n");
				update_file_cur_size=0;
				return 1;
			}
			
			flash_download((uint8_t *)(buf+12), header->pkgSize, update_file_cur_size);
			update_file_cur_size += header->pkgSize;
			rt_kprintf("update_file_cur_size:%d, total filezie:%d\n", update_file_cur_size, header->fileSize);
			if(update_file_cur_size == header->fileSize){
				rt_kprintf("update succesfull, update file size: %d\n", update_file_cur_size);
				//update_file_cur_size=0;
			}
		}
		else{
			buf[len]='\0';
			rt_kprintf("%s\r\n",buf);  //打印接收到的数据
		}
		
		return 0;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT_ALIAS(start_update, start_update, start to update firmware);
#endif
