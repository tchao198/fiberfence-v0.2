#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <dfs_posix.h>
#include <rtthread.h>

#include "string.h"

#define DRV_DEBUG
#define LOG_TAG             "UPDATE_TCP"
#include <drv_log.h>

#include <fal.h>

//static int data_process(uint8_t *buf, int len);
static int flash_erase(int size);
static int flash_download(uint8_t *buf, int len, int cur_size);

typedef struct{
    uint32_t magic;
    uint32_t totalSize;
		uint32_t fileSize;
		uint32_t RVS;
} Header_t;

static void update_server_entry(void* paramemter)
{
    int sock, cnn_sock;
    int bytes_read;
		Header_t *header;
		uint32_t file_len, total_rcv_len=0;
    uint8_t *recv_data;
    rt_uint32_t addr_len;
    struct sockaddr_in server_addr, client_addr;

    recv_data = rt_malloc(1500);
    if (recv_data == RT_NULL)
    {
        rt_kprintf("No memory\n");
        return;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        rt_kprintf("Socket error\n");
        rt_free(recv_data);
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5002);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    if (bind(sock, (struct sockaddr *) &server_addr, sizeof(struct sockaddr))== -1)
    {
        rt_kprintf("Bind error\n");
        rt_free(recv_data);
        return;
    }

		if(listen(sock, 1)<0){
			rt_kprintf("listen error\n");
			rt_free(recv_data);
			return;
		}
		
    addr_len = sizeof(struct sockaddr);
		
		cnn_sock = accept(sock, (struct sockaddr*)&client_addr, &addr_len);
		
		if(cnn_sock<0){
			rt_kprintf("connect error\n");
			close(sock);
			return;
		}
		else{
			rt_kprintf("client from %s:%u connected\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		}
		
		close(sock);
		
		while(1){
			bytes_read = recv(cnn_sock, recv_data, 16, 0);
			if(bytes_read==16){
				//rt_kprintf("recive ok len:%d\n", bytes_read);
				break;
			}
			else if(bytes_read<0){
				rt_kprintf("client disconnected!\n");
				goto exit;
			}
			else{
				rt_kprintf("header error len:%d\n", bytes_read);
				rt_thread_delay(30);
				continue;
			}
		}
		
		header = (Header_t *)recv_data;
		if(header->magic!=0x12345678){
			rt_kprintf("magic error! magic:%08x\n", header->magic);
			goto exit;
		}
		
		rt_kprintf("\nStart to erase download partition!\n");
		file_len = header->fileSize;
		if(flash_erase(file_len)){
			rt_kprintf("Erase download partition failed!\n");
			goto exit;
		}
			
		rt_kprintf("Erase download partition succesfull!\n");
		rt_kprintf("Start to update firmware!\n\n");
		
		while(file_len>total_rcv_len){
			bytes_read = recv(cnn_sock, recv_data, 1024, 0);
			if(bytes_read<0){
				rt_kprintf("recive data error\n");
				break;
			}
			
			flash_download(recv_data, bytes_read, total_rcv_len);
			//file_len-=bytes_read;
			total_rcv_len+=bytes_read;
			rt_kprintf("file_len:%d, total_rcv_len:%d\n", file_len, total_rcv_len);
		}
		
		if(file_len!=total_rcv_len){
			rt_kprintf("firmware update succesfull\n");
			goto exit;
		}
		
		header->magic=0x12345678;
		header->fileSize=total_rcv_len;
		header->RVS=0;
		header->totalSize=total_rcv_len+16;
		send(cnn_sock, recv_data, 16, 0);
		rt_kprintf("firmware update succesfull, total_size:%d\n", total_rcv_len);
		
		exit:
		close(cnn_sock);
		rt_free(recv_data);

		rt_kprintf("TCP Update Server close!\n");
    return;
}

static void start_update_tcp()
{
	rt_thread_t uid;
	uid = rt_thread_create("start_update_tcp", update_server_entry, NULL, 2048, 15, 20);
	if(uid!=NULL)
		rt_thread_startup(uid);
	rt_kprintf("Update Server Waiting for Tcp client on port 5002...\n");
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

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT_ALIAS(start_update_tcp, start_update_tcp, start to update firmware);
#endif
