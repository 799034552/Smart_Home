#include "key.h"
#include <intrins.h>
#define uint unsigned int 
#define uchar unsigned char 
uchar keys[] = {3,4,5,6};
/*****************
 延迟
******************/
void delayms (uint t )
{
	uint i,j,k;
	for(k=0;k<t;k++)
	for( i=0; i<10; i++)
	for( j=0; j<95; j++);
}
/*****************
 获取某个p口的某个电平值
******************/
bit getBit(uchar p, uchar i) {
	uchar mask = 0x01 << i;
	switch(p){
		case 0:
		 return ((P0 & mask) == 0?0:1);
		case 1: 
			return ((P1 & mask) == 0?0:1);
		case 2: 
			return ((P2 & mask) == 0?0:1);
		case 3: 
			return ((P3 & mask) == 0?0:1);
	}
	return 0;
}
/*****************
 扫描按键
******************/
char scanKey(bit longAble)
{
	static bit be_state = 0;
	uchar i;
	if(longAble)
		be_state = 1;
	for(i = 0; i < 4; i++)
	{
		if(getBit(3,keys[i]) == 0)
		{
			delayms(10);
			if(getBit(3,keys[i]) == 0)
			{
				be_state = 1;
				return i;
			}
		}
	}
	be_state = 0;
	return -1;
}
/*****************
 扫描矩阵按键
******************/
char scanMatricKey(bit longAble) {
	static bit be_state = 0;
	uchar temp, out = 0xfe,mask = 0xef,i, j;
	if(longAble)
		be_state = 1;
	for (i = 0; i < 4; i++) {
		P3 = _crol_(out, i);
		temp = P3 & 0xf0;
		if (temp != 0xf0) {
			delayms(10);
			P3 = _crol_(out, i);
			temp = P3 & 0xf0;
			if (temp != 0xf0) {
				temp = P3;
				for (j = 0; j < 4; j++) {
					 if (temp == ((_crol_(out, i) & 0x0f) | (_crol_(mask, j) & 0xf0))) {
						 be_state = 1;
						 return i*4+j;
						 break;
					 }
				}
			}
		}
	}
	be_state = 0;
	return 0;
}