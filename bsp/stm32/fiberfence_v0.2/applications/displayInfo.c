#include <rtthread.h>
#include "displayInfo.h"

int current_display_id = 0;
struct Display_Info info;

/* 初始化lcd显示内容 */
int info_init(void)
{
	struct Display_Item alarm_A_th_item, alarm_B_th_item, optic_power_th_item, 
				optic_power_A_item, optic_power_B_item, 
				alarm_count_A_item, alarm_count_B_item, 
				alarm_time_interval_item,
				reset_item,
				save_config_item,
				load_config_item;
	
	alarm_A_th_item.active = 0;
	alarm_A_th_item.label = "alarm_A_th:";
	alarm_A_th_item.param1 = ALARM_THRESHOLD_A;
	
	alarm_B_th_item.active = 0;
	alarm_B_th_item.label = "alarm_B_th:";
	alarm_B_th_item.param1 = ALARM_THRESHOLD_B;
	
	optic_power_th_item.active = 0;
	optic_power_th_item.label  = "power_th:";
	optic_power_th_item.param1 = OPTIC_POWER_THRESHOLD;
	
	alarm_time_interval_item.active = 0;
	alarm_time_interval_item.label  = "alarm_time:";
	alarm_time_interval_item.param1 = ALARM_INTERVAL;
	
	optic_power_A_item.active = 0;
	optic_power_A_item.label  = "power_A:";
	optic_power_A_item.param1 = 0;
	
	optic_power_B_item.active = 0;
	optic_power_B_item.label  = "power_B:";
	optic_power_B_item.param1 = 0;
	
	alarm_count_A_item.active = 0;
	alarm_count_A_item.label  = "alarm_count_A:";
	alarm_count_A_item.param1 = 0;
	
	alarm_count_B_item.active = 0;
	alarm_count_B_item.label  = "alarm_count_B:";
	alarm_count_B_item.param1 = 0;
	
	reset_item.active=0;
	reset_item.label="RESET_CONFIG";
	
	save_config_item.active=0;
	save_config_item.label="SAVE_CONFIG";
	
	load_config_item.active=0;
	load_config_item.label="LOAD_CONFIG";
	
	info.active = 1;
	info.root_label1 = "Welcome";
	info.root_label2 = "Fiber Fence";
	info.item1 = alarm_A_th_item;
	info.item2 = alarm_B_th_item;
	info.item3 = optic_power_th_item;
	info.item4 = alarm_time_interval_item;
	info.item5 = optic_power_A_item;
	info.item6 = optic_power_B_item;
	info.item7 = alarm_count_A_item;
	info.item8 = alarm_count_B_item;
	info.item9 = reset_item;
	info.item10 = save_config_item;
	info.item11 = load_config_item;
	
	return 0;
}
INIT_APP_EXPORT(info_init);
