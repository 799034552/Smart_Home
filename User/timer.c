#include "timer.h"
#include "uart.h"
#include "music.h"
#include "ADC.h"
#include<stdlib.h>
#define uint unsigned int
#define uchar unsigned char
#define one_day_second  86400
sbit led1 = P1^0;
xdata long cnt = 0;
xdata long nowTime = 16012190; //16012190  15292894
xdata uchar month_day[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
xdata uint year = 2021;
xdata uchar month = 5,day = 24,hour = 12,minute = 0,second = 0, weekDay = 0;
code char weekDays[][7] = {"星期一","星期二","星期\xc8\xfd","星期四","星期五","星期六","星期天"};
bit time_update_flag = 0;
bit sensor_flag = 0;
char door_flag = -1;
char screen_state = 0;
bit AD_flag = 0;
bit sensor_send_flag=0;
uint count_down_time= 0;
xdata char mempool[200];
xdata char timetable[5][5][18];
code uchar class_hour[] = {7, 9, 14, 16,18};
code uchar class_minute[] = {50, 50, 20, 00,50};
/*************************************************
                    闹钟结构体
*************************************************/
struct CLOCK{
	struct CLOCK *pNext;
	uchar state; //0x01 isOpen 0x06 normal
	uchar hour,minute, second;
	uint id;
} *clock_head, *clock_last;
/*************************************************
                    定时器0初始化
*************************************************/
void time0_init(void)
{
	uchar i,j;
	TMOD |= 0x01;
	TH0 = (65536 - 9174) / 256;
	TL0 = (65536 - 9174) % 256;
	EA = 1;
	ET0 = 1;
	TR0 = 1;
	EA=1;
	init_mempool (&mempool, sizeof(mempool));
	clock_head = (struct  CLOCK*)malloc(sizeof(struct CLOCK));
	clock_head->id = 0;
	clock_last = clock_head;
	clock_last->pNext = NULL;
	addClock(7,50,20,0x00);
//	addClock(13,15,0,0x00);
//	addClock(14,15,0,0x00);
//	deleteClock(2);
	for(i=0;i < 5;i++)
	{
		for(j=0; j < 5; j++)
			timetable[i][j][0] = '\0';
	}
	parseClass("0;0;运控|博学楼A06");
	changeToNowTime(nowTime);
}
/*************************************************
                    定时器0中断
*************************************************/
void T0_time(void) interrupt 1
{
	TH0 = (65536 - 9174) / 256;
	TL0 = (65536 - 9174) % 256;
	cnt++;
	if(cnt % 100 == 0) //1s
	{
		led1 = !led1;
		nowTime++;
		time_update_flag = 1;
		if(count_down_time > 0)
			count_down_time--;
        if (door_flag > 0)
            door_flag --;
		if(count_down_time == 1)
		{
			clock_stop();
			//changeToNowTime(nowTime);
			screen_state = 2;
		}
	}
	if(cnt % 300 == 0)
	{
		AD_flag = 1;
	}
	if(cnt % 500 == 0)
	{
		sensor_flag = 1;
	}
	if(cnt % 1000 == 0)
		sensor_send_flag = 1;
	if (cnt >= 1000000)
		cnt = 0;
}
/*************************************************
                    判断是否闰年
*************************************************/
uchar isRun(uint y)
{
	if((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0))
		return 1;
	else
		return 0;
}
/*************************************************
                    数字转时间
*************************************************/
void changeToNowTime(long n)
{
	uchar i;
	long t,u;
	t = n / 86400 +  1;
	u = n % 86400;
	weekDay = (t-4) % 7;
	year = 2021;
	while(t - 365 - isRun(year) > 0)
	{
		year++;
		t -=(365 + isRun(year));
	}
	if(isRun(year))
		month_day[1] = 29;
	else
		month_day[1] = 28;
	month = 1;
	for(i=0; i < 12; i++)
	{
		if((t - month_day[i]) > 0)
		{
			t -=month_day[i];
			month++;
		}
		else
			break;
	}
	day = t;
	hour = u / 3600;
	u %= 3600;
	minute = u / 60;
	u %= 60;
	second = u %60;
}
/*************************************************
                    时间转数字
*************************************************/
long timeToNum(uint tyear,uchar tmonth,uchar tday,uchar thour,uchar tminute,uchar tsecond)
{
	long t = 0;
	uchar i;
	if (isRun(tyear))
		month_day[1] = 29;
	else
		month_day[1] = 28;
	while(tyear - 2021 > 0)
	{
		t += (365 + isRun(tyear))*86400;
		tyear++;
	}
	for(i = 0; i < tmonth - 1; i++)
		t += month_day[i]*86400;
	t+=(tday - 1) * 86400;
	t+= thour*3600;
	t+= tminute*60;
	t+=tsecond;
	return t;
}
/*************************************************
                    时间更新事件
*************************************************/
void time_update()
{
	if(second<59)
	{
		second++;
	} else if (minute < 59)
	{
		second = 0;
		minute++;
	} else 
		changeToNowTime(nowTime);
	checkClock();
}
/*************************************************
                    增加闹钟
*************************************************/
void addClock(uchar hour,uchar minute,uchar second, uchar sta)
{
	struct CLOCK * t;
	t = (struct  CLOCK*)malloc(sizeof(struct CLOCK));
	clock_last->pNext = t;
	t->hour = hour;
	t->minute = minute;
	t->second = second;
	t->state = sta;
	t->pNext = NULL;
	t->id = clock_last->id + 1;
	clock_last = t;
}
void deleteClock(uint id)
{
	struct CLOCK * t;
	struct CLOCK * tt;
	t = clock_head;
	while(t->pNext != NULL)
	{
		if(t->pNext->id == id)
		{
			tt = t->pNext;
			if(tt == clock_last)
				clock_last = t;
			t->pNext = tt->pNext;
			free(tt);
			break;
		}
		t = t->pNext;
	}
}
/*************************************************
                    遍历闹钟
*************************************************/
void showClock()
{
	struct CLOCK * t;
	t = clock_head;
	while(t->pNext != NULL)
	{
		t = t->pNext;
		uartSend_number(t->hour);
		uartSend(":");
		uartSend_number(t->minute);
		uartSend(":");
		uartSend_number(t->second);
		uartSend("  ");
		uartSend_number(t->id);
		uartSend("\r\n");
	}
}
/*************************************************
                    串口屏显示闹钟
*************************************************/
void ShowClocks() 
{
	struct CLOCK * t;
	t = clock_head;
	uart2Clear();
	uart2AddChar("CLR(51);DIR(3);");
	while(t->pNext != NULL)
	{
		t = t->pNext;
		switch(t->id)
		{
			case 1 :uart2AddChar("SBC(51);DCV24(100,55,'"); break;
			case 2 :uart2AddChar("SBC(51);DCV24(100,80,'"); break;
			case 3 :uart2AddChar("SBC(51);DCV24(100,105,'"); break;
			case 4 :uart2AddChar("SBC(51);DCV24(100,130,'"); break;
			case 5 :uart2AddChar("SBC(51);DCV24(100,155,'"); break;
		}
		uart2AddChar(num_to_Str(t->hour,2));
		uart2AddChar(":");
		uart2AddChar(num_to_Str(t->minute,2));
		uart2AddChar(":");
		uart2AddChar(num_to_Str(t->second,2));
		uart2AddChar("',16);");
	}
	uart2SendEnd();
	checkBusy();
}
/*************************************************
                    修改闹钟
*************************************************/
//void Show_onebit(uchar num, uchar bflag, uchar time, uchar flag1, uchar flag2)
//{
//	if((num == flag1) && (bflag == flag2)) //选中标记
//	{
//		uart2AddChar("SBC(51);DCV24(");
//		uart2AddChar(num_to_Str(xaxis[bflag],-1));
//		uart2AddChar(",");
//		uart2AddChar(num_to_Str(yaxis[num],-1));
//		uart2AddChar(",'");
//		uart2AddChar(num_to_Str(time,2));
//		if(bflag > 0)
//		{
//			uart2AddChar(":");
//		}
//		uart2AddChar("',30);");
//	}
//	else
//	{
//		uart2AddChar("SBC(51);DCV24(");
//		uart2AddChar(num_to_Str(xaxis[bflag],-1));
//		uart2AddChar(",");
//		uart2AddChar(num_to_Str(yaxis[num],-1));
//		uart2AddChar(",'");
//		uart2AddChar(num_to_Str(time,2));
//		if(bflag > 0)
//		{
//			uart2AddChar(":");
//		}
//		uart2AddChar("',16);");
//	}
//}

//void Show_oneclock(uchar num, uchar hour, uchar minute, uchar second, uchar flag1,uchar flag2)
//{
//	uart2Clear();
//	Show_onebit(num,2,hour,flag1,flag2);
//	Show_onebit(num,1,minute,flag1,flag2);
//	Show_onebit(num,0,second,flag1,flag2);
//	uart2SendEnd();
//	checkBusy();
//}
/*************************************************
            检查是否有闹钟时事件发生
*************************************************/
void checkClock()
{
	uchar i ;
	struct CLOCK * t;
	t = clock_head;
	while(t->pNext != NULL)
	{
		t = t->pNext;
		if((t->state & 0x01))
			continue;
		if(hour == t->hour && minute == t->minute && second == t->second)
		{
			//如果是正常的闹钟
			if((t->state & 0x06) == 0x00)
			{
				// 有时钟发生
				clock_start();
				count_down_time = 15;
				uart2Clear();uart2AddChar("FSIMG(2711552,0,0,320,240,0)");uart2SendEnd();
				screen_state = 1;
			}
			//工作日闹钟
			else if ((t->state & 0x06) == 0x02 && weekDay<5)
			{
				count_down_time = 3;
			}
		}
	}
	for(i = 0; i < 5;i++)
	{
		if(hour == class_hour[i] && minute == class_minute[i] && second == 0 && weekDay < 5 && timetable[weekDay][i][0] !='\0')
		{
			class_start();
			count_down_time = 15;
			uart2Clear();uart2AddChar("FSIMG(2404352,0,0,320,240,0)");uart2SendEnd();
			screen_state = 1;
			//play_one(2);
		}
	}
}
/*************************************************
                 解析课程表
*************************************************/
void parseClass(char * s)
{
	char t = -1,u = -1;
	uchar w,i,j;
	i = 0;
	while(s[i] != '\0')
	{
		if(s[i] == ';')
		{
			if(t == -1)
				t = w - '0';
			else if (u == -1)
			{
				i++;
				u = w - '0';
				j = 0;
				while(s[i] != ';' && s[i] != '\0')
					timetable[t][u][j++] = s[i++];
				timetable[t][u][j++] = '\0';
				u = -1;
				t = -1;
			}
		}
		w = s[i++];
	}
}
/*************************************************
                    设置时间
*************************************************/
void setTime(long t)
{
	nowTime = t;
	changeToNowTime(nowTime);
}
/*************************************************
                 显示时间
*************************************************/
void showTime()
{
	uart2Clear();
	uart2AddChar("SBC(61);DCV24(80,25,'");
	uart2AddChar(num_to_Str(hour,2));
	uart2AddChar(":");
	uart2AddChar(num_to_Str(minute,2));
	uart2AddChar(":");
	uart2AddChar(num_to_Str(second,2));
	uart2AddChar("',16);");
	uart2AddChar("DCV24(80,50,'");
	uart2AddChar(num_to_Str(month,-1));
	uart2AddChar("月");
	uart2AddChar(num_to_Str(day,-1));
	uart2AddChar("日',16);");
	uart2AddChar("DCV24(85,75,'");
	uart2AddChar(weekDays[weekDay]);
	uart2AddChar("',16);");
	uart2SendEnd();
}
