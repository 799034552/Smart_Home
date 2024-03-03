#include "music.h"
#include "timer.h"
#define uint unsigned int 
#define uchar unsigned char 
sbit io1 = P1^5;
sbit io3 = P1^7;
sbit io2 = P1^6;
uchar N = 3;
void delays (uint t )
{
	uint i,j,k;
	for(k=0;k<t;k++)
	for( i=0; i<10; i++)
	for( j=0; j<95; j++);
}
void music_init()
{
	io1 = 1;
	io2 = 1;
	io3 = 1;
}
void clock_start()
{
	io1 = 0;
}
void warm_start()
{
	io3 = 0;
}
void class_start()
{
	io2 = 0;
}
void clock_stop()
{
	io1 = 1;
	io2 = 1;
	io3 = 1;
}