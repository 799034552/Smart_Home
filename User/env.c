#include "env.h"
#include "uart.h"
#include "math.h"
#include <stdio.h>
sbit wet = P2^2;
#define light_address 0x46                //光照传感器
#define	BMP085_SlaveAddress   0xee        //气压传感器
#define JUDGE  30                         //湿度传感器阈值
#define uchar unsigned char
#define uint unsigned int
/*****
光照传感器
******/
float temp;
/*****
气压传感器
******/
xdata short ac1,ac2,ac3,b1,b2,mb,mc,md;
xdata unsigned short ac4,ac5,ac6;
long  temperature,pressure;
/*****
湿度传感器
******/
uchar wet_data = 0;
xdata uchar   ge,shi,bai,qian,wan,shiwan;
xdata uchar res[] = "111111\r\n"; //结果

xdata char temp1[35] = {'\0'};

//------延迟函数------------
void delay(uint t)		//@11.0592MHz
{
	uint i,j,k;
	for(k=0;k<t;k++)
	for( i=0; i<10; i++)
	for( j=0; j<95; j++);
}
/*********************
      转换函数
**********************/
void conversion(long temp_data)  
{
    shiwan=temp_data/100000+0x30 ;
    temp_data=temp_data%100000;   //取余运算 
    wan=temp_data/10000+0x30 ;
    temp_data=temp_data%10000;   //取余运算
	qian=temp_data/1000+0x30 ;
    temp_data=temp_data%1000;    //取余运算
    bai=temp_data/100+0x30   ;
    temp_data=temp_data%100;     //取余运算
    shi=temp_data/10+0x30    ;
    temp_data=temp_data%10;      //取余运算
    ge=temp_data+0x30; 	
}
void light_write(uchar regis)
{
	start();
	write_byte(light_address);
	write_byte(regis);
	stop();
}
void light_start(void)
{
	light_write(0x01);
	light_write(0x10);
	delay(200);
}

float light_read(void)
{   
	uchar i;
	uchar BUF[8];
	int dis_data;
	start();                          //起始信号
	write_byte(light_address+1);         //发送设备地址+读信号
	for (i=0; i<3; i++)                      //连续读取2个地址数据，存储中BUF
	{
		BUF[i] = read_byte();          //BUF[0]存储0x32地址中的数据
		if (i == 3)
		{
			 send_respons(1);                //最后一个数据需要回NOACK
		}
		else
		{		
			send_respons(0);                //回应ACK
	 }
	}
	stop();                          //停止信号
	delay(5);
	dis_data=BUF[0];
  dis_data=(dis_data<<8)+BUF[1];//合成数据，即光照数据
	temp = (float)dis_data/1.2;
	return temp;
}
uchar* getLightStr(void)
{
	conversion(temp);         //计算数据和显示
	res[5] = ge;
	res[4] = shi;
	res[3] = bai;
	res[2] = qian;
	res[1] = wan;
	res[0] = shiwan;
  return res;
}
/*****************
  气压传感器
******************/
//*********************************************************
//读出BMP085内部数据,连续两个
//*********************************************************
short press_Multiple_read(uchar ST_Address)
{
	uchar msb, lsb;
	short _data;
	start();                          //起始信号
	write_byte(BMP085_SlaveAddress);    //发送设备地址+写信号
	write_byte(ST_Address);             //发送存储单元地址
	start();                          //起始信号
	write_byte(BMP085_SlaveAddress+1);         //发送设备地址+读信号

	msb = read_byte();                 //BUF[0]存储
	send_respons(0);                       //回应ACK
	lsb = read_byte();     
	send_respons(1);                       //最后一个数据需要回NOACK

	stop();                           //停止信号
	delay(5);
	_data = msb << 8;
	_data |= lsb;	
	return _data;
}
long bmp085ReadTemp(void)
{

    start();                  //起始信号
    write_byte(BMP085_SlaveAddress);   //发送设备地址+写信号
    write_byte(0xF4);	          // write register address
    write_byte(0x2E);       	// write register data for temp
    stop();                   //发送停止信号
	  delay(10);	// max time is 4.5ms
	return (long) press_Multiple_read(0xF6);
}
long bmp085ReadPressure(void)
{
	long pressure = 0;
	start();                   //起始信号
	write_byte(BMP085_SlaveAddress);   //发送设备地址+写信号
	write_byte(0xF4);	          // write register address
	write_byte(0x34);       	  // write register data for pressure
	stop();                    //发送停止信号
	delay(10);    	                  // max time is 4.5ms
	pressure = press_Multiple_read(0xF6);
	pressure &= 0x0000FFFF;
	return pressure;	
}
void press_init()
{
	ac1 = press_Multiple_read(0xAA);
	ac2 = press_Multiple_read(0xAC);
	ac3 = press_Multiple_read(0xAE);
	ac4 = press_Multiple_read(0xB0);
	ac5 = press_Multiple_read(0xB2);
	ac6 = press_Multiple_read(0xB4);
	b1 =  press_Multiple_read(0xB6);
	b2 =  press_Multiple_read(0xB8);
	mb =  press_Multiple_read(0xBA);
	mc =  press_Multiple_read(0xBC);
	md =  press_Multiple_read(0xBE);
}
long get_temperature()
{
	long ut;
	long x1, x2, b5;
	ut = bmp085ReadTemp();	   // 读取温度
	x1 = ((long)ut - ac6) * ac5 >> 15;
	x2 = ((long) mc << 11) / (x1 + md);
	b5 = x1 + x2;
	temperature = (b5 + 8) >> 4;
	return temperature;
}
uchar* getTempStr(void)
{
	conversion(temperature);        
	res[5] = ge;
	res[4] = shi;
	res[3] = bai;
	res[2] = qian;
	res[1] = wan;
	res[0] = shiwan;
  return res;
}
long get_pressure()
{
	long up;
	long x1, x2, b5, b6, x3, b3, p;
	unsigned long b4, b7;
	up = bmp085ReadPressure();  // 读取压强
	b6 = b5 - 4000;
	x1 = (b2 * (b6 * b6 >> 12)) >> 11;
	x2 = ac2 * b6 >> 11;
	x3 = x1 + x2;
	b3 = (((long)ac1 * 4 + x3) + 2)/4;
	x1 = ac3 * b6 >> 13;
	x2 = (b1 * (b6 * b6 >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = (ac4 * (unsigned long) (x3 + 32768)) >> 15;
	b7 = ((unsigned long) up - b3) * (50000 >> 0);
	if( b7 < 0x80000000)
		p = (b7 * 2) / b4 ;
	else  
		p = (b7 / b4) * 2;
	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	pressure = p + ((x1 + x2 + 3791) >> 4);
	return pressure;
}
uchar* getPressureStr(void)
{
	conversion(pressure);        
	res[5] = ge;
	res[4] = shi;
	res[3] = bai;
	res[2] = qian;
	res[1] = wan;
	res[0] = shiwan;
  return res;
}
/*****************
  湿度传感器
******************/
void Delay30us()
{
	unsigned char i;

	i = 80;
	while (--i);
}
void Delay20ms()
{
	unsigned char i, j, k;

	i = 1;
	j = 216;
	k = 35;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
uchar GetData (void)
{
	uchar i,j;                                //for循环变量
	uchar t;                                //超时判断
	uchar dat8=0;                        //一次读取的8位数据，需要读5次
	wet = 0;                                //主机发起始信号
	Delay20ms();                        //主机拉低总线至少18ms
	wet = 1;                                //主机拉高总线20~40us
	Delay30us();
	t = 80;                                        //设置超时等待时间
	while(wet && t--);                //等待DHT11拉低总线
	if(t == 0)                                //超时
	{
		wet = 1;
		return 0;                        //通信错误退出，返回错误信号：-1
	}
	//等80us响应信号
	t = 250;                                //设置超时等待时间
	while(!wet && t--);        //等待DHT11拉高总线
	if(t == 0)                                //超时
	{
		wet = 1;
		return 0;                        //通信错误退出，返回错误信号：-2
	}
	//等80us响应信号
	t = 250;                                //设置超时等待时间
	while(wet && t--);                //等待DHT11拉低总线
	if(t == 0)                                //超时
	{
		wet = 1;
		return 0;                        //通信错误退出，返回错误信号：-3
	}

	for(j=0; j<5; j++)                //5次读取
	{
		for(i=0; i<8; i++)                        //1次8个位
		{
			//等待50us开始时隙
			t = 150;                                //设置超时等待时间
			while(!wet && t--);        //等待DHT11拉高总线
			if(t == 0)                                //超时
			{
				wet = 1;
				return 0;                        //通信错误退出，返回错误信号：-4
			}
			t = 0;                                        //记录时间清零
			while(wet && ++t);                //等待并记录高电平持续时间
			dat8 <<= 1;
			if(t > JUDGE)                        //高电平持续时间较长(70us)
				dat8 += 1;                        //传输值为1
		}
		if(j == 0)
			wet_data = dat8;
	}
	Delay30us();                //等待DHT11拉低50us
	Delay30us();
	wet = 1;                        //结束，拉高总线
	return wet_data;                        //返回成功信号
}
uchar* getWetStr(void)
{
	conversion(wet_data);        
	res[5] = ge;
	res[4] = shi;
	res[3] = bai;
	res[2] = qian;
	res[1] = wan;
	res[0] = shiwan;
  return res;
}
/*****************
  显示温度
******************/
void showTemperature()
{
	get_temperature();
	getTempStr();
	uart2Clear();
	uart2AddChar("SBC(1);DCV24(245,50,'");
	uart2AddCharLen(res+3,1);
	uart2AddCharLen(res+4,1);
	uart2AddChar(".");
	uart2AddCharLen(res+5,1);
	uart2AddChar("℃',16);");
	uart2SendEnd();
}
void showPress()
{
	get_pressure();
	getPressureStr();
	uart2Clear();
	uart2AddChar("SBC(49);DCV16(170,164,'");
	res[4] = res[3];
	res[3] = '.';
	res[5] = '\0';
	uart2AddChar(res);
	uart2AddChar("KPa',16);");
	uart2SendEnd();
}
void showLight()
{
	uchar i;
	light_start();
	light_read();
	getLightStr();
	uart2Clear();
	uart2AddChar("SBC(13);DCV16(103,200,'");
	while(res[i++] == '0');
	i--;
	res[6] = '\0';
	uart2AddChar(res+i);
	uart2AddChar("Lux ',16);");
	uart2SendEnd();
}
void showWet()
{
		if(GetData())
		{
			getWetStr();
			uart2Clear();
			uart2AddChar("SBC(14);DCV24(18,200,'");
			res[6] = '\0';
			uart2AddChar(res+4);
			uart2AddChar("%',16);");
			uart2SendEnd();
		}
}
char * getAllEnv()
{
	getTempStr();
	sprintf(temp1,"%s;",res);
	getPressureStr();
	sprintf(temp1,"%s%s;",temp1,res);
	getWetStr();
	sprintf(temp1,"%s%s;",temp1,res);
	getLightStr();
	sprintf(temp1,"%s%s;",temp1,res);
	return temp1;
}

