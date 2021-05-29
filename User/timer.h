#ifndef __TIMER_H_
#define __TIMER_H_
#include "STC12C5A60S2.h"
extern bit time_update_flag;
extern bit sensor_flag;
void time0_init(void);
void time_update();
void addClock(unsigned char hour,unsigned char minute,unsigned char second, unsigned char sta);
void showClock();
void deleteClock(unsigned int id);
void checkClock();
void parseClass(char * s);
void changeToNowTime(long n);
void showTime();
#endif