#include "music.h"
#define uint unsigned int 
#define uchar unsigned char 
sbit io1 = P0^0;
sbit io2 = P0^2;
sbit io3 = P0^2;
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
	io3 = 1;
}
void clock_start()
{
	io1 = 0;
}
void clock_stop()
{
	io1 = 1;
}
void play_one(char n)
{
	uchar t;
	if(n > N)
		t = n -N;
	else
		t = n - N + 10;
	while(t--)
	{
		io3 = 0;
		delays(70);
		io3 = 1;
		delays(70);
	}
	N = n;
}