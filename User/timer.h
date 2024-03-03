#ifndef __TIMER_H_
#define __TIMER_H_
#include "STC12C5A60S2.h"
extern bit time_update_flag;
extern bit AD_flag;
extern bit sensor_flag;
extern bit sensor_send_flag;
extern char door_flag;
extern char screen_state;
extern xdata long nowTime;
extern unsigned int count_down_time;
void time0_init(void);
void time_update();
void addClock(unsigned char hour,unsigned char minute,unsigned char second, unsigned char sta);
void showClock();
void deleteClock(unsigned int id);
void checkClock();
void parseClass(char * s);
void changeToNowTime(long n);
void showTime();
void setTime(long t);
void ShowClocks();
//void Show_onebit(unsigned char num, unsigned char bflag,unsigned char time, unsigned char flag1,unsigned char flag2);
//void Show_oneclock(unsigned char num, unsigned char hour, unsigned char minute, unsigned char second, unsigned char flag1,unsigned char flag2);
#endif