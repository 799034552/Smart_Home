#ifndef __UART_H_
#define __UART_H_
#include "STC12C5A60S2.h"
#define uchar unsigned char
void uart2Init(void);
unsigned char ES2(char b);
unsigned char S2RI(char b);
unsigned char S2TI(char b);
void uart2AddChar(char* s);
void uart2Clear();
void uart2Send();
void uart2SendChar(char*s);
void uart2SendEnd();
void uartInit(void);
void uartSend(char*s);
void uartSend_number(long num);
bit strInclude(uchar*s1,uchar*s2,char len1, char len2);
char* num_to_Str(long num, char want_bit);
void uart2AddCharLen(char* s, uchar len);
#endif
