#include "uart.h"
#include <stdio.h>
//串口数据
unsigned char i2 = 0;
xdata char uart2SendData[100];
xdata char strRes[8] = "10\011111";
//串口2初始化
void uart2Init(void)
{
	S2CON = 0x50;				      //方式1，八位数据，可变波特率
	AUXR1 = 0x00;	 				 //1T工作方式
 	PCON  = 0x00;					 //不倍增波特率
 	IP2   = 0x00;					 //优先级默认
 	BRT   = 0XFD; 					 //设置波特率9600
 	AUXR  = 0x10;					 //启动波特率发生器
 	EA    = 1; 					 //开总中断
	IP   = 0x00;				 //优先级默认
	IE2   = 0x01;					 //开串口2中断
}
unsigned char ES2(char b) {if(b==-1) return IE2&0x01;b?(IE2|=0x01):(IE2&=~0x01);return 1;}
unsigned char S2RI(char b) {if(b==-1) return S2CON&0x01;b?(S2CON|=0x01):(S2CON&=~0x01);return 1;}
unsigned char S2TI(char b) {if(b==-1) return S2CON&0x02;b?(S2CON|=0x02):(S2CON&=~0x02);return 1;}
void uart2AddChar(char* s)
{
	uchar i = 0;
	uchar len = 0;
	while(s[len++] != '\0');
	for(i = 0; i< len-1; i++)
	{
		uart2SendData[i2++] = s[i];
	}
}
void uart2AddCharLen(char* s, uchar len)
{
	uchar i = 0;
	for(i = 0; i< len; i++)
	{
		uart2SendData[i2++] = s[i];
	}
}
void uart2Clear() {i2=0;}

void uart2Send()
{
	unsigned char i = 0;
	ES2(0);
	for(i = 0; i < i2; i++)
	{
		S2BUF = uart2SendData[i];
		while(!S2TI(-1));
		S2TI(0);
	}
	ES2(1);
	uart2Clear();
}
void uart2SendEnd()
{
	uart2AddChar("\r\n");
	uart2Send();
}
void uart2SendChar(char*s)
{
	uart2Clear();
	uart2AddChar(s);
	uart2SendEnd();
}
void uartInit(void)
{
	TMOD|=0x20;
	TH1=0xfd;
	TL1=0xfd;
	TR1=1;
	REN=1;
	SM0=0;
	SM1=1;
	EA=1;
	ES=1;
}
void uartSend(char*s)
{
	uchar i;
	uchar len = 0;
	while(s[len++] != '\0');
	len-=1;
//	ES = 0;
	for(i = 0; i < len;i++)
	{
		SBUF = s[i];
		while(!TI);
		TI = 0;
	}
//	ES = 1;
}
void uartSend_number(long num)
{
	char i;
	uchar nums[12];
	uchar len = 0;
	while(num / 10)
	{
		nums[len++] = num % 10;
		num /= 10;
	}
	nums[len++] = num % 10;
	ES = 0;
	for(i = len - 1; i >= 0;i--)
	{
		SBUF = nums[i] + '0';
		while(!TI);
		TI = 0;
	}
	ES = 1;
}
bit strInclude(uchar*s1,uchar*s2,char len1, char len2)
{
	uchar i = 0,j = 0;
	if (len1 < len2)
		return 0;
	for(i = 0; i <= len1 - len2; i++)
	{
		for(j=0; j < len2; j++)
		{
			if(s1[i+j] != s2[j])
				break;
		}
		if(j == len2)
			return 1;
	}
	return 0;
}
/*********************
      转换函数
**********************/
char* num_to_Str(long num, char want_bit)
{
	uchar i;
	long t = 1;
	if(want_bit == -1)
	{
		want_bit = 1;
		while(num / t != 0)
		{
			want_bit++;
			t *=10;
		}

	}else
	{
		for(i=0; i < want_bit ;i++)
			t*=10;
		num %= t;
	}
	i = 0;
	t /= 10;
	while(t != 0)
	{
		strRes[i++] = num / t + '0';
		num %= t;
		t /= 10;
	}
	strRes[i] = '\0';
	return strRes;
}