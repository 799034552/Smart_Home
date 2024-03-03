#ifndef __EEPROM_H_
#define __EEPROM_H_
#include "STC12C5A60S2.h"
#define uchar unsigned char
#define uint unsigned int
#define ENABLE_ISP 0x82
#define DEBUG_DATA               0x5A  
#define DATA_FLASH_START_ADDRESS 0x00  
union union_temp16;
uchar Byte_Read(uint add);              //读一字节，调用前需打开IAP 功能
void Byte_Program(uint add, uchar ch);  //字节编程，调用前需打开IAP 功能
void Sector_Erase(uint add);            //擦除扇区
void IAP_Disable();                       //关闭IAP 功能
void Delayms();
#endif