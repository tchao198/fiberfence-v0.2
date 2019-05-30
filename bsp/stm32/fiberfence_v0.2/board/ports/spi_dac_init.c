#include <rtthread.h>
#include "drv_spi.h"
#include <string.h>
#include <finsh.h>

static struct rt_spi_device *dac_spi_device2, *dac_spi_device3;

static int rt_hw_spi_dac_init(void)
{
    __HAL_RCC_GPIOI_CLK_ENABLE();
    rt_hw_spi_device_attach("spi2", "spi20", GPIOI, GPIO_PIN_0);
	
		__HAL_RCC_GPIOE_CLK_ENABLE();
    rt_hw_spi_device_attach("spi3", "spi30", GPIOE, GPIO_PIN_0);

    return RT_EOK;
}
INIT_COMPONENT_EXPORT(rt_hw_spi_dac_init);

int dac2_init()
{
	dac_spi_device2 = (struct rt_spi_device*)rt_device_find("spi20");
	if(dac_spi_device2==NULL)
	{
		rt_kprintf("spi_device can not find!\n");
		return -1;
	}
	
	rt_device_open(&dac_spi_device2->parent, RT_DEVICE_FLAG_RDWR);
	//rt_device_close(&adc_spi_device->parent);
	
	/* config spi */
	{
			struct rt_spi_configuration cfg;
			cfg.data_width = 16;
			cfg.mode = RT_SPI_MODE_2 | RT_SPI_MSB | RT_SPI_MASTER; /* SPI Compatible: Mode 0 and Mode 3 */
			cfg.max_hz = 50 * 1000 * 1000; /* 50M */
			rt_spi_configure(dac_spi_device2, &cfg);
	}
	
	
	return 0;
}
INIT_COMPONENT_EXPORT(dac2_init);

int dac3_init()
{
	dac_spi_device3 = (struct rt_spi_device*)rt_device_find("spi30");
	if(dac_spi_device3==NULL)
	{
		rt_kprintf("spi_device can not find!\n");
		return -1;
	}
	
	rt_device_open(&dac_spi_device3->parent, RT_DEVICE_FLAG_RDWR);
	//rt_device_close(&adc_spi_device->parent);
	
	/* config spi */
	{
			struct rt_spi_configuration cfg;
			cfg.data_width = 16;
			cfg.mode = RT_SPI_MODE_2 | RT_SPI_MSB | RT_SPI_MASTER; /* SPI Compatible: Mode 0 and Mode 3 */
			cfg.max_hz = 50 * 1000 * 1000; /* 50M */
			rt_spi_configure(dac_spi_device3, &cfg);
	}
	
	
	return 0;
}
INIT_COMPONENT_EXPORT(dac3_init);


int set_dac(rt_uint16_t value, rt_uint16_t chip_id)
{
		struct rt_spi_device *device;
		if(chip_id==2)
			device = (struct rt_spi_device *) rt_device_find("spi20");
		else if(chip_id==3)
			device = (struct rt_spi_device *) rt_device_find("spi30");
		else
			return -1;
	
		rt_spi_send(device, &value, 2);
		return 0;
}

int set_spi_dac(int argc, char **argv)
{
	
	rt_uint16_t chip_id, value;
	if(argc==3){
		chip_id = atoi(argv[1]);
		value = atoi(argv[2]);
		set_dac(value, chip_id);
	}
	return 0;
}
MSH_CMD_EXPORT(set_spi_dac, set spi dac example:set_spi_dac chip_id value)
