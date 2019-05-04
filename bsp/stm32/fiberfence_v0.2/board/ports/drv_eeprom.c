/*
 * File      : drv_ft5x06.c
 *             ft5x06 touch driver
 * COPYRIGHT (C) 2006 - 2017, RT-Thread Development Team
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
 * 2017-08-08     Yang        the first version
 */
 
#include <rtthread.h>
#include <rtdevice.h>

#ifdef RT_USING_FINSH
#include <finsh.h>
#endif

#include "board.h"

#define SWAP16(v) ((v>>8)&0xff)|((v<<8)&0xff00)

#define I2CBUS_NAME             "i2c1"
#define AT24CXX_ADDR            0x50
#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINTF(...)           rt_kprintf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)   
#endif


static struct rt_i2c_bus_device *_i2c_bus;

int AT24CXX_Read(rt_uint16_t addr,
                        void *buf,
                        size_t len)
{
    struct rt_i2c_msg msgs[2];
	
		addr = SWAP16(addr);         //先发送高地址

    msgs[0].addr  = AT24CXX_ADDR;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf   = (rt_uint8_t*)&addr;
    msgs[0].len   = sizeof(addr);

    msgs[1].addr  = AT24CXX_ADDR;
    msgs[1].flags = RT_I2C_RD;
    msgs[1].buf   = buf;
    msgs[1].len   = len;

    if (rt_i2c_transfer(_i2c_bus, msgs, 2) == 2)
        return len;
    else
        return -1;
}
RTM_EXPORT(AT24CXX_Read);

int AT24CXX_Write(rt_uint16_t addr,
                        void *buf,
                        size_t len)
{
		int i;
		rt_uint8_t temp[sizeof(addr)+len];
    struct rt_i2c_msg msgs[1];
	
		temp[0] = (addr>>8)&0xff;    //MSB
	  temp[1] = addr&0xff;         //LSB
	
		for(i=sizeof(addr); i<sizeof(addr)+len; i++)
			temp[i] = ((rt_uint8_t *)buf)[i-sizeof(addr)];

    msgs[0].addr  = AT24CXX_ADDR;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf   = (rt_uint8_t*)&temp;
    msgs[0].len   = sizeof(addr)+len;

    if (rt_i2c_transfer(_i2c_bus, msgs, 1) == 1)
        return len;
    else
        return -1;
}
RTM_EXPORT(AT24CXX_Write);

#ifdef RT_USING_FINSH
static int test_at24cxx(rt_uint16_t addr)
{
	rt_uint8_t *buf = "this is a test for IIC read and write!";
	rt_uint8_t temp[100]={0}, tt[50]={0};
	AT24CXX_Write(addr, buf, strlen((char*)buf));    //不能跨page写
	rt_thread_delay(5);
	AT24CXX_Read(addr, temp, strlen((char*)buf));
	rt_kprintf("%s\n",temp);
	
	rt_device_t dev;
	dev = rt_device_find(I2CBUS_NAME);
	
	uint16_t maddr = (addr>>8)|(addr<<8);
	uint32_t pos = RT_I2C_WR << 16;
	pos |= AT24CXX_ADDR;
	rt_device_write(dev, pos, &maddr, 2);
	
	pos |= RT_I2C_RD << 16;
	rt_device_read(dev, pos, tt, strlen((char*)buf));
	rt_kprintf("%s\n", tt);
	
	
	return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(test_at24cxx, t_at24c, search at24cxx chip);
#endif

#ifdef RT_USING_FINSH
static int check_at24cxx(void)
{
		rt_uint8_t buf;
    AT24CXX_Read(255,&buf,1);
		if(buf==0x55)
			return 0;
		else
		{
			AT24CXX_Write(255,&buf,1);
			AT24CXX_Read(255,&buf,1);
			if(buf==0x55)
				return 0;
			else
				return -1;
		}
}
FINSH_FUNCTION_EXPORT_ALIAS(check_at24cxx, c_at24c, search at24cxx chip);
#endif

int at24cxx_hw_init(void)
{
    rt_device_t dev;

    dev = rt_device_find(I2CBUS_NAME);
    if (!dev) 
    {
        DEBUG_PRINTF("search device failed: %s\n", I2CBUS_NAME);
        return -1;
    }

    if (rt_device_open(dev, RT_DEVICE_OFLAG_RDWR) != RT_EOK)
    {
        DEBUG_PRINTF("open device failed: %s\n", I2CBUS_NAME);
        return -1;
    }

    DEBUG_PRINTF("at24cxx set i2c bus to %s\n", I2CBUS_NAME);
    _i2c_bus = (struct rt_i2c_bus_device *)dev;

    return 0;
}
INIT_APP_EXPORT(at24cxx_hw_init);
