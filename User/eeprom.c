#include "INTRINS.H"
#include "eeprom.h"
//typedef unsigned char  uchar;
//typedef unsigned int   uint;

//sfr IAP_DATA    = 0xC2;
//sfr IAP_ADDRH   = 0xC3;
//sfr IAP_ADDRL   = 0xC4;
//sfr IAP_CMD     = 0xC5;
//sfr IAP_TRIG    = 0xC6;
//sfr IAP_CONTR   = 0xC7;

//����Flash �����ȴ�ʱ�估����IAP/ISP/EEPROM �����ĳ���
#define ENABLE_ISP 0x82 //ϵͳ����ʱ��<20MHz ʱ����IAP_CONTR �Ĵ������ô�ֵ


#define DEBUG_DATA               0x5A  //�����Գ������մ洢�� EEPROM ��Ԫ����ֵ
#define DATA_FLASH_START_ADDRESS 0x00  //STC5Axx ϵ�� EEPROM ������ʼ��ַ

union union_temp16
{
    uint un_temp16;
    uchar  un_temp8[2];
}my_unTemp16;

uchar Byte_Read(uint add);              //��һ�ֽڣ�����ǰ���IAP ����
void Byte_Program(uint add, uchar ch);  //�ֽڱ�̣�����ǰ���IAP ����
void Sector_Erase(uint add);            //��������
void IAP_Disable();                       //�ر�IAP ����
void Delayms();

//��һ�ֽڣ�����ǰ���IAP ���ܣ����:DPTR = �ֽڵ�ַ������:A = �����ֽ�
uchar Byte_Read(uint add)
{
    IAP_DATA = 0x00;
    IAP_CONTR = ENABLE_ISP;         //��IAP ����, ����Flash �����ȴ�ʱ��
    IAP_CMD = 0x01;                 //IAP/ISP/EEPROM �ֽڶ�����

    my_unTemp16.un_temp16 = add;
    IAP_ADDRH = my_unTemp16.un_temp8[0];    //����Ŀ�굥Ԫ��ַ�ĸ�8 λ��ַ
    IAP_ADDRL = my_unTemp16.un_temp8[1];    //����Ŀ�굥Ԫ��ַ�ĵ�8 λ��ַ

    //EA = 0;
    IAP_TRIG = 0x5A;   //���� 5Ah,����A5h ��ISP/IAP �����Ĵ���,ÿ�ζ������
    IAP_TRIG = 0xA5;   //����A5h ��ISP/IAP ����������������
    _nop_();
    //EA = 1;
    IAP_Disable();  //�ر�IAP ����, ����ص����⹦�ܼĴ���,ʹCPU ���ڰ�ȫ״̬,
                    //һ��������IAP �������֮����ر�IAP ����,����Ҫÿ�ζ���
    return (IAP_DATA);
}

//�ֽڱ�̣�����ǰ���IAP ���ܣ����:DPTR = �ֽڵ�ַ, A= �����ֽڵ�����
void Byte_Program(uint add, uchar ch)
{
    IAP_CONTR = ENABLE_ISP;         //�� IAP ����, ����Flash �����ȴ�ʱ��
    IAP_CMD = 0x02;                 //IAP/ISP/EEPROM �ֽڱ������

    my_unTemp16.un_temp16 = add;
    IAP_ADDRH = my_unTemp16.un_temp8[0];    //����Ŀ�굥Ԫ��ַ�ĸ�8 λ��ַ
    IAP_ADDRL = my_unTemp16.un_temp8[1];    //����Ŀ�굥Ԫ��ַ�ĵ�8 λ��ַ

    IAP_DATA = ch;                  //Ҫ��̵��������ͽ�IAP_DATA �Ĵ���
    //EA = 0;
    IAP_TRIG = 0x5A;   //���� 5Ah,����A5h ��ISP/IAP �����Ĵ���,ÿ�ζ������
    IAP_TRIG = 0xA5;   //����A5h ��ISP/IAP ����������������
    _nop_();
    //EA = 1;
    IAP_Disable();  //�ر�IAP ����, ����ص����⹦�ܼĴ���,ʹCPU ���ڰ�ȫ״̬,
                    //һ��������IAP �������֮����ر�IAP ����,����Ҫÿ�ζ���
}

//��������, ���:DPTR = ������ַ
void Sector_Erase(uint add)
{
    IAP_CONTR = ENABLE_ISP;         //��IAP ����, ����Flash �����ȴ�ʱ��
    IAP_CMD = 0x03;                 //IAP/ISP/EEPROM ������������

    my_unTemp16.un_temp16 = add;
    IAP_ADDRH = my_unTemp16.un_temp8[0];    //����Ŀ�굥Ԫ��ַ�ĸ�8 λ��ַ
    IAP_ADDRL = my_unTemp16.un_temp8[1];    //����Ŀ�굥Ԫ��ַ�ĵ�8 λ��ַ

    //EA = 0;
    IAP_TRIG = 0x5A;   //���� 5Ah,����A5h ��ISP/IAP �����Ĵ���,ÿ�ζ������
    IAP_TRIG = 0xA5;   //����A5h ��ISP/IAP ����������������
    _nop_();
    //EA = 1;
    IAP_Disable();  //�ر�IAP ����, ����ص����⹦�ܼĴ���,ʹCPU ���ڰ�ȫ״̬,
                    //һ��������IAP �������֮����ر�IAP ����,����Ҫÿ�ζ���
}

void IAP_Disable()
{
    //�ر�IAP ����, ����ص����⹦�ܼĴ���,ʹCPU ���ڰ�ȫ״̬,
    //һ��������IAP �������֮����ر�IAP ����,����Ҫÿ�ζ���
    IAP_CONTR = 0;      //�ر�IAP ����
    IAP_CMD   = 0;      //������Ĵ���,ʹ����Ĵ���������,�˾�ɲ���
    IAP_TRIG  = 0;      //��������Ĵ���,ʹ������Ĵ����޴���,�˾�ɲ���
    IAP_ADDRH = 0;
    IAP_ADDRL = 0;
}

void Delayms()
{
    uchar i;
    uint d=5000;
    while (d--)
    {
        i=255;
        while (i--);
    }
}