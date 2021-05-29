#include "iic.h"
#include  <INTRINS.H>
#define uint unsigned int
#define uchar unsigned char
sbit sda = P2^0; 
sbit scl = P2^1;
void delay()		//@11.0592MHz
{
	unsigned char i;
	_nop_();
	_nop_();
	_nop_();
	i = 10;
	while (--i);
}

void start() //开始信号
{
	sda=1;
	scl=1;
	delay();
	sda=0;
	delay();
	scl = 0;
}
void stop() //停止 
{
	sda=0;
	scl=1;
	delay();
	sda=1;
	delay();
}
void rec_respons() //接收应答 
{
//	uint i;
	scl=1;
	delay();
//	while((sda==1)&&(i<250))i++;
	scl=0;
	delay();
}
void send_respons(bit ack) //发送应答 
{
	
	sda = ack;
	scl=1;
	delay();
	scl=0;
	delay();
}
void init() {
	sda=1;
	delay();
	scl=1;
	delay();
}
void write_byte(uchar date) {
	uchar i,temp;
	temp=date;
	for(i=0;i<8;i++) {
		temp=temp<<1;
		scl=0;
		delay();
		sda=CY;
		delay();
		scl=1;
		delay();
	}
	scl=0;
	delay();
	sda=1;
	delay();
	rec_respons();
}
uchar read_byte() {
	uchar i,k;
	scl=0;
	delay();
	sda=1;
	delay();
	for(i=0;i<8;i++) {
		scl=1;
		delay();
		k=(k<<1)|sda;
		scl=0;
		delay();
	}
	return k;
}
