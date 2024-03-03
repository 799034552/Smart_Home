#ifndef __ADC_H_
#define __ADC_H_
#define FOSC    18432000L
#define BAUD    9600
#define ADC_POWER   0x80            //ADC ��Դ����λ
#define ADC_FLAG    0x10            //ADC ת��������־λ
#define ADC_START   0x08            //ADC ��ʼת������λ
#define ADC_SPEEDLL 0x00            //420 ��ʱ������ת��һ��
#define ADC_SPEEDL  0x20            //280 ��ʱ������ת��һ��
#define ADC_SPEEDH  0x40            //140 ��ʱ������ת��һ��
#define ADC_SPEEDHH 0x60            //70 ��ʱ������ת��һ��

#define uint unsigned int
#define uchar unsigned char

void Delay2(uint n);
void InitADC();
uchar GetADCResult(uchar ch);
#endif