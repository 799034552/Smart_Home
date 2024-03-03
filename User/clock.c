#include "clock.h"
#include "key.h"
#include "eeprom.h"
#include "uart.h"
#include "timer.h"
#include <intrins.h>
#define uint unsigned int
#define uchar unsigned char
bit uiflag = 0;
char bflag,key,num1 = 0;
xdata uchar xaxis[3] = {240,205,170};
xdata uchar yaxis[5] = {65,90,115,140,165};
char xdata hour1[5] = {0};
char xdata minute1[5] = {0};
char xdata second1[5] = {0};
uint  address1 = 0x0000;  // ���ڵ�һ������
uint  address2 = 0x0200;  // ���ڵڶ�������
void read_time(uchar num1);
void write_time(uchar num1);

void delay1(uint t)		//@11.0592MHz
{
	unsigned char i, j;
    uint a;
    for(a=0;a<t;a++)
    {
        _nop_();
        i = 10;
        j = 190;
        do
        {
            while (--j);
        } while (--i);
    }
}

void Show_onebit(uchar num, uchar bflag1, uchar time)
{
	if((num == num1) && (bflag1 == bflag)) //ѡ�б��
	{
		uart2AddChar("SBC(51);DCV24(");
		uart2AddChar(num_to_Str(xaxis[bflag1],-1));
		uart2AddChar(",");
		uart2AddChar(num_to_Str(yaxis[num],-1));
		uart2AddChar(",'");
		uart2AddChar(num_to_Str(time,2));
		if(bflag1 > 0)
		{
			uart2AddChar(":");
		}
		uart2AddChar("',30);");
	}
	else
	{
		uart2AddChar("SBC(51);DCV24(");
		uart2AddChar(num_to_Str(xaxis[bflag1],-1));
		uart2AddChar(",");
		uart2AddChar(num_to_Str(yaxis[num],-1));
		uart2AddChar(",'");
		uart2AddChar(num_to_Str(time,2));
		if(bflag1 > 0)
		{
			uart2AddChar(":");
		}
		uart2AddChar("',16);");
	}
}

void Show_oneclock(uchar num, uchar hour, uchar minute, uchar second)
{
	uart2Clear();
	Show_onebit(num,2,hour);
	Show_onebit(num,1,minute);
	Show_onebit(num,0,second);
	uart2SendEnd();
	checkBusy();
}

void setclock()
{	
	uint i;
	num1 = Byte_Read(address1);//ѡ�е����Ӻ�
	for( i = 0; i < 5; i ++) //��eeprom�����ʱ��
	{
		read_time(i);
	}
	do //����ɨ��
	{
		//��������ʾ����
		if(uiflag != 0)
		{
//		uart2AddChar("CLR(51);DIR(3);");
//		uart2SendEnd();
//		checkBusy();
			for(i = 0; i < 5; i++)
			{
				Show_oneclock(i,hour1[i],minute1[i],second1[i]);
			}
		}
		do
		{
			key = scanKey(0);
		}while(key == -1);
		switch(key)
		{
			case 2: if(num1 > 0)  //���� ��
							num1--;
							break;
			case 10: if(num1 < 4) //��
							num1++;
							break;
			case 5: bflag++;//�������    ��
							if(bflag == 3) bflag = 0;
							break;
			case 9: uiflag = 0;
							uart2Clear(); //������ʾ
							uart2AddChar("CLR(16);DIR(3);FSIMG(2097152,0,0,320,240,0)");
							uart2SendEnd();
							checkBusy();
							break;
			case 4: bflag--;//�������    ��
							if(bflag == -1) bflag = 2;
							break;
  }
	}while(key != 7 && uiflag == 1); //ȷ�ϼ�
	do
	{
		if(uiflag != 0)
		{
			for(i = 0; i < 5; i++)
			{
				Show_oneclock(i,hour1[i],minute1[i],second1[i]);
			}	
		}
		do
		{
			key = scanKey(1);
		}while(key == -1);
		switch(key)
		{
			case 9: uiflag = 0;
							uart2Clear(); //������ʾ
							uart2AddChar("CLR(16);DIR(3);FSIMG(2097152,0,0,320,240,0)");
							uart2SendEnd();
							checkBusy();
		case 2:
			switch(bflag) //����
			{
			case 0: second1[num1] ++; //��
					if(second1[num1] > 59) second1[num1] = 0;
					break;
			case 1: minute1[num1]++;  //��
					if(minute1[num1] > 59) minute1[num1] = 0;
					break;
			case 2: hour1[num1]++;		//ʱ
					if(hour1[num1] > 24) hour1[num1] = 0;
					break;
			}
			break;
		case 10:
			switch(bflag) //��С
			{
			case 0: second1[num1]--;
					if(second1[num1] < 0) second1[num1] = 59;
					break;
			case 1: minute1[num1]--;
					if(minute1[num1] < 0) minute1[num1] = 59;
					break;
			case 2: hour1[num1]--;
					if(hour1[num1] < 0) hour1[num1] = 23;
					break;
			}
			break;
		}
	}
	while(key != 7 && uiflag == 1);	
  Sector_Erase(address1);  
	delay1(50);
	Byte_Program(address1, num1);
	delay1(50);
	Byte_Program(address1+16,0x00);
	delay1(50);
	for(i = 0; i < 5; i++) //дeeprom�����ʱ��
	{
		write_time(i);
	}
}

void updateclock()
{
	uchar i,hour0,second0,minute0;
	for( i = 1; i < 6;i++)
	{
		deleteClock(i);
	}
	for( i = 0; i < 5;i++) //��������
	{
		hour0 = Byte_Read(address1+3*i+1);
		minute0 = Byte_Read(address1+3*i+2);
		second0 = Byte_Read(address1+3*i+3);
		addClock(hour0,minute0,second0,0x00);
	}
}

void transfer()
{
 key = scanKey(0);
 if(key == 9) //��ת��ť����
 {
  uiflag = ~uiflag; //��ת��־λ
 }
}

void read_time(uchar num1)
{
	hour1[num1] = Byte_Read(address1+num1*3+1);
	delay1(50);
//	if(hour1[num1] > 23) {hour1[num1] = 0;}
	minute1[num1] = Byte_Read(address1+num1*3+2);
	delay1(50);
//	if(minute1[num1] > 59) {minute1[num1] = 0;}
	second1[num1] = Byte_Read(address1+num1*3+3);
	delay1(50);
//	if(second1[num1] > 59) {second1[num1] = 0;}
}

void write_time(uchar num1)
{
	Byte_Program(address1+num1*3+1, hour1[num1]);
	delay1(50);
	Byte_Program(address1+num1*3+2, minute1[num1]);
	delay1(50);
	Byte_Program(address1+num1*3+3, second1[num1]);
	delay1(50);
}

void clock_init()
{	
	uchar i,flag;
	flag = Byte_Read(address1+16);
	if(flag != 0)
	{
		for(i = 0; i < 17; i++)
		{
			Byte_Program(address1+i,0);
			delay1(50);
		}
	}
}