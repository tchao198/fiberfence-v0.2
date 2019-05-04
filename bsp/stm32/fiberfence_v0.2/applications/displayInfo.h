#ifndef __DISPLAYINFO_H
#define   __DISPLAYINFO_H

//Ä¬ÈÏ²ÎÊý
#define ALARM_THRESHOLD_A       2200
#define ALARM_THRESHOLD_B       2200
#define OPTIC_POWER_THRESHOLD   180
#define ALARM_INTERVAL          5

struct Display_Item
{
	int active;
	char *label;
	int param1;
	int param2;
	int param3;
};

struct Display_Info
{
	int active;
	char *root_label1, *root_label2;
	struct Display_Item item1;
	struct Display_Item item2;
	struct Display_Item item3;
	struct Display_Item item4;
	struct Display_Item item5;
	struct Display_Item item6;
	struct Display_Item item7;
	struct Display_Item item8;
	struct Display_Item item9;
	struct Display_Item item10;
	struct Display_Item item11;
};

extern struct Display_Info info;

int info_init(void);

#endif
