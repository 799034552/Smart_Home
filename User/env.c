#include "env.h"
#include "uart.h"
#include "math.h"
#include <stdio.h>
sbit wet = P2^2;
#define light_address 0x46                //���մ�����
#define	BMP085_SlaveAddress   0xee        //��ѹ������
#define JUDGE  30                         //ʪ�ȴ�������ֵ
#define uchar unsigned char
#define uint unsigned int
/*****
���մ�����
******/
float temp;
/*****
��ѹ������
******/
xdata short ac1,ac2,ac3,b1,b2,mb,mc,md;
xdata unsigned short ac4,ac5,ac6;
long  temperature,pressure;
/*****
ʪ�ȴ�����
******/
uchar wet_data = 0;
xdata uchar   ge,shi,bai,qian,wan,shiwan;
xdata uchar res[] = "111111\r\n"; //���

xdata char temp1[35] = {'\0'};

//------�ӳٺ���------------
void delay(uint t)		//@11.0592MHz
{
	uint i,j,k;
	for(k=0;k<t;k++)
	for( i=0; i<10; i++)
	for( j=0; j<95; j++);
}
/*********************
      ת������
**********************/
void conversion(long temp_data)  
{
    shiwan=temp_data/100000+0x30 ;
    temp_data=temp_data%100000;   //ȡ������ 
    wan=temp_data/10000+0x30 ;
    temp_data=temp_data%10000;   //ȡ������
	qian=temp_data/1000+0x30 ;
    temp_data=temp_data%1000;    //ȡ������
    bai=temp_data/100+0x30   ;
    temp_data=temp_data%100;     //ȡ������
    shi=temp_data/10+0x30    ;
    temp_data=temp_data%10;      //ȡ������
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
	start();                          //��ʼ�ź�
	write_byte(light_address+1);         //�����豸��ַ+���ź�
	for (i=0; i<3; i++)                      //������ȡ2����ַ���ݣ��洢��BUF
	{
		BUF[i] = read_byte();          //BUF[0]�洢0x32��ַ�е�����
		if (i == 3)
		{
			 send_respons(1);                //���һ��������Ҫ��NOACK
		}
		else
		{		
			send_respons(0);                //��ӦACK
	 }
	}
	stop();                          //ֹͣ�ź�
	delay(5);
	dis_data=BUF[0];
  dis_data=(dis_data<<8)+BUF[1];//�ϳ����ݣ�����������
	temp = (float)dis_data/1.2;
	return temp;
}
uchar* getLightStr(void)
{
	conversion(temp);         //�������ݺ���ʾ
	res[5] = ge;
	res[4] = shi;
	res[3] = bai;
	res[2] = qian;
	res[1] = wan;
	res[0] = shiwan;
  return res;
}
/*****************
  ��ѹ������
******************/
//*********************************************************
//����BMP085�ڲ�����,��������
//*********************************************************
short press_Multiple_read(uchar ST_Address)
{
	uchar msb, lsb;
	short _data;
	start();                          //��ʼ�ź�
	write_byte(BMP085_SlaveAddress);    //�����豸��ַ+д�ź�
	write_byte(ST_Address);             //���ʹ洢��Ԫ��ַ
	start();                          //��ʼ�ź�
	write_byte(BMP085_SlaveAddress+1);         //�����豸��ַ+���ź�

	msb = read_byte();                 //BUF[0]�洢
	send_respons(0);                       //��ӦACK
	lsb = read_byte();     
	send_respons(1);                       //���һ��������Ҫ��NOACK

	stop();                           //ֹͣ�ź�
	delay(5);
	_data = msb << 8;
	_data |= lsb;	
	return _data;
}
long bmp085ReadTemp(void)
{

    start();                  //��ʼ�ź�
    write_byte(BMP085_SlaveAddress);   //�����豸��ַ+д�ź�
    write_byte(0xF4);	          // write register address
    write_byte(0x2E);       	// write register data for temp
    stop();                   //����ֹͣ�ź�
	  delay(10);	// max time is 4.5ms
	return (long) press_Multiple_read(0xF6);
}
long bmp085ReadPressure(void)
{
	long pressure = 0;
	start();                   //��ʼ�ź�
	write_byte(BMP085_SlaveAddress);   //�����豸��ַ+д�ź�
	write_byte(0xF4);	          // write register address
	write_byte(0x34);       	  // write register data for pressure
	stop();                    //����ֹͣ�ź�
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
	ut = bmp085ReadTemp();	   // ��ȡ�¶�
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
	up = bmp085ReadPressure();  // ��ȡѹǿ
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
  ʪ�ȴ�����
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
	uchar i,j;                                //forѭ������
	uchar t;                                //��ʱ�ж�
	uchar dat8=0;                        //һ�ζ�ȡ��8λ���ݣ���Ҫ��5��
	wet = 0;                                //��������ʼ�ź�
	Delay20ms();                        //����������������18ms
	wet = 1;                                //������������20~40us
	Delay30us();
	t = 80;                                        //���ó�ʱ�ȴ�ʱ��
	while(wet && t--);                //�ȴ�DHT11��������
	if(t == 0)                                //��ʱ
	{
		wet = 1;
		return 0;                        //ͨ�Ŵ����˳������ش����źţ�-1
	}
	//��80us��Ӧ�ź�
	t = 250;                                //���ó�ʱ�ȴ�ʱ��
	while(!wet && t--);        //�ȴ�DHT11��������
	if(t == 0)                                //��ʱ
	{
		wet = 1;
		return 0;                        //ͨ�Ŵ����˳������ش����źţ�-2
	}
	//��80us��Ӧ�ź�
	t = 250;                                //���ó�ʱ�ȴ�ʱ��
	while(wet && t--);                //�ȴ�DHT11��������
	if(t == 0)                                //��ʱ
	{
		wet = 1;
		return 0;                        //ͨ�Ŵ����˳������ش����źţ�-3
	}

	for(j=0; j<5; j++)                //5�ζ�ȡ
	{
		for(i=0; i<8; i++)                        //1��8��λ
		{
			//�ȴ�50us��ʼʱ϶
			t = 150;                                //���ó�ʱ�ȴ�ʱ��
			while(!wet && t--);        //�ȴ�DHT11��������
			if(t == 0)                                //��ʱ
			{
				wet = 1;
				return 0;                        //ͨ�Ŵ����˳������ش����źţ�-4
			}
			t = 0;                                        //��¼ʱ������
			while(wet && ++t);                //�ȴ�����¼�ߵ�ƽ����ʱ��
			dat8 <<= 1;
			if(t > JUDGE)                        //�ߵ�ƽ����ʱ��ϳ�(70us)
				dat8 += 1;                        //����ֵΪ1
		}
		if(j == 0)
			wet_data = dat8;
	}
	Delay30us();                //�ȴ�DHT11����50us
	Delay30us();
	wet = 1;                        //��������������
	return wet_data;                        //���سɹ��ź�
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
  ��ʾ�¶�
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
	uart2AddChar("��',16);");
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

