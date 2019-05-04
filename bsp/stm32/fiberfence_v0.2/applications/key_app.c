#include "key_app.h"
#include "lcd.h"
#include "string.h"
#include <rtthread.h>

/*****************************************************
* 定义各按键对应的执行函数
*****************************************************/

extern int current_display_id;
extern struct Display_Info info;

/* 显示一个条目内容 */
void lcdDisplayItem(struct Display_Item item)
{
	LcdCommandWrite(0x01);
	delay_ms(2);
	LcdCommandWrite(0x80+(16-strlen(item.label))/2);
	delay_ms(1);
	lcdStrWrite(item.label);
	delay_ms(1);
	LcdCommandWrite(0xC5);
	delay_ms(1);
	if(current_display_id<9)
		lcdDecimalWrite(item.param1);
	delay_ms(1);
}

/* 显示主窗口 */
void lcdDisplayMainwindow()
{
	LcdCommandWrite(0x01);
	delay_ms(2);
	LcdCommandWrite(0x85);
	delay_ms(1);
	lcdStrWrite(info.root_label1);
	delay_ms(1);
	LcdCommandWrite(0xC3);
	delay_ms(1);
	lcdStrWrite(info.root_label2);
	delay_ms(1);
}

/* 执行上翻按键功能 */
void key_up_press(void)
{
	switch(current_display_id)
	{
		case 0:
			current_display_id++;
			info.active = 0;
			info.item1.active = 1;
			lcdDisplayItem(info.item1);
			break;
		case 1:
			current_display_id++;
			info.item1.active = 0;
			info.item2.active = 1;
			lcdDisplayItem(info.item2);
			break;
		case 2:
			current_display_id++;
			info.item2.active = 0;
			info.item3.active = 1;
			lcdDisplayItem(info.item3);
			break;
		case 3:
			current_display_id++;
			info.item3.active = 0;
			info.item4.active = 1;
			lcdDisplayItem(info.item4);
			break;
		case 4:
			current_display_id++;
			info.item4.active = 0;
			info.item5.active = 1;
			lcdDisplayItem(info.item5);
			break;
		case 5:
			current_display_id++;
			info.item5.active = 0;
			info.item6.active = 1;
			lcdDisplayItem(info.item6);
			break;
		case 6:
			current_display_id++;
			info.item6.active = 0;
			info.item7.active = 1;
			lcdDisplayItem(info.item7);
			break;
		case 7:
			current_display_id++;
			info.item7.active = 0;
			info.item8.active = 1;
			lcdDisplayItem(info.item8);
			break;
		case 8:
			current_display_id++;
			info.item8.active = 0;
			info.item1.active = 1;
			lcdDisplayItem(info.item9);
			break;
		case 9:
			current_display_id++;
			info.item8.active = 0;
			info.item1.active = 1;
			lcdDisplayItem(info.item10);
			break;
		case 10:
			current_display_id++;
			info.item8.active = 0;
			info.item1.active = 1;
			lcdDisplayItem(info.item11);
			break;
		case 11:
			current_display_id=1;
			info.item11.active = 0;
			info.item1.active = 1;
			lcdDisplayItem(info.item1);
			break;
		default:
			break;
	}
}

/* 执行下翻按键功能 */
void key_down_press(void)
{
	switch(current_display_id)
	{
		case 0:
			current_display_id=1;
			info.active = 0;
			info.item1.active = 1;
			lcdDisplayItem(info.item1);
			break;
		case 1:
			current_display_id=11;
			info.item1.active = 0;
			info.item8.active = 1;
			lcdDisplayItem(info.item11);
			break;
		case 2:
			current_display_id--;
			info.item2.active = 0;
			info.item1.active = 1;
			lcdDisplayItem(info.item1);
			break;
		case 3:
			current_display_id--;
			info.item3.active = 0;
			info.item2.active = 1;
			lcdDisplayItem(info.item2);
			break;
		case 4:
			current_display_id--;
			info.item4.active = 0;
			info.item3.active = 1;
			lcdDisplayItem(info.item3);
			break;
		case 5:
			current_display_id--;
			info.item5.active = 0;
			info.item4.active = 1;
			lcdDisplayItem(info.item4);
			break;
		case 6:
			current_display_id--;
			info.item6.active = 0;
			info.item5.active = 1;
			lcdDisplayItem(info.item5);
			break;
		case 7:
			current_display_id--;
			info.item7.active = 0;
			info.item6.active = 1;
			lcdDisplayItem(info.item6);
			break;
		case 8:
			current_display_id--;
			info.item8.active = 0;
			info.item7.active = 1;
			lcdDisplayItem(info.item7);
			break;
		case 9:
			current_display_id--;
			info.item8.active = 0;
			info.item7.active = 1;
			lcdDisplayItem(info.item8);
			break;
		case 10:
			current_display_id--;
			info.item8.active = 0;
			info.item7.active = 1;
			lcdDisplayItem(info.item9);
			break;
		case 11:
			current_display_id--;
			info.item8.active = 0;
			info.item7.active = 1;
			lcdDisplayItem(info.item10);
			break;
		default:
			break;
	}
}

/* 向上调节当前显示参数 */
void key_plus_press(void)
{
	switch(current_display_id)
	{
		case 0:
			break;
		case 1:
			if(info.item1.param1<4095)
				info.item1.param1++;
			lcdDisplayItem(info.item1);
			break;
		case 2:
			if(info.item2.param1<4095)
				info.item2.param1++;
			lcdDisplayItem(info.item2);
			break;
		case 3:
			if(info.item3.param1<4095)
				info.item3.param1++;
			lcdDisplayItem(info.item3);
			break;
		case 4:
			if(info.item4.param1<100)
				info.item4.param1++;
			lcdDisplayItem(info.item4);
			break;
		default:
			break;
	}
}

/* 向下调节当前显示参数 */
void key_sub_press(void)
{
	switch(current_display_id)
	{
		case 0:
			break;
		case 1:
			if(info.item1.param1>1)
				info.item1.param1--;
			lcdDisplayItem(info.item1);
			break;
		case 2:
			if(info.item2.param1>1)
				info.item2.param1--;
			lcdDisplayItem(info.item2);
			break;
		case 3:
			if(info.item3.param1>1)
				info.item3.param1--;
			lcdDisplayItem(info.item3);
			break;
		case 4:
			if(info.item4.param1>1)
				info.item4.param1--;
			lcdDisplayItem(info.item4);
			break;
		default:
			break;
	}
}

extern void reset_config(void);
extern void save_config(void);
extern void load_config(void);

/* 确认当前显示操作 */
void key_enter_press(void)
{
	if(current_display_id==9){
		LcdCommandWrite(0x01);
		delay_ms(500);
		LcdCommandWrite(0x80+(16-strlen(info.item9.label))/2);
		delay_ms(1);
		lcdStrWrite(info.item9.label);
		delay_ms(1);
		reset_config();
	}
	else if(current_display_id==10){
		LcdCommandWrite(0x01);
		delay_ms(500);
		LcdCommandWrite(0x80+(16-strlen(info.item10.label))/2);
		delay_ms(1);
		lcdStrWrite(info.item10.label);
		delay_ms(1);
		save_config();
	}
	else if(current_display_id==11){
		LcdCommandWrite(0x01);
		delay_ms(500);
		LcdCommandWrite(0x80+(16-strlen(info.item11.label))/2);
		delay_ms(1);
		lcdStrWrite(info.item11.label);
		delay_ms(1);
		load_config();
	}
}

/* 返回显示主窗口 */
void key_back_perss(void)
{
	switch(current_display_id)
	{
		case 0:
			current_display_id=0;
			info.active = 1;
			lcdDisplayMainwindow();
			break;
		case 1:
			current_display_id=0;
			info.active = 1;
			info.item1.active = 0;
			lcdDisplayMainwindow();
			break;
		case 2:
			current_display_id=0;
			info.active = 1;
			info.item2.active = 0;
			lcdDisplayMainwindow();
			break;
		case 3:
			current_display_id=0;
			info.active = 1;
			info.item3.active = 0;
			lcdDisplayMainwindow();
			break;
		case 4:
			current_display_id=0;
			info.active = 1;
			info.item4.active = 0;
			lcdDisplayMainwindow();
			break;
		case 5:
			current_display_id=0;
			info.active = 1;
			info.item5.active = 0;
			lcdDisplayMainwindow();
			break;
		case 6:
			current_display_id=0;
			info.active = 1;
			info.item6.active = 0;
			lcdDisplayMainwindow();
			break;
		case 7:
			current_display_id=0;
			info.active = 1;
			info.item7.active = 0;
			lcdDisplayMainwindow();
			break;
		case 8:
			current_display_id=0;
			info.active = 1;
			info.item8.active = 0;
			lcdDisplayMainwindow();
			break;
		default:
			break;
	}
}


