#ifndef _TYPE_DEFINE_H
#define _TYPE_DEFINE_H

#include "macro_define.h"
union U_B16B08
{
	uint8_t  B08[2];
	uint16_t  B16;
};

union U_B32B08
{
	uint8_t  B08[4];
	uint32_t  B32;
};

struct S_B32ADDB08
{
	union   U_B32B08   u_E32;
   uint8_t             B08;  
};

struct S_LONG_ADD_CHAR
{
	uint32_t   B32;
   uint8_t   B08;  
};

////////////////////////////////////////////////////////////////////

struct S_MeterVariable   //��������
{
	union U_B32B08   u_U;				//��ѹxxxxxxx.x
	union U_B32B08   u_I1;				//����xxxxx.xxx
	union U_B32B08   u_P1;				//�й�����xxxx.xxxx
	union U_B32B08   u_P2;				//���ڹ���xxxx.xxxx
	union U_B32B08   u_Cosa;			//��������xxxxx.xxx
	
	union U_B32B08   u_I2;				//���ߵ���xxxxx.xxx	
	union U_B16B08   u_Freq;         //����Ƶ��xx.xx	
	union U_B32B08   u_Pav;				//һ����ƽ������xxxx.xxxx
	union U_B16B08   u_Temper_M;		//��ǰ����¶�xxx.x
	union U_B16B08   u_Voltage_B;		//��ǰ��ص�ѹxx.xx
	union U_B32B08   u_Voltage_B_T;	//��ص�ѹ����ʱ��xxxxxxxx(hex����???)
};

union  U_220VFlag      //220V���λ��ʶ
{
    uint8_t    Flag;
    struct
    {
        uint8_t  Below_80v:  1;
        uint8_t  Below_70v:  1;
        uint8_t  Below_60v:  1;
        uint8_t  Up_115v:    1;
    }  Bit;
};

struct S_EmuParameter
{
    union   U_B16B08  u_PAGAIN_Para;//PAGAINУ׼����
    union   U_B16B08  u_APHCAL_Para;//PAHCALУ׼����
    union   U_B16B08  u_PAOFF_Para; //PAOFFУ׼����
    
    union   U_B16B08  u_UOFF_Para; //UOFFУ׼����     
    union   U_B16B08  u_UGAIN_Para; //UGAINУ׼����  
 
    union   U_B16B08  u_IAOFF_Para; //IAOFFУ׼���� 
    union   U_B16B08  u_IAGAIN_Para;//IAGAINУ׼����  
    
    union   U_B16B08  u_IBOFF_Para; //IBOFFУ׼����    
    union   U_B16B08  u_IBGAIN_Para;//IBGAINУ׼����   
  
    union   U_B16B08  u_Crc16;		 //2�ֽ�У���
};



#endif
