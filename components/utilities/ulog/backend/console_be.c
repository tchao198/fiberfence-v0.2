/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-09-04     armink       the first version
 */

#include <rthw.h>
#include <ulog.h>
#include <rtthread.h>
#include <dfs_posix.h>
#include <string.h>

#ifdef ULOG_BACKEND_USING_CONSOLE

#if defined(ULOG_ASYNC_OUTPUT_BY_THREAD) && ULOG_ASYNC_OUTPUT_THREAD_STACK < 384
#error "The thread stack size must more than 384 when using async output by thread (ULOG_ASYNC_OUTPUT_BY_THREAD)"
#endif

static struct ulog_backend console, logfile;
static struct rt_semaphore sem_lock;

void ulog_console_backend_output(struct ulog_backend *backend, rt_uint32_t level, const char *tag, rt_bool_t is_raw,
        const char *log, size_t len)
{
    rt_device_t dev = rt_console_get_device();

#ifdef RT_USING_DEVICE
    if (dev == RT_NULL)
    {
        rt_hw_console_output(log);
    }
    else
    {
        rt_uint16_t old_flag = dev->open_flag;

        dev->open_flag |= RT_DEVICE_FLAG_STREAM;
        rt_device_write(dev, 0, log, len);
        dev->open_flag = old_flag;
    }
#else
    rt_hw_console_output(log);
#endif

}

int ulog_console_backend_init(void)
{
    ulog_init();
    console.output = ulog_console_backend_output;

    ulog_backend_register(&console, "console", RT_TRUE);

    return 0;
}
//INIT_PREV_EXPORT(ulog_console_backend_init);

static int count = 0;
void ulog_file_backend_output(struct ulog_backend *backend, rt_uint32_t level, const char *tag, rt_bool_t is_raw,
        const char *log, size_t len)
{
		int fd;
		char write_overage_c[4] = { '\r', '\r', '\r', '\r' };
    size_t write_size_temp = 0;
		write_size_temp = RT_ALIGN_DOWN(len, 4);
	
		rt_sem_take(&sem_lock, RT_WAITING_FOREVER);
		if(count>100000){
			count=0;
			if(!strcmp(tag,"MAIN")  && level==LOG_LVL_ERROR)
				fd = open("/alarmlog.txt", O_WRONLY | O_CREAT | O_TRUNC);
			else
				fd = open("/syslog.txt", O_WRONLY | O_CREAT | O_TRUNC);
		}
		else{
			if(!strcmp(tag,"MAIN") && level==LOG_LVL_ERROR)
				fd = open("/alarmlog.txt", O_WRONLY | O_CREAT | O_APPEND);
			else
				fd = open("/syslog.txt", O_WRONLY | O_CREAT | O_APPEND);
		}
		
		if(fd<0){
			//close(fd);
			rt_sem_release(&sem_lock);
			return;
		}

		write(fd, log, write_size_temp);
		if((write_size_temp != len)){
			memcpy(write_overage_c, log + write_size_temp, len - write_size_temp);
      write(fd, (char *) write_overage_c, sizeof(write_overage_c));
		}
		
		count++;
		fsync(fd);
		close(fd);
		rt_sem_release(&sem_lock);
}

int ulog_file_backend_init(void)
{
    ulog_init();
		rt_sem_init(&sem_lock, "file_lock", 1, RT_IPC_FLAG_FIFO);
	
    logfile.output = ulog_file_backend_output;

    ulog_backend_register(&logfile, "logfile", RT_TRUE);

    return 0;
}
INIT_PREV_EXPORT(ulog_file_backend_init);

void ulog_clean(){
	dfs_mkfs("elm", "W25Q64");
	unlink("/syslog.txt");
}


void read_log(int argc, char **argv){
	int size, fd, len, ret, filesize;
	char buf[81];
	struct stat status;
	
	if(argc==2){
		size = atoi(argv[1]);
		if(size>5000)
			size=5000;
	}
	else if(argc==1){
		size = 500;
	}
	else{
		rt_kprintf("Usage: read_log or read_log size(in bytes)!\n");
		return;
	}
	
	ret = stat("/syslog.txt", &status);
	if(ret == 0)
		filesize = status.st_size;
	else{
		rt_kprintf("No system infomation for read.\n");
		return;
	}
	
	if(size>filesize){
		rt_kprintf("read size is too more.\n");
		return;
	}
	
	rt_sem_take(&sem_lock, RT_WAITING_FOREVER);
	fd = open("/syslog.txt", O_RDONLY);
	lseek(fd, -size, SEEK_END);
	do{
		len=read(fd,buf,80);
		if(len>0)
			rt_kprintf("%s", buf);
		memset(buf, 0, 81);
	}
	while(len>0);
	close(fd);
	rt_sem_release(&sem_lock);
}

void read_alarm(int argc, char **argv){
	int size, fd, len, ret, filesize;
	char buf[81];
	struct stat status;
	
	if(argc==2){
		size = atoi(argv[1]);
		if(size>5000)
			size=5000;
	}
	else if(argc==1){
		size = 500;
	}
	else{
		rt_kprintf("Usage: read_alarm or read_alarm size(in bytes)!\n");
		return;
	}
	
	ret = stat("/alarmlog.txt", &status);
	if(ret == 0)
		filesize = status.st_size;
	else{
		rt_kprintf("No alarm infomation for read.\n");
		return;
	}
	
	if(size>filesize){
		rt_kprintf("read size is too more.\n");
		return;
	}
	
	rt_sem_take(&sem_lock, RT_WAITING_FOREVER);
	fd = open("/alarmlog.txt", O_RDONLY);
	lseek(fd, -size, SEEK_END);
	do{
		len=read(fd,buf,80);
		if(len>0)
			rt_kprintf("%s", buf);
		memset(buf, 0, 81);
	}
	while(len>0);
	close(fd);
	rt_sem_release(&sem_lock);
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT_ALIAS(ulog_clean, ulog_clean, clean ulog);
MSH_CMD_EXPORT_ALIAS(read_log, read_log, read syslog);
MSH_CMD_EXPORT_ALIAS(read_alarm, read_alarm, read alarmlog);
#endif /* RT_USING_FINSH */

#endif /* ULOG_BACKEND_USING_CONSOLE */
