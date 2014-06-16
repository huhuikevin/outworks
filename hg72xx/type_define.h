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

struct S_MeterVariable   //计量变量
{
	union U_B32B08   u_U;				//电压xxxxxxx.x
	union U_B32B08   u_I1;				//电流xxxxx.xxx
	union U_B32B08   u_P1;				//有功功率xxxx.xxxx
	union U_B32B08   u_P2;				//视在功率xxxx.xxxx
	union U_B32B08   u_Cosa;			//功率因数xxxxx.xxx
	
	union U_B32B08   u_I2;				//零线电流xxxxx.xxx	
	union U_B16B08   u_Freq;         //电网频率xx.xx	
	union U_B32B08   u_Pav;				//一分钟平均功率xxxx.xxxx
	union U_B16B08   u_Temper_M;		//当前电表温度xxx.x
	union U_B16B08   u_Voltage_B;		//当前电池电压xx.xx
	union U_B32B08   u_Voltage_B_T;	//电池电压工作时间xxxxxxxx(hex保存???)
};

union  U_220VFlag      //220V检测位标识
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
    union   U_B16B08  u_PAGAIN_Para;//PAGAIN校准参数
    union   U_B16B08  u_APHCAL_Para;//PAHCAL校准参数
    union   U_B16B08  u_PAOFF_Para; //PAOFF校准参数
    
    union   U_B16B08  u_UOFF_Para; //UOFF校准参数     
    union   U_B16B08  u_UGAIN_Para; //UGAIN校准参数  
 
    union   U_B16B08  u_IAOFF_Para; //IAOFF校准参数 
    union   U_B16B08  u_IAGAIN_Para;//IAGAIN校准参数  
    
    union   U_B16B08  u_IBOFF_Para; //IBOFF校准参数    
    union   U_B16B08  u_IBGAIN_Para;//IBGAIN校准参数   
  
    union   U_B16B08  u_Crc16;		 //2字节校验和
};



#endif
