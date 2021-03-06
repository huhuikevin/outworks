//说明：FSK载波 过零发送模式
//从 plcfsk_rx_v12z_hd.c 修改而来
#include <hic.h>
#include "type.h"
#include "system.h"
#include "timer8n.h"
#include "soc_25xx.h"
#include "tool.h"
#include "debug.h"
#include "config.h"

#ifdef CONFIG_200BPS_PLC
#define  CCPMODE  0x02
#define  COMPMODE 0x01
#define  RH_Time  0X05
#define  RL_Time  0X33
#ifdef SENDER
#define  R_LED PB5
#else
#define  R_LED PB7
#endif
#define  S_LED PB7  
#define	 STA     PB6
#define  PLC_TXEN  PC1

#define  PLC_TX   PLC_TXEN=1
#define  PLC_RX   PLC_TXEN=0

#define  Sync_bit1_cnt_Max  0x1f

#define TX_Bit1()	            PLC_MOD|=0x01    //PLC_TXD
#define TX_Bit0()               PLC_MOD&=0xfe
#define MaxPlcL 100
#define Zero_Num 4

#define NORMAL 			0x03
#define PnNum_Const 15
#define Sync_Set    0x5A
#define Sync21_Set    0x6e
#define Sync_Char  0xaf
#define Pn7_Con1   0X17

static volatile   section64  sbit  PLC_FSK_RXD      @ (unsigned) &PLC_MOD* 8 + 3 ;

unsigned char Sync_bit1_cnt,Work_step;
uchar section1  testbyte, Rx_status;
                                                /* 8ms*/  /* 6.7ms*/
const uchar  T16g1RH_Time[6]={0x40,0x9c,0xdc,0x82,0xad,0x6e};


uchar  Sum_Max,ZXDM,Sum_FXMax,Temp1,FXDM;

section20 uchar BitData_T[64]@0xa00;//BitDataBak_T[16];	//同步时的位数据
//section20 uchar BitData_T1[64]@0xa40;

section20 uchar Bit1Num_T[64]@0xa40;					//同步时收到1的个数（18T内）
//section21 uchar Bit1Num_T1[64]@0xac0;

uchar  SYM_offset,SYCl_offset;
uchar  SYM_offset2,SYCl_offset2;
uchar  Sync_Step;

uchar  plc_byte_data,sync_word_l;
sbit   r_sync_bit,t_nor_bit,t_end_bit,Plc_SyncBZ,Psk_FxBz,Rec_Zero_bz;
sbit   Plc_Tx_Bit,Plc_Tx_first_Bit,Plc_Tx_Second_Bit;
uchar  Plc_data_bit_cnt,Plc_data_byte_cnt,Plc_data_len,Plc_Mode,Plc_ZeroMode;
uchar  Plc_Samples_bit1_cnt, Plc_Samples_byte;

section3 uchar plc_data[MaxPlcL]@0x180;
section3 uchar RSSIV,RSSIT; 
section3 uchar SYM_off[8],SYCl_off[8];


section14 uchar Zero_cnt;
section14 union SVR_INT_B08 T16G1R_int[8],T16G1R_20ms[8],T16G1R_old,T16G1R_new;//T16G1R1,T16G1R2,T16G1R3,T16G1R4,T16G1R5,T16G1R6;
section14 union SVR_LONG_B08 T16G1R_S;

section13 uchar RSSIByte_buf[32],RSSIBit_buf[8];
section13 uchar ZXDM_buf[40],RSSIV_buf[40];

#if 0
#define trans_step_next()\
do {\
	if (trans_step == 0){ \
		T16G2RH = T3_53ms_RH;\
		T16G2RL = T3_53ms_RL;\
	}else{\
		T16G2RH = T6_47ms_RH;\
		T16G2RL = T6_47ms_RL;\
	}\
	trans_step ^= 1;\
}while(0);
#else
#define trans_step_next()\
do {\
    T16G2RH=T16G1R_S.NumChar[1];	\
    T16G2RL=T16G1R_S.NumChar[0]; \
}while(0);
#endif


#define plc_restart() \
do{\
	T16G2IE=0;\
  	Plc_Mode=0;\
  	Work_step=0;\
  	Sync_Step=0;\
  	Plc_SyncBZ=0;\
	r_sync_bit = 0;\
   T16G1IF=0;\
  	T16G1IE=1;\
}while(0);

#define plc_restart_sync() \
do{\
	T16G2IE=0;\
  	Plc_Mode=0;\
  	Work_step=0;\
  	Sync_Step=0;\
  	Plc_SyncBZ=0;\
	r_sync_bit = 0;\
   T16G1IF=0;\
  	T16G1IE=1;\
}while(0);

#define plc_recv_finished()\
do{\
	disable_irq();\
	T16G2IE=0;\
	T16G1IE=0;\
}while(0);\


#define do_left_shift_1bit(b) \
do {\
	BitData_T[b]<<=1;\
	if(BitData_T[b+1]&0x80) \
	{		\
		BitData_T[b]++;	\
		Bit1Num_T[b]++;\
		Bit1Num_T[b+1]--;\
	}\
}while(0);


void IniT16G1(uchar Mode)
{
    if(Mode==CCPMODE)		//捕获
    {
        T16G1CL=0x21;
        T16G1CH=0x04;
    }
    else if(Mode==COMPMODE)		//比较
    {
        T16G1CH=0x0b;
        T16G1RH=RH_Time;
        T16G1RL=RL_Time;
        T16G1CL=0x21;		//4:1 
    }
}
void IniT16G2(uchar Mode)
{
  
    T16G2CH=0x0b;
    T16G2CL=0x10;		//2:1 
    switch(Mode)
    {
    case 1:
        // T16G1RH=T16G1R_S.NumChar[1];  //赋下次过零接收时间
        //  T16G1RL=T16G1R_S.NumChar[0];
        T16G2RH=T16g1RH_Time[1];		//过零后8ms发
        T16G2RL=T16g1RH_Time[0];
        break;
    case 2:
        // T16G1RH=T16G1R_S.NumChar[1];  //赋下次过零接收时间
        // T16G1RL=T16G1R_S.NumChar[0];
        T16G2RH=T16g1RH_Time[3];		//过零后6.7ms发
        T16G2RL=T16g1RH_Time[2];
        break;
    case 3:
        //   T16G1RH=T16G1R_S.NumChar[1];  //赋下次过零接收时间
        //   T16G1RL=T16G1R_S.NumChar[0];
        T16G2RH=T16g1RH_Time[5];		//过零后ms发
        T16G2RL=T16g1RH_Time[4];
        break;
    default:
        T16G2RH=T16G1R_S.NumChar[1];  //赋下次过零接收时间
        T16G2RL=T16G1R_S.NumChar[0];
    break;
    }
    T16G2L=10;
    T16G2H=0; 
    T16G2CL=0x11;		//4:1 
}

void Ini_Plc_Rec(void)
{
    PLC_RW=0x02;
  
    PLC_ADDR=PLC_00H_LNA1T0;
    PLC_DATA=0X43;		//0x44
    PLC_ADDR=PLC_01H_LNA1T1;
    PLC_DATA=0X44;  
    PLC_ADDR=PLC_02H_LNA1T2;
    PLC_DATA=0X41;
  
    PLC_ADDR=PLC_03H_LNA2T0;
    PLC_DATA=0X38;		//0x88
    PLC_ADDR=PLC_04H_LNA2T1;
    PLC_DATA=0X88;
    PLC_ADDR=PLC_05H_LNA2T2;
    PLC_DATA=0X04;
  
    PLC_ADDR=PLC_06H_LPFT;
    PLC_DATA=0XB1;
    PLC_ADDR=PLC_07H_PGA1T0;
    PLC_DATA=0X45;		//0x40
    PLC_ADDR=PLC_08H_PGA1T1;
    PLC_DATA=0X18;
  
    PLC_ADDR=PLC_09H_PGA1T2;
    PLC_DATA=0X88;
    PLC_ADDR=PLC_0AH_PGA2T0;
    PLC_DATA=0X1F;		//0x3f
    PLC_ADDR=PLC_0BH_PGA2T1;
    PLC_DATA=0X88;  
  
    PLC_ADDR=PLC_0CH_PGA2T2;
    PLC_DATA=0X01;
    PLC_ADDR=PLC_0DH_ADCT0;
    PLC_DATA=0X80;
    PLC_ADDR=PLC_0EH_ADCT1;
    PLC_DATA=0X88;  
  
    PLC_ADDR=PLC_0FH_ADCT2;
    PLC_DATA=0X88;
    PLC_ADDR=PLC_10H_ADCT3;
    PLC_DATA=0X88;
    PLC_ADDR=PLC_11H_DACT0;
    PLC_DATA=0X13;  
    PLC_ADDR=PLC_12H_PAMPT0;
    PLC_DATA=0X03;  
    PLC_ADDR=PLC_13H_PDT0;
    PLC_DATA=0;
    PLC_ADDR=PLC_14H_PLCT0;
    PLC_DATA=0X02;
    PLC_ADDR=PLC_15H_ADC_D0;
    PLC_DATA=0;  
    PLC_ADDR=PLC_16H_ADC_D1;
    PLC_DATA=0;
    PLC_ADDR=PLC_17H_ADC_DIN;
    PLC_DATA=0; 

    PLC_ADDR=0x20;  
    PLC_DATA=0x33; 
    PLC_ADDR=0x21;  
    PLC_DATA=0x33; 
    PLC_ADDR=0x22;  
    PLC_DATA=0x33; 
    PLC_ADDR=0x23;  
    PLC_DATA=0x33; 
    PLC_ADDR=0x24;  
    PLC_DATA=0x33; 
    PLC_ADDR=0x25;  
    PLC_DATA=0x33;
    PLC_ADDR=0x26;  
    PLC_DATA=0x33; 
    PLC_ADDR=0x27;  
    PLC_DATA=0x33; 
    PLC_ADDR=0x28;  
    PLC_DATA=0x33; 
    PLC_ADDR=0x29;  
    PLC_DATA=0x33; 
    PLC_ADDR=0x2a;  
    PLC_DATA=0x33; 
    PLC_ADDR=0x2b;  
    PLC_DATA=0x33; 
    PLC_ADDR=0x2c;  
    PLC_DATA=0x33;
    PLC_ADDR=0x2d;  
    PLC_DATA=0x33; 
    PLC_ADDR=0x2e;  
    PLC_DATA=0x33; 
  
    PLC_ADDR=0x30;  
    PLC_DATA=0x55; 
    PLC_ADDR=0x31;  
    PLC_DATA=0x55; 
    PLC_ADDR=0x32;  
    PLC_DATA=0x55; 
    PLC_ADDR=0x33;  
    PLC_DATA=0x55; 
    PLC_ADDR=0x34;  
    PLC_DATA=0x55; 
    PLC_ADDR=0x35;  
    PLC_DATA=0x55; 
    PLC_ADDR=0x36;  
    PLC_DATA=0x55; 
    PLC_ADDR=0x37;  
    PLC_DATA=0x55;
    PLC_ADDR=0x38;  
    PLC_DATA=0x55; 
    PLC_ADDR=0x39;  
    PLC_DATA=0x55; 
    PLC_ADDR=0x3a;  
    PLC_DATA=0x55; 
    PLC_ADDR=0x3b;  
    PLC_DATA=0x55;
    PLC_ADDR=0x3c;  
    PLC_DATA=0x55; 
    PLC_ADDR=0x3d;  
    PLC_DATA=0x55; 
    PLC_ADDR=0x3e;  
    PLC_DATA=0x55; 
  
    PLC_ADDR=CSMAS;
    PLC_DATA=0xC8;			//          0xd3:			//5.6mv;  0xb6
  
    PLC_ADDR=CSMAT;
    PLC_DATA=0xc9;			//4MS 未选择基带成型发送,快速刷新
	//PLC_DATA=0x20;			//2MS 选择基带成型发送
  
    PLC_ADDR=PLC_AFEC;
    PLC_DATA=0X30;			//Dac 1.6v(6db)
 
    // PLC_ADDR=DIGTEST;
    // PLC_DATA=0X40;
    PLC_ADDR=FREQ_RX1;
    PLC_DATA=0x02;
    //   PLC_DATA=0x01;
    PLC_ADDR=FREQ_RX0;
    PLC_DATA=0x1b;
    //   PLC_DATA=0xEC;
     
    PLC_ADDR=0x49;
    PLC_DATA=0x06;
    PLC_ADDR=DKG;
    PLC_DATA=0x0B;
    PLC_ADDR=DIGTEST;	 
    //   PLC_DATA=0x00;		// default
    //	PLC_DATA=0x08;			// port B6=psk_RXD
    PLC_DATA=0x04;			// port B7=fsk_RXD	

    PLC_ADDR=MIXFWH;
	PLC_DATA = 0x80;
     
    // PLC_MOD=0x68;			//开接收0X68
    PLC_MOD=0;	
    PLC_RW=0x00; 
}
void Ini_Plc_Tx(void)
{
	
    PLC_RW=0x02;
    PLC_ADDR=DAG;
    PLC_DATA=0x1f;		//0X3F
  
    PLC_ADDR=0x06;
    PLC_DATA=0X31;  //
  
    PLC_ADDR=0x11;
    PLC_DATA=0X33;  //ox13
  
    PLC_ADDR=0x12;
    PLC_DATA=0X13;  //0x03
  
    PLC_ADDR=FREQ_TX1;

    PLC_DATA=0x02;//01
    PLC_ADDR=FREQ_TX0;
    
    PLC_DATA=0x06;//d8
    PLC_ADDR=FREQ_TX2;
    PLC_DATA=0x30;   //06
    PLC_ADDR=FREQ_TX3;
    PLC_DATA=2;
    //FREQ_TX0=0xE1;
 
    // PLC_MOD=0x1f;
    PLC_MOD=0x23;
    //PLC_TXEN=1;
    PLC_RW=0x00;

 // PLC_RW=0x01;
 //PLC_ADDR=DAG;
 //testbyte=PLC_DATA;
 //PLC_RW=0x00;
}

uchar  Read_PlcReg(uchar RegAdd)
{
    uchar ucA;
    PLC_RW=0x01;

    PLC_ADDR=RegAdd;
    ucA=PLC_DATA;
    PLC_RW=0x00;
 
    return(ucA);
}

/************************************
** 函数原型: void Delay(uchar MS); **
** 功    能: 延时time=MS ms.     **
************************************/
void Delay(uint MS)
{
    uint aa;//,ucB;
    uchar bb;
    for(aa=0;aa<MS;aa++) 
    {
        for(bb=0;bb<0xFC;)
        {
            bb++;
            NOP();
        }
        for(bb=0;bb<0xFC;)
        {
            bb++;
            NOP();
        }
        for(bb=0;bb<0xFC;)
        {
            bb++;
            NOP();
        }
        for(bb=0;bb<0xFC;)
        {
            bb++;
            NOP();
        }
        for(bb=0;bb<0xFc;)
        {
            bb++;
        }
    }
}

#define  delay16t()    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP()NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP()
#define  delay8t()    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP()
#define  delay4t()    NOP();NOP();NOP();NOP();
#define  delay14t()   NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP(); NOP();NOP();NOP();NOP();NOP();NOP() 
void delay5us(void)
{
    NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();

    //NOP();NOP();NOP();
    //NOP();NOP();
}
void delay4us(void)
{
    NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();
    // NOP();NOP();NOP();NOP();NOP();
    // NOP();NOP();NOP();NOP();NOP();

    //NOP();NOP();NOP();
    //NOP();NOP();
}
//位发送间隔时间 133.2uS
void DelayBit()	
{
    uchar bb;
    for(bb=94;bb!=0;bb--)		//每次14T
 	{
        delay8t();
 	}
 //  NOP();
 //  NOP();
// NOP();
}
//最后发送的位少延时8条指令
void DelayEndBit()
{
    uchar bb;
    for(bb=65;bb!=0;bb--)		//93改为90
 	{
        delay14t();
 	}
 //	NOP();
 //	NOP();
  	NOP();
}

void Delay2Clock(void)	
{
    uchar bb;
    delay5us();
    delay5us();
  
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
    NOP();
}


/************************************
** 函数原型: void Delay(uchar MS); **
** 功    能: 延时time=MS*4 ms.     **
************************************/
void Delay10ms(void)
{
    uchar ucA,ucB;
    for(ucA=0;ucA<40;ucA++) 
        for(ucB=0;ucB<0xf0;ucB++);
}


void  Sum_DM15(void )
{
    uchar ucS1,ucS0;
    ucS1=Bit1Num_T[1];
    ucS1+=Bit1Num_T[3];
    ucS1+=Bit1Num_T[4];
    ucS1+=Bit1Num_T[7];
    ucS1+=Bit1Num_T[11];
    ucS1+=Bit1Num_T[12];
    ucS1+=Bit1Num_T[13];
    ucS1+=Bit1Num_T[14];
    
    ucS0=Bit1Num_T[0];	
    ucS0+=Bit1Num_T[2];	
    ucS0+=Bit1Num_T[5];	
    ucS0+=Bit1Num_T[6];	
    ucS0+=Bit1Num_T[8];	
    ucS0+=Bit1Num_T[9];	
    ucS0+=Bit1Num_T[10];
     
    ZXDM=ucS1+56-ucS0;
}


void  Sum_DM21_Sync(uchar offset )
{
    uchar ucS1,ucS0;
    switch(offset)
    {
    case 0 :
        ucS1=Bit1Num_T[2];
        ucS1+=Bit1Num_T[4];
        ucS1+=Bit1Num_T[5];
        ucS1+=Bit1Num_T[6];    
        ucS0=Bit1Num_T[0];	
        ucS0+=Bit1Num_T[1];	
        ucS0+=Bit1Num_T[3];
   
        ucS1+=Bit1Num_T[9];
        ucS1+=Bit1Num_T[11];
        ucS1+=Bit1Num_T[12];
        ucS1+=Bit1Num_T[13];
        ucS0+=Bit1Num_T[7];	
        ucS0+=Bit1Num_T[8];	    	
        ucS0+=Bit1Num_T[10];

        ucS1+=Bit1Num_T[16];
        ucS1+=Bit1Num_T[18];
        ucS1+=Bit1Num_T[19];
        ucS1+=Bit1Num_T[20];   
        ucS0+=Bit1Num_T[14];	
        ucS0+=Bit1Num_T[15];	    	
        ucS0+=Bit1Num_T[17];
  
        ZXDM=ucS1+72-ucS0;
        break;
    case 1 :
        ucS1=Bit1Num_T[3];
        ucS1+=Bit1Num_T[5];
        ucS1+=Bit1Num_T[6];
        ucS1+=Bit1Num_T[7];    
        ucS0=Bit1Num_T[1];	
        ucS0+=Bit1Num_T[2];	
        ucS0+=Bit1Num_T[4];
   
        ucS1+=Bit1Num_T[10];
        ucS1+=Bit1Num_T[12];
        ucS1+=Bit1Num_T[13];
        ucS1+=Bit1Num_T[14];
        ucS0+=Bit1Num_T[8];	
        ucS0+=Bit1Num_T[9];	    	
        ucS0+=Bit1Num_T[11];
     
        ucS1+=Bit1Num_T[17];
        ucS1+=Bit1Num_T[19];
        ucS1+=Bit1Num_T[20];
        ucS1+=Bit1Num_T[21];   
        ucS0+=Bit1Num_T[15];	
        ucS0+=Bit1Num_T[16];	    	
        ucS0+=Bit1Num_T[18];
   
        ZXDM=ucS1+72-ucS0;
        break;
    case 2 :
        ucS1=Bit1Num_T[4];   
        ucS1+=Bit1Num_T[6];
        ucS1+=Bit1Num_T[7];
        ucS1+=Bit1Num_T[8];     
        ucS0=Bit1Num_T[2];	
        ucS0+=Bit1Num_T[3];	
        ucS0+=Bit1Num_T[5];
   
        ucS1+=Bit1Num_T[11];
        ucS1+=Bit1Num_T[13];
        ucS1+=Bit1Num_T[14];
        ucS1+=Bit1Num_T[15];
        ucS0+=Bit1Num_T[9];	
        ucS0+=Bit1Num_T[10];	    	
        ucS0+=Bit1Num_T[12];
     
        ucS1+=Bit1Num_T[18];
        ucS1+=Bit1Num_T[20];
        ucS1+=Bit1Num_T[21];
        ucS1+=Bit1Num_T[22];   
        ucS0+=Bit1Num_T[16];	
        ucS0+=Bit1Num_T[17];	    	
        ucS0+=Bit1Num_T[19];
   
        ZXDM=ucS1+72-ucS0;
        break;
    case 3 :
        ucS1=Bit1Num_T[5];   
        ucS1+=Bit1Num_T[7];
        ucS1+=Bit1Num_T[8];
        ucS1+=Bit1Num_T[9];     
        ucS0=Bit1Num_T[3];	
        ucS0+=Bit1Num_T[4];	
        ucS0+=Bit1Num_T[6];

        ucS1+=Bit1Num_T[12];
        ucS1+=Bit1Num_T[14];
        ucS1+=Bit1Num_T[15];
        ucS1+=Bit1Num_T[16];
        ucS0+=Bit1Num_T[10];	
        ucS0+=Bit1Num_T[11];	    	
        ucS0+=Bit1Num_T[13];
     
        ucS1+=Bit1Num_T[19];
        ucS1+=Bit1Num_T[21];
        ucS1+=Bit1Num_T[22];
        ucS1+=Bit1Num_T[23];   
        ucS0+=Bit1Num_T[17];	
        ucS0+=Bit1Num_T[18];	    	
        ucS0+=Bit1Num_T[20];
   
        ZXDM=ucS1+72-ucS0;
        break;
    }
    if(ZXDM>168)
        ZXDM=255-ZXDM;
    FXDM=168-ZXDM;
}

void  Sum_DM21_Sync2(uchar offset )
{
    uchar ucS1,ucS0;
    switch(offset)
    {
    case 0 :
        ucS1=Bit1Num_T[2+24];
        ucS1+=Bit1Num_T[4+24];
        ucS1+=Bit1Num_T[5+24];
        ucS1+=Bit1Num_T[6+24];    
        ucS0=Bit1Num_T[0+24];	
        ucS0+=Bit1Num_T[1+24];	
        ucS0+=Bit1Num_T[3+24];
   
        ucS1+=Bit1Num_T[9+24];
        ucS1+=Bit1Num_T[11+24];
        ucS1+=Bit1Num_T[12+24];
        ucS1+=Bit1Num_T[13+24];
        ucS0+=Bit1Num_T[7+24];	
        ucS0+=Bit1Num_T[8+24];	    	
        ucS0+=Bit1Num_T[10+24];

        ucS1+=Bit1Num_T[16+24];
        ucS1+=Bit1Num_T[18+24];
        ucS1+=Bit1Num_T[19+24];
        ucS1+=Bit1Num_T[20+24];   
        ucS0+=Bit1Num_T[14+24];	
        ucS0+=Bit1Num_T[15+24];	    	
        ucS0+=Bit1Num_T[17+24];
  
        ZXDM=ucS1+72-ucS0;
        break;
    case 1 :
        ucS1=Bit1Num_T[3+24];
        ucS1+=Bit1Num_T[5+24];
        ucS1+=Bit1Num_T[6+24];
        ucS1+=Bit1Num_T[7+24];    
        ucS0=Bit1Num_T[1+24];	
        ucS0+=Bit1Num_T[2+24];	
        ucS0+=Bit1Num_T[4+24];
   
        ucS1+=Bit1Num_T[10+24];
        ucS1+=Bit1Num_T[12+24];
        ucS1+=Bit1Num_T[13+24];
        ucS1+=Bit1Num_T[14+24];
        ucS0+=Bit1Num_T[8+24];	
        ucS0+=Bit1Num_T[9+24];	    	
        ucS0+=Bit1Num_T[11+24];
     
        ucS1+=Bit1Num_T[17+24];
        ucS1+=Bit1Num_T[19+24];
        ucS1+=Bit1Num_T[20+24];
        ucS1+=Bit1Num_T[21+24];   
        ucS0+=Bit1Num_T[15+24];	
        ucS0+=Bit1Num_T[16+24];	    	
        ucS0+=Bit1Num_T[18+24];
   
        ZXDM=ucS1+72-ucS0;
        break;
    case 2 :
        ucS1=Bit1Num_T[4+24];   
        ucS1+=Bit1Num_T[6+24];
        ucS1+=Bit1Num_T[7+24];
        ucS1+=Bit1Num_T[8+24];     
        ucS0=Bit1Num_T[2+24];	
        ucS0+=Bit1Num_T[3+24];	
        ucS0+=Bit1Num_T[5+24];
   
        ucS1+=Bit1Num_T[11+24];
        ucS1+=Bit1Num_T[13+24];
        ucS1+=Bit1Num_T[14+24];
        ucS1+=Bit1Num_T[15+24];
        ucS0+=Bit1Num_T[9+24];	
        ucS0+=Bit1Num_T[10+24];	    	
        ucS0+=Bit1Num_T[12+24];
     
        ucS1+=Bit1Num_T[18+24];
        ucS1+=Bit1Num_T[20+24];
        ucS1+=Bit1Num_T[21+24];
        ucS1+=Bit1Num_T[22+24];   
        ucS0+=Bit1Num_T[16+24];	
        ucS0+=Bit1Num_T[17+24];	    	
        ucS0+=Bit1Num_T[19+24];
   
        ZXDM=ucS1+72-ucS0;
        break;
    case 3 :
        ucS1=Bit1Num_T[5+24];   
        ucS1+=Bit1Num_T[7+24];
        ucS1+=Bit1Num_T[8+24];
        ucS1+=Bit1Num_T[9+24];     
        ucS0=Bit1Num_T[3+24];	
        ucS0+=Bit1Num_T[4+24];	
        ucS0+=Bit1Num_T[6+24];

        ucS1+=Bit1Num_T[12+24];
        ucS1+=Bit1Num_T[14+24];
        ucS1+=Bit1Num_T[15+24];
        ucS1+=Bit1Num_T[16+24];
        ucS0+=Bit1Num_T[10+24];	
        ucS0+=Bit1Num_T[11+24];	    	
        ucS0+=Bit1Num_T[13+24];
     
        ucS1+=Bit1Num_T[19+24];
        ucS1+=Bit1Num_T[21+24];
        ucS1+=Bit1Num_T[22+24];
        ucS1+=Bit1Num_T[23+24];   
        ucS0+=Bit1Num_T[17+24];	
        ucS0+=Bit1Num_T[18+24];	    	
        ucS0+=Bit1Num_T[20+24];
   
        ZXDM=ucS1+72-ucS0;
        break;
    }
    if(ZXDM>168)
        ZXDM=255-ZXDM;
    FXDM=168-ZXDM;
}


sbit Recv_Bit()
{
    if(ZXDM>84)
	{
		//NOP();
	    return(1);
	}
	else 
	{
	    return(0);
	}	   
}

/**************  
*******21PN
*******数据1扩频发送**********/

void TX1_PN21(void)
{ 
    TX_Bit1()      ;//1 		//多发1个无用的 0520
	DelayBit();
    
    TX_Bit1()      ;//1 		//多发1个无用的 
	DelayBit();
    
	TX_Bit0()      ;//0   
	DelayBit();

    TX_Bit0()      ;//0   
	DelayBit();
    
    TX_Bit1()      ;//1    
	DelayBit();
    
    TX_Bit0()      ;//0   
	DelayBit();
    
    TX_Bit1()      ;//1    
	DelayBit();
  	
    TX_Bit1()      ;//1    
	DelayBit();
	
    TX_Bit1()       ;//1  
	DelayBit();
	
    TX_Bit0()       ;//0    
	DelayBit();
  	
    TX_Bit0()       ;//0      
	DelayBit();
  	
    TX_Bit1()       ;//1  
	DelayBit();
    
    TX_Bit0()      ;//0    
	DelayBit();
  	
    TX_Bit1()       ;//1  
	DelayBit();
  	
    TX_Bit1()       ;//1   
	DelayBit();
  	
    TX_Bit1()       ;//1    
	DelayBit();
	
    TX_Bit0()      ;//0   
	DelayBit();
    
    TX_Bit0()      ;//0   
	DelayBit();
    
    TX_Bit1()      ;//1    
	DelayBit();
    
    TX_Bit0()      ;//1   
	DelayBit();
    
    TX_Bit1()      ;//1    
	DelayBit();
  	
    TX_Bit1()      ;//1    
	DelayBit();
	
    TX_Bit1()       ;//1  
	DelayBit();//DelayEndBit();//1303
}

/**************  
*******21PN   两位间发送间隔1332T
*******数据0扩频发送**********/

void TX0_PN21(void)
{   
    TX_Bit0()      ;//1    多发1BIT 0520
	DelayBit();
    
    TX_Bit0()      ;//1    多发1BIT
	DelayBit();
    
    TX_Bit1()      ;//1   
	DelayBit();
    
    TX_Bit1()      ;//1  
	DelayBit();
    
    TX_Bit0()      ;//0    
	DelayBit();
    
    TX_Bit1()      ;//1   
	DelayBit();
    
    TX_Bit0()      ;//0    
	DelayBit();
  	
    TX_Bit0()      ;//0    
	DelayBit();
	
    TX_Bit0()       ;//0  
	DelayBit();
	
    TX_Bit1()      ;//0   
	DelayBit();
    
    TX_Bit1()      ;//0   
	DelayBit();
    
    TX_Bit0()      ;//1    
	DelayBit();
    
    TX_Bit1()      ;//1   
	DelayBit();
    
    TX_Bit0()      ;//1    
	DelayBit();
  	
    TX_Bit0()      ;//0    
	DelayBit();
	
    TX_Bit0()       ;//0  
	DelayBit();  
	
    TX_Bit1()      ;//0   
	DelayBit();
    
    TX_Bit1()      ;//0   
	DelayBit();
    
    TX_Bit0()      ;//1    
	DelayBit();
    
    TX_Bit1()      ;//1   
	DelayBit();
    
    TX_Bit0()      ;//1    
	DelayBit();
  	
    TX_Bit0()      ;//0    
	DelayBit();
	
    TX_Bit0()       ;//0  	
	DelayBit();;//DelayEndBit();//1303
}
//位间隔1998US
void tx_normal_data(void)   /*发送正常数据*/
{
	
	if(Plc_data_bit_cnt==0)   /*此字节发送完*/
	{	
        Plc_data_byte_cnt++;
	   	
		Plc_data_bit_cnt=8;        /*应发送8位,先减一后发送,所以为9;*/
		if(t_nor_bit==1)
		{
            plc_byte_data=plc_data[Plc_data_byte_cnt]; /*取要发送的数据,有0x09af*/
		        
			if(Plc_data_byte_cnt==Plc_data_len)   /*全部发送完*/
			{
                t_nor_bit=0;
			   	t_end_bit=1;
			   	S_LED=0;			//最后一位扩频位尚未发送
			  	return;		
			}
		}
		else
		{
            plc_byte_data=0xff;
		    if(Plc_data_byte_cnt==3) 
		    {
                t_nor_bit=1;
		        Plc_data_byte_cnt=0;
		        plc_byte_data=plc_data[Plc_data_byte_cnt];
		    }
		}		
	}
	else if (Plc_data_bit_cnt != 8)
	{
        plc_byte_data<<=1;	
	}
	if(plc_byte_data&0x80) 
        Plc_Tx_Bit=1;		
	else
        Plc_Tx_Bit=0;	 

    Plc_data_bit_cnt--;	 /*还应发送几位*/
   
 }
	
   
/*******************接收载波数据******************/
void rcv_normal_data(void)   /*正常接收*/
{
	uchar i;
	Plc_data_bit_cnt--;     	/*还剩几位*/
    
    if((Plc_data_byte_cnt+1)==1)
    {
        if (Plc_data_bit_cnt==0)
        NOP();
    }
	if(Plc_data_bit_cnt!=0)	/*未接收完一个字节*/
		return;
	
	Plc_data_bit_cnt=8;
	plc_data[Plc_data_byte_cnt]=plc_byte_data;              /*存储一个字节*/
	Plc_data_byte_cnt++;

	if(Plc_data_byte_cnt==1) 
	{// Plc_data_len=plc_data[6]+8;
        if(plc_data[0]>=MaxPlcL) 
	  	{
		plc_restart();
            return;
		}
	}	
	else	
	{ 
      if((Plc_data_byte_cnt+1)==9)
        NOP();
		if((Plc_data_byte_cnt+1)>=plc_data[0])// plc_data[1])       /*如果全部接收完*/
		{
            Rx_status='R';
			r_sync_bit=0;
			plc_byte_data=0;
			R_LED = 1;
			plc_recv_finished();
		}
	}	/*还未接收完*/
	//Plc_data_bit_cnt=8;
}


void Recv_Plc_8Bit(void)		//1332t采样8次,166T和167间隔采样
{
	uchar ucA;
	Plc_Samples_byte=0;	
	Plc_Samples_bit1_cnt=0;	
	delay5us();
	delay14t();
	delay14t();
	   
	Plc_Samples_byte<<=1;
	if(PLC_FSK_RXD) 
	{	
        Plc_Samples_byte|=0x01;
	 	Plc_Samples_bit1_cnt++;
    } 
    else
    {
        NOP();
        NOP();
        NOP();
    }
	delay5us();     	
	delay5us();
	delay5us();
	NOP();
	//   NOP();
	Plc_Samples_byte<<=1;
	if(PLC_FSK_RXD) 
	{	
        Plc_Samples_byte|=0x01;
	 	Plc_Samples_bit1_cnt++;
    } 
    else
    {
        NOP();
        NOP();
        NOP();
    }
    delay5us();     	
	delay5us();
	delay5us();
	NOP();
	NOP();
	Plc_Samples_byte<<=1;
	if(PLC_FSK_RXD)
	{
        Plc_Samples_byte|=0x01;
	 	Plc_Samples_bit1_cnt++;
    } 
    else
    {
        NOP();
        NOP();
        NOP();
    }
    delay5us();     	
	delay5us();
	delay5us();
	NOP();
	//   NOP();
	Plc_Samples_byte<<=1;
	if(PLC_FSK_RXD)
	{	
        Plc_Samples_byte|=0x01;
	  	Plc_Samples_bit1_cnt++;
    } 
    else
    {
        NOP();
        NOP();
        NOP();
    }
    delay5us();     	
	delay5us();
	delay5us();
	NOP();
	NOP();
	Plc_Samples_byte<<=1;
	if(PLC_FSK_RXD)
	{	
        Plc_Samples_byte|=0x01;
	 	Plc_Samples_bit1_cnt++;
    } 
    else
    {
        NOP();
        NOP();
       	NOP();
    }
    delay5us();     	
	delay5us();
	delay5us();
	NOP();
	//   NOP();
	Plc_Samples_byte<<=1;
	if(PLC_FSK_RXD)
	{	
        Plc_Samples_byte|=0x01;
	 	Plc_Samples_bit1_cnt++;
    } 
    else
    {
        NOP();
        NOP();
        NOP();
    }
    delay5us();     	
	delay5us();
	delay5us();
	NOP();
	NOP();
	Plc_Samples_byte<<=1;
	if(PLC_FSK_RXD)
	{	
        Plc_Samples_byte|=0x01;
	 	Plc_Samples_bit1_cnt++;
    } 
    else
    {
        NOP();
        NOP();
        NOP();
    }
    delay5us();     	
	delay5us();
	delay5us();
	NOP();
	Plc_Samples_byte<<=1;
	if(PLC_FSK_RXD)
	{
        Plc_Samples_byte|=0x01;
	 	Plc_Samples_bit1_cnt++;
    } 
    else
    {
        NOP();
        NOP();
        NOP();
    }
    delay14t();
    NOP();
	NOP();
    NOP();
    NOP();
    NOP();
    NOP();
}

void Recv_Plc_78Bit(void)		//1332t采样8次,166T和167间隔采样
{
	uchar ucA;
	Plc_Samples_byte=0;	
	Plc_Samples_bit1_cnt=0;	
	delay5us();
	delay14t();
	delay14t();
	   
	Plc_Samples_byte<<=1;
	if(PLC_FSK_RXD) 
	{	
        Plc_Samples_byte|=0x01;
	 	Plc_Samples_bit1_cnt++;
    } 
    else
    {
        NOP();
        NOP();
        NOP();
    }
	delay5us();     	
	delay5us();
	delay5us();
	NOP();
	//   NOP();
	Plc_Samples_byte<<=1;
	if(PLC_FSK_RXD) 
	{	
        Plc_Samples_byte|=0x01;
	 	Plc_Samples_bit1_cnt++;
    } 
    else
    {
        NOP();
        NOP();
        NOP();
    }
    delay5us();     	
	delay5us();
	delay5us();
	NOP();
	NOP();
	Plc_Samples_byte<<=1;
	if(PLC_FSK_RXD)
	{
        Plc_Samples_byte|=0x01;
	 	Plc_Samples_bit1_cnt++;
    } 
    else
    {
        NOP();
        NOP();
        NOP();
    }
    delay5us();     	
	delay5us();
	delay5us();
	NOP();
	//   NOP();
	Plc_Samples_byte<<=1;
	if(PLC_FSK_RXD)
	{	
        Plc_Samples_byte|=0x01;
	  	Plc_Samples_bit1_cnt++;
    } 
    else
    {
        NOP();
        NOP();
        NOP();
    }
    delay5us();     	
	delay5us();
	delay5us();
	NOP();
	NOP();
	Plc_Samples_byte<<=1;
	if(PLC_FSK_RXD)
	{	
        Plc_Samples_byte|=0x01;
	 	Plc_Samples_bit1_cnt++;
    } 
    else
    {
        NOP();
        NOP();
       	NOP();
    }
    delay5us();     	
	delay5us();
	delay5us();
	NOP();
	//   NOP();
	Plc_Samples_byte<<=1;
	if(PLC_FSK_RXD)
	{	
        Plc_Samples_byte|=0x01;
	 	Plc_Samples_bit1_cnt++;
    } 
    else
    {
        NOP();
        NOP();
        NOP();
    }
    delay5us();     	
	delay5us();
	delay5us();
	NOP();
	NOP();
	Plc_Samples_byte<<=1;
	if(PLC_FSK_RXD)
	{	
        Plc_Samples_byte|=0x01;
	 	Plc_Samples_bit1_cnt++;
    } 
    else
    {
        NOP();
        NOP();
        NOP();
    }

}

void  Recv_25Pn(void)
{

#if 1
    STA=0;
    //* 0512
    Recv_Plc_78Bit();				
	Bit1Num_T[0]=Plc_Samples_bit1_cnt;	//1的个数
	BitData_T[0]=Plc_Samples_byte;	
	
       delay14t();
    	delay14t();
    	delay14t();
    	NOP();
       NOP();
        NOP();        
#endif	
    //*     0512
	Recv_Plc_8Bit();				
	Bit1Num_T[0]=Plc_Samples_bit1_cnt;	//1的个数
	BitData_T[0]=Plc_Samples_byte;	
	delay14t();
	delay14t();
	delay14t();
	NOP();
    NOP();
    NOP();
             
	Recv_Plc_8Bit();				
	Bit1Num_T[1]=Plc_Samples_bit1_cnt;	//1的个数
	BitData_T[1]=Plc_Samples_byte;	
	delay14t();
	delay14t();
	delay14t();
	NOP();
	NOP();
	NOP();
	  
	Recv_Plc_8Bit();				
	Bit1Num_T[2]=Plc_Samples_bit1_cnt;	//1的个数
	BitData_T[2]=Plc_Samples_byte;	
	delay14t();
	delay14t();
	delay14t();
	NOP();
    NOP();
    NOP();
	
    Recv_Plc_8Bit();				
	Bit1Num_T[3]=Plc_Samples_bit1_cnt;	//1的个数
	BitData_T[3]=Plc_Samples_byte;	
	delay14t();
	delay14t();
	delay14t();
	NOP();
    NOP();
    NOP();
	
    Recv_Plc_8Bit();				
	Bit1Num_T[4]=Plc_Samples_bit1_cnt;	//1的个数
	BitData_T[4]=Plc_Samples_byte;	
	delay14t();
	delay14t();
	delay14t();
	NOP();
    NOP();
    NOP();
	  	
    Recv_Plc_8Bit();				
    Bit1Num_T[5]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[5]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
       
    Recv_Plc_8Bit();				
    Bit1Num_T[6]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[6]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
    
    Recv_Plc_8Bit();				
    Bit1Num_T[7]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[7]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
    
    Recv_Plc_8Bit();				
    Bit1Num_T[8]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[8]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
    
    Recv_Plc_8Bit();				
    Bit1Num_T[9]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[9]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
    
    Recv_Plc_8Bit();				
    Bit1Num_T[10]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[10]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
    
    Recv_Plc_8Bit();				
    Bit1Num_T[11]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[11]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
    
    Recv_Plc_8Bit();				
    Bit1Num_T[12]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[12]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
    
    Recv_Plc_8Bit();				
    Bit1Num_T[13]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[13]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
        
    Recv_Plc_8Bit();				
    Bit1Num_T[14]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[14]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t(); 
    NOP();
    NOP();
    NOP();
        
    Recv_Plc_8Bit();				
    Bit1Num_T[15]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[15]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
             
    Recv_Plc_8Bit();				
    Bit1Num_T[16]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[16]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
             
    Recv_Plc_8Bit();				
    Bit1Num_T[17]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[17]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
            
    Recv_Plc_8Bit();				
    Bit1Num_T[18]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[18]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
     
    Recv_Plc_8Bit();				
    Bit1Num_T[19]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[19]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    // delay14t();
    PLC_RW=0x01; 	 	
    PLC_ADDR=0x51;
    RSSIV=PLC_DATA; 
    PLC_RW=0x00;
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    
    Recv_Plc_8Bit();				
    Bit1Num_T[20]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[20]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
      
    Recv_Plc_8Bit();				
    Bit1Num_T[21]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[21]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
    
    Recv_Plc_8Bit();				
    Bit1Num_T[22]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[22]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
     
    Recv_Plc_8Bit();				
    Bit1Num_T[23]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[23]=Plc_Samples_byte;
#if 0	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
     
    Recv_Plc_8Bit();				
    Bit1Num_T[24]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[24]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
     
    Recv_Plc_8Bit();				
    Bit1Num_T[25]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[25]=Plc_Samples_byte;	
    //STA=1;	 
    //RSSIBit_buf[Plc_data_bit_cnt-1]=RSSIV; 
#endif	
}


void  Recv_sec_25Pn(void)
{
#if 0
    STA=0;
    //* 0512
    Recv_Plc_8Bit();				
	Bit1Num_T[24]=Plc_Samples_bit1_cnt;	//1的个数
	BitData_T[26]=Plc_Samples_byte;	
	
    delay14t();
	delay14t();
	delay14t();
	NOP();
    NOP();
    NOP();
#endif	
    //*     0512
	Recv_Plc_8Bit();				
	Bit1Num_T[24]=Plc_Samples_bit1_cnt;	//1的个数
	BitData_T[24]=Plc_Samples_byte;	
	delay14t();
	delay14t();
	delay14t();
	NOP();
    NOP();
    NOP();
             
	Recv_Plc_8Bit();				
	Bit1Num_T[1+24]=Plc_Samples_bit1_cnt;	//1的个数
	BitData_T[1+24]=Plc_Samples_byte;	
	delay14t();
	delay14t();
	delay14t();
	NOP();
	NOP();
	NOP();
	  
	Recv_Plc_8Bit();				
	Bit1Num_T[2+24]=Plc_Samples_bit1_cnt;	//1的个数
	BitData_T[2+24]=Plc_Samples_byte;	
	delay14t();
	delay14t();
	delay14t();
	NOP();
    NOP();
    NOP();
	
    Recv_Plc_8Bit();				
	Bit1Num_T[3+24]=Plc_Samples_bit1_cnt;	//1的个数
	BitData_T[3+24]=Plc_Samples_byte;	
	delay14t();
	delay14t();
	delay14t();
	NOP();
    NOP();
    NOP();
	
    Recv_Plc_8Bit();				
	Bit1Num_T[4+24]=Plc_Samples_bit1_cnt;	//1的个数
	BitData_T[4+24]=Plc_Samples_byte;	
	delay14t();
	delay14t();
	delay14t();
	NOP();
    NOP();
    NOP();
	  	
    Recv_Plc_8Bit();				
    Bit1Num_T[5+24]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[5+24]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
       
    Recv_Plc_8Bit();				
    Bit1Num_T[6+24]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[6+24]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
    
    Recv_Plc_8Bit();				
    Bit1Num_T[7+24]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[7+24]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
    
    Recv_Plc_8Bit();				
    Bit1Num_T[8+24]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[8+24]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
    
    Recv_Plc_8Bit();				
    Bit1Num_T[9+24]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[9+24]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
    
    Recv_Plc_8Bit();				
    Bit1Num_T[10+24]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[10+24]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
    
    Recv_Plc_8Bit();				
    Bit1Num_T[11+24]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[11+24]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
    
    Recv_Plc_8Bit();				
    Bit1Num_T[12+24]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[12+24]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
    
    Recv_Plc_8Bit();				
    Bit1Num_T[13+24]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[13+24]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
        
    Recv_Plc_8Bit();				
    Bit1Num_T[14+24]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[14+24]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t(); 
    NOP();
    NOP();
    NOP();
        
    Recv_Plc_8Bit();				
    Bit1Num_T[15+24]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[15+24]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
             
    Recv_Plc_8Bit();				
    Bit1Num_T[16+24]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[16+24]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
             
    Recv_Plc_8Bit();				
    Bit1Num_T[17+24]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[17+24]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
            
    Recv_Plc_8Bit();				
    Bit1Num_T[18+24]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[18+24]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
     
    Recv_Plc_8Bit();				
    Bit1Num_T[19+24]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[19+24]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    // delay14t();
    PLC_RW=0x01; 	 	
    PLC_ADDR=0x51;
    RSSIV=PLC_DATA; 
    PLC_RW=0x00;
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    
    Recv_Plc_8Bit();				
    Bit1Num_T[20+24]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[20+24]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
      
    Recv_Plc_8Bit();				
    Bit1Num_T[21+24]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[21+24]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
    
    Recv_Plc_8Bit();				
    Bit1Num_T[22+24]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[22+24]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
     
    Recv_Plc_8Bit();				
    Bit1Num_T[23+24]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[23+24]=Plc_Samples_byte;	
#if 0	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
     
    Recv_Plc_8Bit();				
    Bit1Num_T[24+26]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[24+26]=Plc_Samples_byte;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
     
    Recv_Plc_8Bit();				
    Bit1Num_T[25+26]=Plc_Samples_bit1_cnt;	//1的个数
    BitData_T[25+26]=Plc_Samples_byte;	
    STA=1;	 
    RSSIBit_buf[Plc_data_bit_cnt-1]=RSSIV;  	
#endif	
}




void Zero_Time_adjust()
{
    uchar ucA,ucB;
    uint uiT_20ms;
    //  T16G1R_S.NumLong=0;
  
 	if(T16G1R_new.NumInt<T16G1R_old.NumInt)
 	{
 	    uiT_20ms=65536-T16G1R_old.NumInt;
 	    uiT_20ms+=T16G1R_new.NumInt;
 	}
 	else
 	    uiT_20ms=T16G1R_new.NumInt-T16G1R_old.NumInt;
 	
 	if((uiT_20ms>53500)||(uiT_20ms<46500))
 	{
 	    uiT_20ms=0; 	 
 	}
 	else 
 	{
 	    T16G1R_S.NumLong=0;
 		Zero_cnt++;
 		//
 		T16G1R_20ms[Zero_cnt].NumInt=uiT_20ms;
 	  
		if(Zero_cnt>=Zero_Num)		
 		{
 		    Zero_cnt=Zero_Num-1;
 		    for(ucA=0;ucA<Zero_Num;ucA++)
 			{
 			   T16G1R_20ms[ucA].NumInt=T16G1R_20ms[ucA+1].NumInt;
 			   T16G1R_S.NumLong+=T16G1R_20ms[ucA+1].NumInt;
 			}
 		    if((T16G1R_S.NumChar[0]&0X03)<3)			//四舍5入处理
 		        T16G1R_S.NumLong=T16G1R_S.NumLong/Zero_Num;		//每次过零用的时间T0
 		    else
 		    {
 		        T16G1R_S.NumLong=T16G1R_S.NumLong/Zero_Num;
 		        T16G1R_S.NumLong++;
 		    }
 		    
 		    if(Plc_ZeroMode!=1)
 		        Plc_ZeroMode=1;
 		}
 	}
}
void T16G1Int_Proc(void)
{
    if(Plc_Mode==0)
    {  
        if(Plc_ZeroMode==1)
        {      	
            IniT16G2(2);
            T16G2IF=0;
            T16G2IE=1;
            Work_step=1; 
        }
    }
    else if(Plc_Mode=='T' && Plc_ZeroMode == 1)
    {
        if(Work_step!=2)
        {
            IniT16G2(2);
            T16G2IF=0;
            T16G2IE=1;
            Work_step=1; 
        }
        else
        { //T16G1IE=0;
        }
    }	

   	
    T16G1R_old.NumInt=T16G1R_new.NumInt;
    T16G1R_new.NumChar[0]=T16G1RL;
    T16G1R_new.NumChar[1]=T16G1RH; 	
    Zero_Time_adjust();
	//  Zero_cnt=0;
	Work_step=0;
	//	   Plc_Mode='R';
}

void T16G2Int_Proc(void)
{	
	//if (Rec_Zero_bz) //for debug
    //PB5 = 1;
    if(Plc_Mode=='T')
    {
    //        Uart_rec1Point=0;
        if(Work_step==2)
        {
            PLC_MOD=0x23;
            PLC_TX;
            if(t_end_bit==0)  //
		    {
		    	trans_step_next();
		        if(Plc_Tx_first_Bit)
		            TX0_PN21();
		        else
		            TX1_PN21();
		        
				DelayBit();
				DelayBit();
				DelayBit();
				//DelayBit();
				//DelayBit();
				
		        if(Plc_Tx_Second_Bit)
		            TX0_PN21();
		        else
		            TX1_PN21();
		        
			tx_normal_data();
			Plc_Tx_first_Bit = Plc_Tx_Bit;
			if(t_end_bit==0){
         			tx_normal_data();
			        Plc_Tx_Second_Bit= Plc_Tx_Bit;
         }
	
				
		        PLC_MOD=0x00;
		        //T16G2RH=T16G1R_S.NumChar[1];  //赋下次过零接收时间
                //T16G2RL=T16G1R_S.NumChar[0];
		    }
            if(t_end_bit)
            {
                Plc_Mode=0;
           	
           	    t_nor_bit=0;
           	    Plc_data_len=0;
           	    R_LED=1;
                //     	 T16G1IF=0;			//0802
                T16G1IE=1;
                 
		        T16G2IE=0;
	            PLC_MOD=0X59;
            }
            PLC_RX;
 	
 	        //    PLC_MOD=0;
     
	    }
	    else // if(Work_step==1)		//
	    {
	        //Ini_Plc_Tx();
	        Work_step=2;
            PLC_MOD=0x23;
            //  PLC_RW=0x00;
            PLC_TX;
        
	        //T16G2RH=T16G1R_S.NumChar[1];  //赋下次过零接收时间
            //T16G2RL=T16G1R_S.NumChar[0];
			trans_step_next();
		        if(Plc_Tx_first_Bit)
		            TX0_PN21();
		        else
		            TX1_PN21();			
			//delay14t();
            //PLC_RX;
			DelayBit();
			DelayBit();
			DelayBit();
			//DelayBit();
			//DelayBit();
		        if(Plc_Tx_Second_Bit)
		            TX0_PN21();
		        else
		            TX1_PN21();

			tx_normal_data();
			Plc_Tx_first_Bit = Plc_Tx_Bit;
			
			tx_normal_data();
			Plc_Tx_Second_Bit= Plc_Tx_Bit;			

			delay14t();

		
            PLC_RX;
			
 	        //    	Ini_Plc_Rec();
 	        PLC_MOD=0;
 	        T16G1IE=0;
	        //  T16G1IF=0;		//0802
            //   T16G1CL=0x21;		//
        }
    }
    else  if(Plc_Mode=='R')
    {
        Rec_Zero_bz=1; 
		trans_step_next();
		//DelayBit();
        if(Work_step==2)
        {
            PLC_MOD=0x59;
         
   	        Recv_25Pn();
            DelayBit();
			DelayBit();
		 	Recv_sec_25Pn();   	
   	        PLC_MOD=0x00;		//0720
        }
   	    else // if(Work_step==1)		//
	    {
	        //Ini_Plc_Tx();
            PLC_MOD=0x59;
            //   PLC_RX;
            Recv_25Pn();
			DelayBit();
            DelayBit();
		    Recv_sec_25Pn(); 			
            PLC_MOD=0x00;       //0720
            Work_step=2;
	   
            //   PLC_MOD=0x00;
 	        //    	Ini_Plc_Rec();
            //	  PLC_MOD=0x59;
            //   T16G1CL=0x21;		//
        }
        //T16G2RH=T16G1R_S.NumChar[1];  //赋下次过零接收时间
        //T16G2RL=T16G1R_S.NumChar[0];
    }
    else  if(Plc_Mode==0)
    {     
        PLC_MOD=0x59;
		trans_step_next();
        //   PLC_RX;
        Recv_25Pn();
		DelayBit();
		DelayBit();
		//DelayBit();
		//DelayBit();
		//DelayBit();
//		DelayBit();
		Recv_sec_25Pn(); 
        //T16G2RH=T16G1R_S.NumChar[1];  //赋下次过零接收时间
        //T16G2RL=T16G1R_S.NumChar[0];
         
        PLC_MOD=0x00;    //0720
        Plc_Mode='F';  
        Rec_Zero_bz=1;
		T16G1IE=0;
		Sync_Step = 0;
		//	T16G1IF=0;	//0802
        //  Work_step=2;
        //    Rec_Zero_bz=1;
    }
    else  if(Plc_Mode=='F')
    {
        PLC_MOD=0x59;
        //   PLC_RX;
       
	    //  T16G2RH=T16G1R_S.NumChar[1];  //赋下次过零接收时间
        //  T16G2RL=T16G1R_S.NumChar[0];
        trans_step_next();
        Recv_25Pn(); 
		DelayBit();
		DelayBit();
		//DelayBit();
		//DelayBit();
		//DelayBit();
//		DelayBit();
		Recv_sec_25Pn(); 		
        PLC_MOD=0x00;		//0720
        Plc_Mode='F';     
        Rec_Zero_bz=1;   
        //  Work_step=2;
        //    Rec_Zero_bz=1;
    }
    
    if(T16G1IF)
    {   	
   	    T16G1R_old.NumInt=T16G1R_new.NumInt;
   	    T16G1R_new.NumChar[0]=T16G1RL;
   	    T16G1R_new.NumChar[1]=T16G1RH;
   	
   	    //Zero_cnt++; 
	    Zero_Time_adjust();
		T16G1IF=0;   	
   	}
	//PB5  = 0;
}


void  INTS(void) interrupt 
{	
	
	if((T16G2IF)&&(T16G2IE))  
	{ 
	    T16G2Int_Proc();
	    T16G2IF=0;	   
	}
	if((T16G1IF)&&(T16G1IE))  
	{  
	    T16G1Int_Proc();
	    T16G1IF=0;
	}
 }
 

void Sync_offset(uchar off)
{
switch(off)
{
	case 0:
		do_left_shift_1bit(0);
	case 1:
		do_left_shift_1bit(1);
	case 2:
		do_left_shift_1bit(2);
	default:
		do_left_shift_1bit(3);
		do_left_shift_1bit(4);
		do_left_shift_1bit(5);
		do_left_shift_1bit(6);
		do_left_shift_1bit(7);
		do_left_shift_1bit(8);
		do_left_shift_1bit(9);
		do_left_shift_1bit(10);
		do_left_shift_1bit(11);
		do_left_shift_1bit(12);
		do_left_shift_1bit(13);
		do_left_shift_1bit(14);
		do_left_shift_1bit(15);
		do_left_shift_1bit(16);
		do_left_shift_1bit(17);
		do_left_shift_1bit(18);
		do_left_shift_1bit(19);
		do_left_shift_1bit(20);
		do_left_shift_1bit(21);
		do_left_shift_1bit(22);
		break;
}
}


void Sync_adjust(void)
{
    uchar ucB,ucC;
    for(ucB=0;ucB<SYCl_offset;ucB++) 				//约13335T
    {
    #if 0
        for(ucC=SYM_offset;ucC<23;ucC++)
        {        
            BitData_T[ucC]<<=1;
            if(BitData_T[ucC+1]&0x80) 
            {
                BitData_T[ucC]++;		//下一字节的高位移到当前的最低位
	            Bit1Num_T[ucC]++;
                Bit1Num_T[ucC+1]--;
            }  
        }
	#else
			if (0 == SYM_offset)
				Sync_offset(0);
			if (1 == SYM_offset)
				Sync_offset(1);
			if (2 == SYM_offset)
				Sync_offset(2);
			else
				Sync_offset(2);	
	#endif
    }
}

void Sync_offset2(uchar off)
{
	switch(off)
	{
		case 0:
			do_left_shift_1bit(0+24);
		case 1:
			do_left_shift_1bit(1+24);
		case 2:
			do_left_shift_1bit(2+24);
		default:
			do_left_shift_1bit(3+24);
			do_left_shift_1bit(4+24);
			do_left_shift_1bit(5+24);
			do_left_shift_1bit(6+24);
			do_left_shift_1bit(7+24);
			do_left_shift_1bit(8+24);
			do_left_shift_1bit(9+24);
			do_left_shift_1bit(10+24);
			do_left_shift_1bit(11+24);
			do_left_shift_1bit(12+24);
			do_left_shift_1bit(13+24);
			do_left_shift_1bit(14+24);
			do_left_shift_1bit(15+24);
			do_left_shift_1bit(16+24);
			do_left_shift_1bit(17+24);
			do_left_shift_1bit(18+24);
			do_left_shift_1bit(19+24);
			do_left_shift_1bit(20+24);
			do_left_shift_1bit(21+24);
			do_left_shift_1bit(22+24);
			break;
		}
}




void Sync_adjust2(void)
{
    uchar ucB,ucC;
    for(ucB=0;ucB<SYCl_offset2;ucB++) 				//约13335T
    {
    #if 0
        for(ucC=SYM_offset2+24;ucC<23+24;ucC++)
        {       
            BitData_T[ucC]<<=1;
            if(BitData_T[ucC+1]&0x80) 
            {
                BitData_T[ucC]++;		//下一字节的高位移到当前的最低位
	            Bit1Num_T[ucC]++;
                Bit1Num_T[ucC+1]--;
            }  
        }
		#else
		if (0 == SYM_offset2)
			Sync_offset2(0);
		if (1 == SYM_offset2)
			Sync_offset2(1);
		if (2 == SYM_offset2)
			Sync_offset2(2);
		else
			Sync_offset2(2);

		#endif		
    }
}
int16u bitcnt = 0;
void _Plc_RecvProc(uchar offset)
{
    //if(1/*Sync_Step=='E'*/)
    if (offset == 0) {
		Sync_adjust();	
		Sum_DM21_Sync(SYM_offset);
       }else {
		Sync_adjust2();	
		Sum_DM21_Sync2(SYM_offset2);
	 }
	sync_word_l<<=1;	
	if(plc_byte_data&0x80)
	{	
	    sync_word_l|=0x01;
	}
	plc_byte_data<<=1;   
    if(Recv_Bit())
        plc_byte_data|=0x01;

    //ZXDM_buf[Sync_bit1_cnt]=ZXDM;
    //RSSIV_buf[Sync_bit1_cnt]=RSSIV;
    if(r_sync_bit==1)    //如果收到桢同步信号09
	{
     bitcnt ++;
	    rcv_normal_data();    //
	 	return;
	}
   Sync_bit1_cnt++;
   if (Sync_bit1_cnt >= 16)
   NOP();
	if(((plc_byte_data&0x01)==0)&&(sync_word_l!=0xff))
	{
		plc_restart();
		R_LED=0;

		return;
	}
	if((plc_byte_data==0xaa)&&(sync_word_l==0xff)) //Sync_Char
	{
	    Plc_data_bit_cnt=8;  //收到位数标志置为9,除此位是先减一后接收外,其它都是先接收后减一,所以其它应为8;
		r_sync_bit=1;   //收到桢同步标志置1
									//	r_func1_bit=0;	//准备收第一个字节FUNC
		Plc_data_byte_cnt=0;
        bitcnt = 0;
		//R_LED=1;
	}
	else {
	    if(Sync_bit1_cnt>=Sync_bit1_cnt_Max)
		{
			plc_restart();
		    R_LED=0;
		    IniT16G1(CCPMODE);
		}
	}        
}

union SVR_INT_B08 t1, t2;
int test;

void Plc_RecvProc()
{
	// T16G2IE = 0;
	//	  t1.NumChar[0]=T16G1L;
	//	  t1.NumChar[1] = T16G1H; 
//test = 0xaa;
	_Plc_RecvProc(0);
	if (Sync_Step == 'E')
		_Plc_RecvProc(24);
   else
    NOP();
	
	//  t2.NumChar[0]=T16G1L;
	//   t2.NumChar[1] = T16G1H; 
	//  test = t2.NumInt - t1.NumInt;
	 //T16G2IE = 1; 	
}


//相位同步法1,每次移1BIT,移7次，每次都要按15个字节按字节移15次；共计算8*15=120次
void _Sync1_Proc()
{
    uchar ucA,ucB,ucC;

    Sum_Max=0;
    Sum_FXMax=0;  //0720
    
    SYM_offset=0;
    SYCl_offset=0;

    //BitData_T[23+offset]=BitData_T[0+offset];
    for(ucB=0;ucB<8;ucB++) 				//约13335T
    {
        for(ucA=0;ucA<3;ucA++)
    	{
            Sum_DM21_Sync(ucA);
	       	if(ZXDM>Sum_Max)
	        	{
	  	       	Sum_Max=ZXDM;
	  	        	SYM_offset=ucA;
	  	        	SYCl_offset=ucB;
	        	}					
    		
            //    else 6 nop 
	        if(FXDM>Sum_FXMax)
	        {
	  	        Sum_FXMax=FXDM;	  		  	
	        }
    	}
#if 0
        for(ucC=0;ucC<23+0;ucC++)
        {
            BitData_T[ucC]<<=1;
            if(BitData_T[ucC+1]&0x80) 
            {    	
                BitData_T[ucC]++;		//下一字节的高位移到当前的最低位
	            Bit1Num_T[ucC]++;
                Bit1Num_T[ucC+1]--;
            }
			
            //  else
            //  {  //16 NOP();
          
            //  } 
        }
#else
	{
		do_left_shift_1bit(0);
		do_left_shift_1bit(1);
		do_left_shift_1bit(2);
		do_left_shift_1bit(3);
		do_left_shift_1bit(4);
		do_left_shift_1bit(5);
		do_left_shift_1bit(6);
		do_left_shift_1bit(7);
		do_left_shift_1bit(8);
        	do_left_shift_1bit(9);
        	do_left_shift_1bit(10);
        	do_left_shift_1bit(11);
        	do_left_shift_1bit(12);
        	do_left_shift_1bit(13);
        	do_left_shift_1bit(14);
        	do_left_shift_1bit(15);
        	do_left_shift_1bit(16);
        	do_left_shift_1bit(17);
        	do_left_shift_1bit(18);
        	do_left_shift_1bit(19);
        	do_left_shift_1bit(20);
        	do_left_shift_1bit(21);
        	do_left_shift_1bit(22);
        //OFF_do(23);
    	}
#endif		
    }
 
    if(Sync_Step=='C')
    {
		if((Sum_Max>Sum_FXMax)&&(Sum_Max>Sync21_Set))
   	    {
   	        Plc_Mode='R';
    	    Sync_Step='E';
            Sync_bit1_cnt=0;            
            plc_byte_data=0;
            Work_step=1;
            Plc_data_bit_cnt = 8;
            //R_LED=1;
    	}
 	    else
  	    {
			plc_restart_sync();
   	    }
    }
    else 
    {
        if((Sum_Max>Sum_FXMax)&&(Sum_Max>Sync21_Set))
        {
            Plc_SyncBZ=1;
      	    Sync_Step='C';
      	    T16G1IE=0;    	
        }
        else
  	    {
		plc_restart_sync();
   	    }
    }
}


void _Sync2_Proc()
{
    uchar ucA,ucB,ucC;

    Sum_Max=0;
    Sum_FXMax=0;  //0720

    	SYM_offset2=0;
    	SYCl_offset2=0;
	
    //BitData_T[23+offset]=BitData_T[0+offset];
    for(ucB=0;ucB<8;ucB++) 				//约13335T
    {
        for(ucA=0;ucA<3;ucA++)
    	{
    	
			   Sum_DM21_Sync2(ucA);
	       	if(ZXDM>Sum_Max)
	        	{
	  	       	Sum_Max=ZXDM;
	  	        	SYM_offset2=ucA;
	  	        	SYCl_offset2=ucB;
	        	}			
    		

            //    else 6 nop 
	        if(FXDM>Sum_FXMax)
	        {
	  	        Sum_FXMax=FXDM;	  		  	
	        }
    	}
#if 0
        for(ucC=24;ucC<23+24;ucC++)
        {
            BitData_T[ucC]<<=1;
            if(BitData_T[ucC+1]&0x80) 
            {    	
                BitData_T[ucC]++;		//下一字节的高位移到当前的最低位
	            Bit1Num_T[ucC]++;
                Bit1Num_T[ucC+1]--;
            }
			
            //  else
            //  {  //16 NOP();
          
            //  } 
        }
#else
	{
	    	do_left_shift_1bit(0+24);
	    	do_left_shift_1bit(1+24);
	    	do_left_shift_1bit(2+24);
	    	do_left_shift_1bit(3+24);
	    	do_left_shift_1bit(4+24);
	    	do_left_shift_1bit(5+24);
	    	do_left_shift_1bit(6+24);
	    	do_left_shift_1bit(7+24);
	    	do_left_shift_1bit(8+24);
	    	do_left_shift_1bit(9+24);
	    	do_left_shift_1bit(10+24);
	    	do_left_shift_1bit(11+24);
	    	do_left_shift_1bit(12+24);
	    	do_left_shift_1bit(13+24);
	    	do_left_shift_1bit(14+24);
	    	do_left_shift_1bit(15+24);
	    	do_left_shift_1bit(16+24);
	    	do_left_shift_1bit(17+24);
	    	do_left_shift_1bit(18+24);
	    	do_left_shift_1bit(19+24);
	    	do_left_shift_1bit(20+24);
	    	do_left_shift_1bit(21+24);
	    	do_left_shift_1bit(22+24);
	    	//OFF_do(23+24);

    }
#endif		
    }
 
    if(Sync_Step=='C')
    {
		if((Sum_Max>Sum_FXMax)&&(Sum_Max>Sync21_Set))
   	    {
   	        Plc_Mode='R';
    	    Sync_Step='E';
            Sync_bit1_cnt=0;            
            plc_byte_data=0;
            Work_step=1;
            Plc_data_bit_cnt = 8;
            R_LED=0;
    	}
 	    else
  	    {
			plc_restart_sync();
   	    }
    }
    else 
    {
        if((Sum_Max>Sum_FXMax)&&(Sum_Max>Sync21_Set))
        {
            Plc_SyncBZ=1;
      	    Sync_Step='C';
      	    T16G1IE=0;    	
        }
        else
  	    {
		plc_restart_sync();
   	    }
    }
}

//union SVR_INT_B08 t1, t2;
//int test;
void Plc_SyncProc(void)
{
   //T16G2IE = 0;
  //	t1.NumChar[0]=T16G1L;
  //	t1.NumChar[1] = T16G1H; 
	_Sync1_Proc();
	if (Sync_Step == 'C')
		_Sync2_Proc();
	
  	//t2.NumChar[0]=T16G1L;
   //	t2.NumChar[1] = T16G1H;	
	//test = t2.NumInt - t1.NumInt;
  // T16G2IE = 1;     
   
}


BOOL plc_tx_idle()
{
    if (Plc_Mode=='T')//NOT IN TX
        return FALSE;
    else
	 return TRUE;    
}

int8u plc_tx_bytes(uchar *pdata ,uchar num)

{  
    if (FALSE == plc_tx_idle())
        return 0;

	plc_data[0]=0xaa;	
    plc_data[1]=num+2;	
	MMemcpy(&plc_data[2],pdata,num);
	
	Ini_Plc_Tx();
    S_LED=1;
	R_LED = 0;
//	cal_chk_sum();
	Plc_Mode='T';           
	//tmr_init=0xff;
	Work_step=0;
	t_end_bit=0;
	t_nor_bit=0;
	Plc_data_bit_cnt=8;
	Plc_data_byte_cnt=0;
	Plc_data_len=num+2;		//9
	plc_byte_data=0xff;
	Plc_Tx_first_Bit = 1;//first sync bit
	Plc_Tx_Second_Bit = 1; // second sync bit
	//SSC15=Pn15_Con1;
	IniT16G1(CCPMODE);
	T16G2IE=0;
	T16G2IF=0;
    T16G1IF=0;
	T16G1IE=1;
	enable_irq();
	//	T16G1H=5;
	//	T16G1L=0;
	return num;
}

int8u plc_rx_bytes(uchar *pdata)
{
    if(Rx_status=='R')
    {
    	int8u len = Plc_data_byte_cnt-1;
        MMemcpy(pdata, &plc_data[1], len);

        Plc_Mode=0;
        IniT16G1(CCPMODE);
 	    r_sync_bit=0;
        Rx_status=0;
        Plc_SyncBZ=0;
	    Sync_Step=0;
		Plc_data_byte_cnt = 0;
	    T16G2IE=0;
  	    T16G2IF=0;
  	    Work_step=0;
  	    //R_LED=0;
	    T16G1IF=0;
  	    T16G1IE=1;
		enable_irq();
        S_LED=0;        
        
        return  len;     
    }
    return 0;
}

void plc_init(void)
{
    PLC_RX;
    T16G1R_S.NumLong=50000;
    Ini_Plc_Rec();
    PLC_RX;
    watchdog();
    PLC_RW=0x01;
    PLC_ADDR=0x51;
    testbyte=PLC_DATA;
    PLC_RW=0x00;
 
    r_sync_bit=0;
    watchdog();
    T16G2CH=0x0b;
    T16G2CL=0x10;		//4:1 
    IniT16G1(CCPMODE);
    T16G1IF=0;
    T16G1IE=1;
	Plc_Mode = 0;
	Plc_ZeroMode = 0;
	Rec_Zero_bz= 0;
    Plc_data_byte_cnt = 0;
}

void plc_driver_process(void)
{
    watchdog();
    
    if(Rec_Zero_bz)
    {	
    
        if(Plc_Mode=='R')
        {       
            if(Sync_Step=='E')
                Plc_RecvProc();
        }
        else
        {
            if(Plc_Mode=='F')
    	    {
    	        Plc_SyncProc();    		  
    	    }
        }
        Rec_Zero_bz=0;
    }    
}

#endif /* ifdef CONFIG_200BPS_PLC*/
