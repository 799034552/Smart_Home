#ifndef __CLOCK_H_
#define __CLOCK_H_
#include "STC12C5A60S2.h"
#define uint unsigned int
#define uchar unsigned char
extern bit uiflag;
extern uint address1;
extern char num1;
void setclock();
void transfer();
void clock_init();
void updateclock();
#endif