#ifndef __IIC_H_
#define __IIC_H_
#include "STC12C5A60S2.h"
#define uchar unsigned char
void start(); //��ʼ�ź�
void stop(); //ֹͣ
void rec_respons(); //Ӧ��
void send_respons(bit ack); //��Ӧ��
void init(); //��ʼ��
void write_byte(uchar date); //д���ֽ�
uchar read_byte(); //��ȡ���ֽ�

#endif