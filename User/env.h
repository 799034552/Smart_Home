#ifndef __ENV_H_
#define __ENV_H_
#include "STC12C5A60S2.h"
#include "iic.h"
void light_start(void);
float light_read(void);
uchar* getLightStr(void);
void press_init();
void bmp085Convert();
long get_temperature();
uchar* getTempStr(void);
long get_pressure();
uchar* getPressureStr(void);
uchar GetData (void);
uchar* getWetStr(void);
void showTemperature();
void showPress();
void showLight();
void showWet();
#endif