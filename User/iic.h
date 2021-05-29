#ifndef __IIC_H_
#define __IIC_H_
#include "STC12C5A60S2.h"
#define uchar unsigned char
void start(); //开始信号
void stop(); //停止
void rec_respons(); //应答
void send_respons(bit ack); //不应答
void init(); //初始化
void write_byte(uchar date); //写单字节
uchar read_byte(); //读取单字节

#endif