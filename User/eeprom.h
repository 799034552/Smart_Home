#ifndef __EEPROM_H_
#define __EEPROM_H_
#include "STC12C5A60S2.h"
#define uchar unsigned char
#define uint unsigned int
#define ENABLE_ISP 0x82
#define DEBUG_DATA               0x5A  
#define DATA_FLASH_START_ADDRESS 0x00  
union union_temp16;
uchar Byte_Read(uint add);              //��һ�ֽڣ�����ǰ���IAP ����
void Byte_Program(uint add, uchar ch);  //�ֽڱ�̣�����ǰ���IAP ����
void Sector_Erase(uint add);            //��������
void IAP_Disable();                       //�ر�IAP ����
void Delayms();
#endif