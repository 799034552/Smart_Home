#ifndef __ADC_H_
#define __ADC_H_
#define FOSC    18432000L
#define BAUD    9600
#define ADC_POWER   0x80            //ADC 电源控制位
#define ADC_FLAG    0x10            //ADC 转换结束标志位
#define ADC_START   0x08            //ADC 开始转换控制位
#define ADC_SPEEDLL 0x00            //420 个时钟周期转换一次
#define ADC_SPEEDL  0x20            //280 个时钟周期转换一次
#define ADC_SPEEDH  0x40            //140 个时钟周期转换一次
#define ADC_SPEEDHH 0x60            //70 个时钟周期转换一次

#define uint unsigned int
#define uchar unsigned char

void Delay2(uint n);
void InitADC();
uchar GetADCResult(uchar ch);
#endif