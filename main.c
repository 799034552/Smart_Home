#include "STC12C5A60S2.h"  //库文件
#include "uart.h"
#include "env.h"
#include "timer.h"
#include "music.h"
#include "key.h"
#include "eeprom.h"
#include "ADC.h"
#include "clock.h"
#include <stdio.h>
#define uchar unsigned char//宏定义无符号字符型
#define uint unsigned int  //宏定义无符号整型
/********************************************************************
                            初始定义
*********************************************************************/
bit flag = 0;
bit dFlag = 0;
sbit led = P1^1;
sbit led5 = P1^5;
sbit led7 = P1^7;
sbit ledN = P0^7;
xdata uchar receive[100];
bit door = 0, light = 0, secure = 0;
uchar pos=0;
//------延迟函数------------
void delay1ms (uint t )
{
	uint i,j,k;
	for(k=0;k<t;k++)
	for( i=0; i<10; i++)
	for( j=0; j<95; j++);
}
/*************************************************
                  显示门状态
*************************************************/
void showDoorState()
{
	uart2Clear();
	if(door)
		uart2SendChar("SBC(61);DCV16(275,163,'ON',16);");
	else
		uart2SendChar("SBC(61);DCV16(275,163,'OFF',16);");
}
/*************************************************
                  显示灯状态
*************************************************/
void showLightState()
{
	uart2Clear();
	if(light)
		uart2SendChar("SBC(46);DCV16(275,220,'ON',16);");
	else
		uart2SendChar("SBC(46);DCV16(275,220,'OFF',16);");
}
/*************************************************
                  显示安保状态
*************************************************/
void showSecure()
{
	uart2Clear();
	if(secure)
		uart2SendChar("SBC(23);DCV16(190,220,'ON',16);");
	else
		uart2SendChar("SBC(23);DCV16(190,220,'OFF',16);");
}
/*************************************************
                  串口1中断函数
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
                    等待串口1返回
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
							向门发送数据
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
							向灯发送数据
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
							向服务器发送数据
*************************************************/
void sendToServe(uchar * s)
{
	int len = 0;
	uchar temp[30] = {'\0'};
	//return;
	while(s[len++] != '\0');
	len-= 1;
	sprintf(temp, "AT+CIPSEND=0,%d\r\n",len);
	uartSend(temp);
	waitUart();
	uartSend(s);
	waitUart();
}
/*************************************************
                    串口1事件
*************************************************/
void uartEvent()
{
	if(strInclude(receive,"doorOn",pos-2,6))
	{
		ledN = !ledN;
		door = 1;
		showDoorState();
		sendToDoor("open\r\n");
        delay1ms(500);
        sendToDoor("open\r\n");
        delay1ms(500);
	}
    if(strInclude(receive,"comeIn",pos-2,6))
	{
		ledN = !ledN;
        sendToServe("comeIn\r\n");
        delay1ms(200);
		if (secure)
		{
			warm_start();
			count_down_time = 15;
			uart2Clear();uart2AddChar("FSIMG(2250752,0,0,320,240,0)");uart2SendEnd();
			screen_state = 1;
		}
		
	}
	else if(strInclude(receive,"doorOff",pos-2,7))
	{
		ledN = !ledN;
		door = 0;
		showDoorState();
	}
	else if(strInclude(receive,"lightOn",pos-2,7))
	{
		ledN = !ledN;
		light = 1;
		showLightState();
		sendToLight("openl\r\n");
		delay1ms(500);
		sendToLight("openl\r\n");
		delay1ms(500);
	}
	else if (strInclude(receive,"lightClose",pos-2,10))
	{
		ledN = !ledN;
		light = 0;
		showLightState();
		sendToLight("closel\r\n");
		delay1ms(500);
		sendToLight("closel\r\n");
		delay1ms(500);
	}
	else if (strInclude(receive,"class",pos-2,5))
	{
		parseClass(receive);
		ledN = !ledN;
	}
	else if (strInclude(receive,"change",pos-2,6))
	{
		sendToServe("change\r\n");
		ledN = !ledN;
	}
}
///*************************************************
//                    串口屏是否忙
//*************************************************/
//void checkBusy()
//{
//	Busy = 1;
//	while(Busy);
//}
/*************************************************
                    连接服务器
*************************************************/
void connectService()
{
	long time = 0;
	uchar i = 14;
	//return;
	uartSend("AT+CIPMUX=1\r\n");
	waitUart();
	uartSend("AT+CIPSTART=0,\"TCP\",\"203.195.134.115\",7000\r\n");
	//uartSend("AT+CIPSTART=0,\"TCP\",\"192.168.123.210\",7000\r\n");
	waitUart();
	uartSend("AT+CIPSERVER=1,8080\r\n");
	waitUart();
	//sendToServe("haha123\r\n");
	sendToServe("time\r\n");
	while(1)
	{
		if(flag){
			receive[pos++] = '\0';
			if (strInclude(receive,"time",pos-2,4))
			{
				while(receive[i] != '\r' && receive[i] != '\0')
				{
					time = time*10 + receive[i] - '0';
					i++;
				}
				setTime(time);
				uart2Clear();
				uart2AddChar("CLR(1);");
				uart2AddChar("DCV16(30,30,'");uart2AddChar(receive);uart2AddChar("',18);");
				uart2SendEnd();checkBusy();
				break;
			}
			flag = 0;
			pos = 0;
			uart2Clear();
			uart2AddChar("CLR(1);");
			uart2AddChar("DCV16(30,30,'");uart2AddChar(receive);uart2AddChar("',18);");
			uart2SendEnd();
		}
	}
}
/*************************************************
                    串口2中断
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
                    更新传感器数据
*************************************************/
void updataSensor()
{
	showTemperature();checkBusy();
	showPress();checkBusy();
	showLight();checkBusy();
	showWet();checkBusy();
	showDoorState();checkBusy();
	showLightState();checkBusy();
	showSecure();checkBusy();
}

/********************************************************************
                            主函数
*********************************************************************/   	
void main()
{
    char key = -1;
	delay1ms(2000);
	time0_init();
	uartInit();
	uart2Init();
	//connectService();
	uart2Clear(); //背景显示
	uart2AddChar("CLR(16);DIR(3);FSIMG(2097152,0,0,320,240,0)");
	uart2SendEnd();
	delay1ms(1000);
	uart2AddChar("");
	uart2SendEnd();
	pos = 0;
	press_init();
	music_init();
	clock_init();
//	InitADC();
	updataSensor();
//	play_one(1);
	while(1)
	{
		uchar pflag;
		if(uiflag == 0 && screen_state == 0)
		{
//			uart2Clear(); //背景显示
//			uart2AddChar("CLR(16);DIR(3);FSIMG(2097152,0,0,320,240,0)");
//			uart2SendEnd();
//		led5 = !led5;
//		delay1ms(1000);
//		if(scanKey(0) != -1)
//		{
//			led5 = !led5;
//			delay1ms(300);
//		}
//		play_one(2);
//		led = !led;
//		delay1ms(2000);
//		

		/*************
			传感器更新事件
		**************/
		if(sensor_flag)
		{
			updataSensor();
			sensor_flag = 0;
		}
		/*************
			时钟更新事件
		**************/
		if(time_update_flag)
		{
			time_update();
			showTime();checkBusy();
			time_update_flag = 0;
		}
     /*************
			传感器发送事件
		**************/
//			if(sensor_send_flag)
//			{
//					sensor_send_flag = 0;
//					sendToServe(getAllEnv());
//			}
		/*************
			压力传感器更新事件
		**************/
		if(AD_flag)
		{
			uchar pressure;
			InitADC();
			pressure = GetADCResult(1);
			if(pressure < 100 && secure) //报警
			{
				warm_start();
				count_down_time = 15;
				uart2Clear();uart2AddChar("FSIMG(2250752,0,0,320,240,0)");uart2SendEnd();
				screen_state = 1;
//				clock_start();
//				delay1ms(1000);
//				clock_stop();
			}
			AD_flag = 0;
		}
		/*************
			按键扫描事件
		**************/
    key = scanKey(0);
		if(key != -1)
		{
            if(key == 3)
            {
                ledN = !ledN;
                door = 1;
                door_flag = 7;
                showDoorState();
                sendToDoor("open\r\n");
                delay1ms(500);
                sendToDoor("open\r\n");
                delay1ms(500);
                sendToServe("lockOn\r\n");
                delay1ms(500);
            }
						else if (key == 0)
						{
							ledN = !ledN;
							light = 1;
							showLightState();
							sendToLight("openl\r\n");
							delay1ms(500);
							sendToLight("openl\r\n");
							delay1ms(500);
						}
						else if (key == 1)
						{
							ledN = !ledN;
							light = 0;
							showLightState();
							sendToLight("closel\r\n");
							delay1ms(500);
							sendToLight("closel\r\n");
							delay1ms(500);
						}
            else if (key == 6)
            {
							ledN = !ledN;
							secure = !secure;
							showSecure();checkBusy();
            }
						else if(key == 9)
						{
							uiflag = ~uiflag;
						}
		}
        if(door_flag == 0)
        {
            ledN = !ledN;
            door = 0;
            door_flag = -1;
            showDoorState();
        }
		/*************
		  接收服务器数据
		****************/
		if(flag)
		{
			uartEvent();
			//receive[pos++] = '\0';
			//uartSend(receive);
//			if(!strInclude(receive,"bytes",pos-2,5))
//			{
//				uart2Clear();
//				uart2AddChar("DCV16(70,100,'");uart2AddChar(receive);uart2AddChar("',18);");
//				uart2SendEnd();
//			}
			pos = 0;
			flag = 0;
		}
		}
		else if(uiflag == 0 && screen_state == 1)
		{
			if (scanKey(0) != -1)
			{
				screen_state = 2;
				clock_stop();
			}
		}
		else if (uiflag == 0 && screen_state == 2)
		{
			uart2AddChar("FSIMG(2097152,0,0,320,240,0)");uart2SendEnd();checkBusy();
			showTime();checkBusy();updataSensor();
			screen_state = 0;
			changeToNowTime(nowTime);
		}
		/*************
		  闹钟串口屏显示
		****************/
		else 
		{
			if(pflag == 0)
			{
				uart2Clear(); //背景显示
				uart2AddChar("CLR(51);DIR(3);");
				uart2SendEnd();
				checkBusy();
				pflag = 1;
			}
			updateclock();//更新闹钟
			setclock(); //设置闹钟
			updateclock();
		}
	}
}