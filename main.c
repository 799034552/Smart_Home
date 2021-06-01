#include "STC12C5A60S2.h"  //���ļ�
#include "uart.h"
#include "env.h"
#include "timer.h"
#include "music.h"
#include "key.h"
#define uchar unsigned char//�궨���޷����ַ���
#define uint unsigned int  //�궨���޷�������
/********************************************************************
                            ��ʼ����
*********************************************************************/
bit flag = 0;
bit dFlag = 0;
sbit led = P1^1;
sbit led5 = P1^5;
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
			break;
		}
	}
}
/*************************************************
                    ����1�¼�
*************************************************/
void uartEvent()
{
	if(strInclude(receive,"change",pos-2,6))
				led5 = !led5;
}
/*************************************************
                    ���ӷ�����
*************************************************/
void connectService()
{
	uartSend("AT+CIPMUX=1\r\n");
	waitUart();
	uartSend("AT+CIPSTART=0,\"TCP\",\"203.195.134.115\",7000\r\n");
	waitUart();
	uartSend("AT+CIPSEND=0,7\r\n");
	waitUart();
	uartSend("haha1\r\n");
	waitUart();
	delay1ms(1000);
	uartSend("AT+CIPSEND=0,7\r\n");
	waitUart();
	uartSend("haha2\r\n");
	waitUart();
	delay1ms(1000);
	uartSend("AT+CIPSEND=0,7\r\n");
	waitUart();
	uartSend("haha3\r\n");
	waitUart();
	delay1ms(1000);
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
	showTemperature();checkBusy();
	showPress();checkBusy();
	showLight();checkBusy();
	showWet();checkBusy();
	showDoorState();checkBusy();
	showLightState();checkBusy();
	showWork();checkBusy();
}
/********************************************************************
                            ������
*********************************************************************/   	
void main()
{
	time0_init();
	uartInit();
//	connectService();
	uart2Init();
	uart2Clear();
	uart2AddChar("CLR(16);DIR(3);FSIMG(2097152,0,0,320,240,0)");
	uart2SendEnd();
	delay1ms(1000);
//	uart2AddChar("");
//	uart2SendEnd();
//	pos = 0;
//	press_init();
//	music_init();
	delay1ms(1000);
//	updataSensor();
	while(1)
	{
		if(scanMatricKey(0) != -1)
		{
			led5 = !led5;
			delay1ms(300);
		}
//		play_one(2);
//		led = !led;
//		delay1ms(2000);
//		
		/*************
			ʱ�Ӹ����¼�
		**************/
//		if(time_update_flag)
//		{
//			//uartSend_number(2021);
//			time_update();
//			showTime();checkBusy();
//			time_update_flag = 0;
//		}
		/*************
			�����������¼�
		**************/
//		if(sensor_flag)
//		{
//			updataSensor();
//			sensor_flag = 0;
//		}
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
//		if(flag)
//		{
//			uartEvent();
//			receive[pos++] = '\0';
////			uartSend(receive);
//			uart2Clear();
//			uart2AddChar("CLR(1);");
//			uart2AddChar("DCV16(30,30,'");uart2AddChar(receive);uart2AddChar("',18);");
//			uart2SendEnd();
//			pos = 0;
//			flag = 0;
//		}
	}
}