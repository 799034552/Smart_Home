#include "reg52.h"
#include "intrins.h"
#include "ADC.h"

sfr ADC_CONTR   =   0xBC;           //ADC 控制寄存器
sfr ADC_RES     =   0xBD;           //ADC 转换结果寄存器
sfr ADC_LOW2    =   0xBE;           //ADC 低2位结果寄存器
sfr P1ASF       =   0x9D;           //Pl口模拟功能控制寄存器

/********************************************************************
                             延时函数
*********************************************************************/
void Delay2(uint n)
{
    uint x;
    while (n--)
    {
        x = 5000;
        while (x--);
    }
}
/********************************************************************
                              ADC 初始化
*********************************************************************/
void InitADC()
{
    P1ASF = 0x02;                  		  //设置P1口为开漏模式，使用AD功能
    ADC_RES = 0;                   		  //AD数据寄存器清空
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL;  //打开AD电源，转换周期420
    Delay2(2);                    		  //延时
}
/********************************************************************
                              ADC 数据转换函数
*********************************************************************/
uchar GetADCResult(uchar ch)
{
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ch | ADC_START;//开始转换
    _nop_();                        //延时一个机器周期
    _nop_();						//延时一个机器周期
    _nop_();						//延时一个机器周期
    _nop_();						//延时一个机器周期
    while (!(ADC_CONTR & ADC_FLAG));//等待转换结束
    ADC_CONTR &= ~ADC_FLAG;         //关闭AD转换

    return ADC_RES;                 //返回数据
}
