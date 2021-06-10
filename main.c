#include "STC12C5A60S2.h"  //���ļ�
#include "uart.h"
#include "env.h"
#include "timer.h"
#include "music.h"
#include "key.h"
#include <stdio.h>
#define uchar unsigned char//�궨���޷����ַ���
#define uint unsigned int  //�궨���޷�������
/********************************************************************
                            ��ʼ����
*********************************************************************/
bit flag = 0;
bit dFlag = 0;
sbit led = P1^1;
sbit led5 = P1^5;
sbit led7 = P1^7;
bit Busy = 0;
xdata uchar receive[100];
bit door = 0, light = 0;
xdata int workTime = 10;
uchar pos=0;
//------�ӳٺ���------------
void delay1ms (uint t )
{
	uint i,j,k;
	for(k=0;k<t;k++)
	for( i=0; i<10; i++)
	for( j=0; j<95; j++);
}
/*************************************************
                  ��ʾ��״̬
*************************************************/
void showDoorState()
{
	uart2Clear();
	
	if(door)
		uart2SendChar("SBC(51);DCV16(275,163,'ON',16);");
	else
		uart2SendChar("SBC(51);DCV16(275,163,'OFF',16);");
}
/*************************************************
                  ��ʾ��״̬
*************************************************/
void showLightState()
{
	uart2Clear();
	
	if(door)
		uart2SendChar("SBC(46);DCV16(275,220,'ON',16);");
	else
		uart2SendChar("SBC(46);DCV16(275,220,'OFF',16);");
}
/*************************************************
                  ��ʾ����ʱ��
*************************************************/
void showWork()
{
	uart2Clear();
	uart2AddChar("SBC(23);DCV16(178,225,'");
	uart2AddChar(num_to_Str(workTime,-1));
	uart2AddChar("����',16);");
	uart2SendEnd();
}
/*************************************************
                  ����1�жϺ���
*************************************************/
void ser() interrupt 4
{
	uchar temp;
	RI=0;
	temp = SBUF;
	if (flag)
		return;
	if (pos >= 100){
		dFlag = 0;
		pos = 0;
	}
	if(temp == ',')
		temp = '.';
	receive[pos++] = temp;
	if (dFlag){
		if(temp == 0x0a && pos > 2)
			flag = 1;
		else
			pos = 0;
		dFlag = 0;
	} else{
		if(temp == 0x0d)
			dFlag = 1;
		else if (temp == 0x0a)
			pos = 0;
	}
}
/*************************************************
                    �ȴ�����1����
*************************************************/
void waitUart()
{
	while(1)
	{
		if(flag){
			flag = 0;
			pos = 0;
			delay1ms(500);
			break;
		}
	}
}
/*************************************************
							���ŷ�������
*************************************************/
void sendToDoor(uchar * s)
{
	int len = 0;
	uchar temp[30] = {'\0'};
	while(s[len++] != '\0');
	len-= 1;
	sprintf(temp, "AT+CIPSEND=1,%d\r\n",len);
	uartSend(temp);
	waitUart();
	uartSend(s);
	waitUart();
}
/*************************************************
							��Ʒ�������
*************************************************/
void sendToLight(uchar * s)
{
	int len = 0;
	uchar temp[30] = {'\0'};
	while(s[len++] != '\0');
	len-= 1;
	sprintf(temp, "AT+CIPSEND=2,%d\r\n",len);
	uartSend(temp);
	waitUart();
	uartSend(s);
	waitUart();
}

/*************************************************
                    ����1�¼�
*************************************************/
void uartEvent()
{
	if(strInclude(receive,"doorOn",pos-2,6))
	{
		led7 = !led7;
		sendToDoor("open\r\n");
	}
	else if(strInclude(receive,"lightOn",pos-2,7))
	{
		led7 = !led7;
		sendToLight("open\r\n");
	}
	else if (strInclude(receive,"lightClose",pos-2,10))
	{
		led7 = !led7;
		sendToLight("close\r\n");
	}
	else if (strInclude(receive,"class",pos-2,5))
	{
		parseClass(receive);
		led7 = !led7;
	}
	else if (strInclude(receive,"change",pos-2,6))
		led7 = !led7;
}
/*************************************************
							���������������
*************************************************/
void sendToServe(uchar * s)
{
	int len = 0;
	uchar temp[30] = {'\0'};
	while(s[len++] != '\0');
	len-= 1;
	sprintf(temp, "AT+CIPSEND=0,%d\r\n",len);
	uartSend(temp);
	waitUart();
	uartSend(s);
	waitUart();
}
/*************************************************
                    ���ӷ�����
*************************************************/
void connectService()
{
	uartSend("AT+CIPMUX=1\r\n");
	waitUart();
	uartSend("AT+CIPSTART=0,\"TCP\",\"203.195.134.115\",7000\r\n");
	//uartSend("AT+CIPSTART=0,\"TCP\",\"192.168.123.210\",7000\r\n");
	waitUart();
	uartSend("AT+CIPSERVER=1,8080\r\n");
	waitUart();
	sendToServe("haha123\r\n");
}
/*************************************************
                    ����2�ж�
*************************************************/
void uart2_isr()  interrupt 8
{
	if(S2RI(-1))
	{
		S2RI(0);
		if(S2BUF == '\n')
			Busy = 0;
//		dat = S2BUF;
//		flag = 1;
	 }
}
/*************************************************
                    �������Ƿ�æ
*************************************************/
void checkBusy()
{
	Busy = 1;
	while(Busy);
}
/*************************************************
                    ���´���������
*************************************************/
void updataSensor()
{
	showTemperature();//checkBusy();
	showPress();//checkBusy();
	showLight();//checkBusy();
	showWet();//checkBusy();
	showDoorState();//checkBusy();
	showLightState();//checkBusy();
	showWork();//checkBusy();
	if(!flag)
		sendToServe(getAllEnv());
}
/********************************************************************
                            ������
*********************************************************************/   	
void main()
{
	delay1ms(4000);
	time0_init();
	uartInit();
	connectService();
	uart2Init();
	uart2Clear();
	uart2AddChar("CLR(16);DIR(3);FSIMG(2097152,0,0,320,240,0)");
	uart2SendEnd();
	delay1ms(1000);
	uart2AddChar("");
	uart2SendEnd();
	pos = 0;
	press_init();
	music_init();
	updataSensor();
	led7 = 0;
	while(1)
	{
//		led5 = !led5;
//		delay1ms(1000);
//		if(scanMatricKey(0) != -1)
//		{
//			led5 = !led5;
//			delay1ms(300);
//		}
//		play_one(2);
//		led = !led;
//		delay1ms(2000);
//		
		/*************
			ʱ�Ӹ����¼�
		**************/
		if(time_update_flag)
		{
			time_update();
			showTime();//checkBusy();
			time_update_flag = 0;
		}
		/*************
			�����������¼�
		**************/
		if(sensor_flag)
		{
			updataSensor();
			sensor_flag = 0;
		}
		/*************
		ʪ�ȴ�����
		**************/
//		if(GetData())
//		{
//			uartSend(getWetStr());
//		}
//		delay1ms(2000);
		
		/*************
		��ѹ������
		**************/
//		delay1ms(1000);
//		get_temperature();
//		uartSend("�¶�:");
//		uartSend(getTempStr());
//		get_pressure();
//		uartSend("ѹ��:");
//		uartSend(getPressureStr());
//		delay1ms(1000);
		
		
		/*************
		  ���մ�����
		****************/
//		light_start();
//		light_read();
//		uartSend("����:");
//		uartSend(getLightStr());
//		led=!led;
//		delay1ms(1000);
		
		/*************
		  ���շ���������
		****************/
		if(flag)
		{
			uartEvent();
			receive[pos++] = '\0';
//			uartSend(receive);
//			uart2Clear();
//			uart2AddChar("CLR(1);");
//			uart2AddChar("DCV16(30,30,'");uart2AddChar(receive);uart2AddChar("',18);");
//			uart2SendEnd();
			pos = 0;
			flag = 0;
		}
	}
}