//说明：FSK载波 过零发送模式
//从 plcfsk_rx_v12z_hd.c 修改而来

/*
0x7e 0x7e 0xaa data data.... 0x7e
数据位中，连续有5个1的时候，插入一个0，接受的时候，连续5个1，丢掉后面的0
*/
#include <hic.h>
#include "type.h"
#include "soc_25xx.h"
#include "tool.h"
#include "system.h"
#include "debug.h"

#define INSERT_0BIT_CNT 5
#define  CCPMODE  0x02
#define  COMPMODE 0x01
#define  RH_Time  0X05
#define  RL_Time  0X33
#define  R_LED PB5		 
#define  S_LED PC0  
#define	 STA     PB6
#define  PLC_TXEN  PC1

#define  PLC_TX   PLC_TXEN=1 //使能发送PA
#define  PLC_RX   PLC_TXEN=0 //关闭发送PA

#define  Sync_bit1_cnt_Max  0x1f

#define TX_Bit1()	            PLC_MOD|=0x01    //PLC_TXD
#define TX_Bit0()               PLC_MOD&=0xfe
#define MaxPlcL      64
#define Zero_Num     4

#define NORMAL 			0x03
#define PnNum_Const 15
#define Sync_Set    0x5A
#define Sync21_Set    0x6e
#define Sync_Char  0xaf
#define Pn7_Con1   0X17

static volatile   section64  sbit  PLC_FSK_RXD      @ (unsigned) &PLC_MOD* 8 + 3 ;

unsigned char Sync_bit1_cnt,TX_step;
uchar section1  testbyte, tx_rx_byte;
uchar section1  continue_1bit_cnt;//
const uchar  T16g1RH_Time[6]={0x40,0x9c,0x93,0x2d,0xad,0x6e};
uchar  Sum_Max,ZXDM,Sum_FXMax,FXDM;

uchar BitData_T[27];//BitDataBak_T[16];	//同步时的位数据
uchar Bit1Num_T[27];					//同步时收到1的个数（18T内）
uchar SYM_offset,Sync_Step;
	  
uchar  SYCl_offset,SYMFX_offset,SYClFX_offset;
uchar  bit1_cnt,plc_byte_data,sync_word_l, Sync_data_byte_cnt, Sync_data_bit_cnt;
sbit   r_sync_bit,t_nor_bit,t_end_bit,Plc_Tx_Bit,Plc_SyncBZ,Psk_FxBz,Rec_Zero_bz,ACZero_Bz;
uchar  Plc_data_bit_cnt,Plc_data_byte_cnt,plc_data_len,Plc_S,Pn15_cnt,Plc_Mode,Plc_ZeroMode;

section3 uchar Plc_data[MaxPlcL]@180;
section3 uchar RSSIV,RSSIT; 
section3 uchar SYM_off[8],SYCl_off[8];
section3 uchar Sync_data[3];
section13 uchar ZXDM_buf[40],RSSIV_buf[40];
section13 uchar RSSIByte_buf[32],RSSIBit_buf[8];

section14 uchar Zero_cnt,Tx_timer;
section14 union SVR_INT_B08 T16G1R_int[8],T16G1R_20ms[8],T16G1R_old,T16G1R_new,T16G1_TRStartint;//T16G1R1,T16G1R2,T16G1R3,T16G1R4,T16G1R5,T16G1R6;
section14 union SVR_LONG_B08 T16G1R_S;


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
        T16G2RH=T16g1RH_Time[3];		//过零后ms发
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

uchar  Sum_DM7(uchar offset )
{
    uchar PnTemp,ucA,ZXDM_temp;
    ZXDM_temp=0;
    PnTemp=Pn7_Con1;
    for(ucA=0;ucA<7;ucA++)
    {
        PnTemp<<=1;
        if(PnTemp&0x80)    
            ZXDM_temp+=Bit1Num_T[ucA+offset]; 
        else
            ZXDM_temp+=(8-Bit1Num_T[ucA+offset]);
    }
    return(ZXDM_temp);
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

sbit Recv_Bit()
{
    if(ZXDM>84)
	{
		NOP();
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
	DelayEndBit();//1303
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
	DelayEndBit();//1303
}
//位间隔1998US
void tx_normal_data(void)   /*发送正常数据*/
{
    /* 如果发送的是正常的数据，而且有连续5个1，插入一个0*/
    if (t_nor_bit)
    {
        if (continue_1bit_cnt == INSERT_0BIT_CNT){
            continue_1bit_cnt = 0; 
            Plc_Tx_Bit = 0;
            return;   
        }
    }
    Plc_data_bit_cnt--;   /*还应发送几位*/
	if(Plc_data_bit_cnt==0)   /*此字节发送完*/
	{	
        Plc_data_byte_cnt++;
	   	
		Plc_data_bit_cnt=8;        /*应发送8位,先减一后发送,所以为9;*/
		if(t_nor_bit==1)
		{
            plc_byte_data=Plc_data[Plc_data_byte_cnt]; /*取要发送的数据,有0x09af*/
		        
			if(Plc_data_byte_cnt==plc_data_len)   /*全部发送完*/
			{
                t_nor_bit=0;
			   	t_end_bit=1;
			   	S_LED=0;			//最后一位扩频位尚未发送
			  	return;		
			}
		}
		else
		{
            plc_byte_data=0x7e;
		    if(Plc_data_byte_cnt==2) 
		    {
                t_nor_bit=1;
		        Plc_data_byte_cnt=0;
		        plc_byte_data=Plc_data[Plc_data_byte_cnt];
		    }
		}		
	}
	else
	{
        plc_byte_data<<=1;	
	}
	if(plc_byte_data&0x80)
	{
        Plc_Tx_Bit=1;
        continue_1bit_cnt ++;		
	}
	else {
        Plc_Tx_Bit=0;	
        continue_1bit_cnt = 0;
    } 	   			
 }
//recv 0x7e,0x7e, 0xaa
void rcv_sync_data(void)
{
	Sync_data_bit_cnt--;     	/*还剩几位*/
	if(Sync_data_bit_cnt!=0)	/*未接收完一个字节*/
		return;
    Sync_data[Sync_data_byte_cnt]=plc_byte_data;  
	Sync_data_byte_cnt++;
	Sync_data_bit_cnt=8;
}
   
/*******************接收载波数据******************/
void rcv_normal_data(void)   /*正常接收*/
{	
	Plc_data_bit_cnt--;     	/*还剩几位*/
	if(Plc_data_bit_cnt!=0)	/*未接收完一个字节*/
		return;
	Plc_data_byte_cnt++;
	Plc_data_bit_cnt=8;
	Plc_data[Plc_data_byte_cnt-1]=plc_byte_data;              /*存储一个字节*/
	
	if(Plc_data_byte_cnt > MaxPlcL) 
	{// plc_data_len=Plc_data[6]+8;
        //if(Plc_data[1]>=MaxPlcL) 
	  	{
            T16G2IE=0;
  	        Plc_Mode=0;
  	        TX_step=0;
  	        Sync_Step=0;
  	        r_sync_bit=0;
  	        Plc_SyncBZ=0;
            T16G1IF=0;
  	        T16G1IE=1;
            return;
		}
	}	
	else	
	{ 
		if(Plc_data[Plc_data_byte_cnt-1] == 0x7e) /*收到结束sync，全部接收完*/
		{
            tx_rx_byte='R';
			r_sync_bit=0;
			plc_byte_data=0;									
		}
	}	/*还未接收完*/
	//Plc_data_bit_cnt=8;
}

void Recv_Plc_8Bit(void)		//1332t采样8次,166T和167间隔采样
{
	uchar ucA;
	Plc_S=0;	
	bit1_cnt=0;	
	delay5us();
	delay14t();
	delay14t();
	   
	Plc_S<<=1;
	if(PLC_FSK_RXD) 
	{	
        Plc_S|=0x01;
	 	bit1_cnt++;
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
	Plc_S<<=1;
	if(PLC_FSK_RXD) 
	{	
        Plc_S|=0x01;
	 	bit1_cnt++;
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
	Plc_S<<=1;
	if(PLC_FSK_RXD)
	{
        Plc_S|=0x01;
	 	bit1_cnt++;
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
	Plc_S<<=1;
	if(PLC_FSK_RXD)
	{	
        Plc_S|=0x01;
	  	bit1_cnt++;
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
	Plc_S<<=1;
	if(PLC_FSK_RXD)
	{	
        Plc_S|=0x01;
	 	bit1_cnt++;
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
	Plc_S<<=1;
	if(PLC_FSK_RXD)
	{	
        Plc_S|=0x01;
	 	bit1_cnt++;
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
	Plc_S<<=1;
	if(PLC_FSK_RXD)
	{	
        Plc_S|=0x01;
	 	bit1_cnt++;
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
	Plc_S<<=1;
	if(PLC_FSK_RXD)
	{
        Plc_S|=0x01;
	 	bit1_cnt++;
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

void  Recv_25Pn(void)
{
    STA=0;
    //* 0512
    Recv_Plc_8Bit();				
	Bit1Num_T[0]=bit1_cnt;	//1的个数
	BitData_T[0]=Plc_S;	
	
    delay14t();
	delay14t();
	delay14t();
	NOP();
    NOP();
    NOP();
    //*     0512
	Recv_Plc_8Bit();				
	Bit1Num_T[0]=bit1_cnt;	//1的个数
	BitData_T[0]=Plc_S;	
	delay14t();
	delay14t();
	delay14t();
	NOP();
    NOP();
    NOP();
             
	Recv_Plc_8Bit();				
	Bit1Num_T[1]=bit1_cnt;	//1的个数
	BitData_T[1]=Plc_S;	
	delay14t();
	delay14t();
	delay14t();
	NOP();
	NOP();
	NOP();
	  
	Recv_Plc_8Bit();				
	Bit1Num_T[2]=bit1_cnt;	//1的个数
	BitData_T[2]=Plc_S;	
	delay14t();
	delay14t();
	delay14t();
	NOP();
    NOP();
    NOP();
	
    Recv_Plc_8Bit();				
	Bit1Num_T[3]=bit1_cnt;	//1的个数
	BitData_T[3]=Plc_S;	
	delay14t();
	delay14t();
	delay14t();
	NOP();
    NOP();
    NOP();
	
    Recv_Plc_8Bit();				
	Bit1Num_T[4]=bit1_cnt;	//1的个数
	BitData_T[4]=Plc_S;	
	delay14t();
	delay14t();
	delay14t();
	NOP();
    NOP();
    NOP();
	  	
    Recv_Plc_8Bit();				
    Bit1Num_T[5]=bit1_cnt;	//1的个数
    BitData_T[5]=Plc_S;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
       
    Recv_Plc_8Bit();				
    Bit1Num_T[6]=bit1_cnt;	//1的个数
    BitData_T[6]=Plc_S;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
    
    Recv_Plc_8Bit();				
    Bit1Num_T[7]=bit1_cnt;	//1的个数
    BitData_T[7]=Plc_S;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
    
    Recv_Plc_8Bit();				
    Bit1Num_T[8]=bit1_cnt;	//1的个数
    BitData_T[8]=Plc_S;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
    
    Recv_Plc_8Bit();				
    Bit1Num_T[9]=bit1_cnt;	//1的个数
    BitData_T[9]=Plc_S;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
    
    Recv_Plc_8Bit();				
    Bit1Num_T[10]=bit1_cnt;	//1的个数
    BitData_T[10]=Plc_S;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
    
    Recv_Plc_8Bit();				
    Bit1Num_T[11]=bit1_cnt;	//1的个数
    BitData_T[11]=Plc_S;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
    
    Recv_Plc_8Bit();				
    Bit1Num_T[12]=bit1_cnt;	//1的个数
    BitData_T[12]=Plc_S;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
    
    Recv_Plc_8Bit();				
    Bit1Num_T[13]=bit1_cnt;	//1的个数
    BitData_T[13]=Plc_S;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
        
    Recv_Plc_8Bit();				
    Bit1Num_T[14]=bit1_cnt;	//1的个数
    BitData_T[14]=Plc_S;	
    delay14t();
    delay14t();
    delay14t(); 
    NOP();
    NOP();
    NOP();
        
    Recv_Plc_8Bit();				
    Bit1Num_T[15]=bit1_cnt;	//1的个数
    BitData_T[15]=Plc_S;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
             
    Recv_Plc_8Bit();				
    Bit1Num_T[16]=bit1_cnt;	//1的个数
    BitData_T[16]=Plc_S;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
             
    Recv_Plc_8Bit();				
    Bit1Num_T[17]=bit1_cnt;	//1的个数
    BitData_T[17]=Plc_S;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
            
    Recv_Plc_8Bit();				
    Bit1Num_T[18]=bit1_cnt;	//1的个数
    BitData_T[18]=Plc_S;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
     
    Recv_Plc_8Bit();				
    Bit1Num_T[19]=bit1_cnt;	//1的个数
    BitData_T[19]=Plc_S;	
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
    Bit1Num_T[20]=bit1_cnt;	//1的个数
    BitData_T[20]=Plc_S;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
      
    Recv_Plc_8Bit();				
    Bit1Num_T[21]=bit1_cnt;	//1的个数
    BitData_T[21]=Plc_S;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
    
    Recv_Plc_8Bit();				
    Bit1Num_T[22]=bit1_cnt;	//1的个数
    BitData_T[22]=Plc_S;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
     
    Recv_Plc_8Bit();				
    Bit1Num_T[23]=bit1_cnt;	//1的个数
    BitData_T[23]=Plc_S;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
     
    Recv_Plc_8Bit();				
    Bit1Num_T[24]=bit1_cnt;	//1的个数
    BitData_T[24]=Plc_S;	
    delay14t();
    delay14t();
    delay14t();
    NOP();
    NOP();
    NOP();
     
    Recv_Plc_8Bit();				
    Bit1Num_T[25]=bit1_cnt;	//1的个数
    BitData_T[25]=Plc_S;	
    STA=1;	 
    RSSIBit_buf[Plc_data_bit_cnt-1]=RSSIV;  	
}


void Zero_Time_hd()
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
            IniT16G2(1);
            T16G2IF=0;
            T16G2IE=1;
            TX_step=1; 
        }
    }
    else if(Plc_Mode=='T')
    {
        if(TX_step!=2)
        {
            IniT16G2(1);
            T16G2IF=0;
            T16G2IE=1;
            TX_step=1; 
        }
        else
        { //T16G1IE=0;
        }
    }	

   	
    T16G1R_old.NumInt=T16G1R_new.NumInt;
    T16G1R_new.NumChar[0]=T16G1RL;
    T16G1R_new.NumChar[1]=T16G1RH; 	
    //  ACZero_Bz=1;
    Zero_Time_hd();
	//  Zero_cnt=0;
	TX_step=0;
	//	   Plc_Mode='R';
   }
void T16G2Int_Proc(void)
{
    T16G1_TRStartint.NumChar[1]=T16G1H;
    T16G1_TRStartint.NumChar[0]=T16G1L;
 
    if(T16G1_TRStartint.NumChar[1]!=T16G1H)
    {
        T16G1_TRStartint.NumChar[0]=T16G1L;
    }
    if(Plc_Mode=='T')
    {
    //        Uart_rec1Point=0;
        if(TX_step==2)
        {
            PLC_MOD=0x23;
            PLC_TX;
            if(t_end_bit==0)  //
		    {
		        if(Plc_Tx_Bit)
		            TX0_PN21();
		        else
		            TX1_PN21();
		        
		        tx_normal_data(); 
		        PLC_MOD=0x00;
		        T16G2RH=T16G1R_S.NumChar[1];  //赋下次过零接收时间
                T16G2RL=T16G1R_S.NumChar[0];		   
		    }
            if(t_end_bit)
            {
                Plc_Mode=0;
           	
           	    t_nor_bit=0;
           	    plc_data_len=0;
	 
           	    R_LED=0;
                //     	 T16G1IF=0;			//0802
                T16G1IE=1;
                 
		        T16G2IE=0;
	            PLC_MOD=0X59;
            }
            PLC_RX;
 	
 	        //    PLC_MOD=0;
     
	    }
	    else // if(TX_step==1)		//
	    {
	        //Ini_Plc_Tx();
	        TX_step=2;
            PLC_MOD=0x23;
            //  PLC_RW=0x00;
            PLC_TX;
        
	        T16G2RH=T16G1R_S.NumChar[1];  //赋下次过零接收时间
            T16G2RL=T16G1R_S.NumChar[0];
            TX0_PN21();
         
            Plc_Tx_Bit=1;
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
        if(TX_step==2)
        {
            PLC_MOD=0x59;
         
   	        Recv_25Pn();
   	
   	        PLC_MOD=0x00;		//0720
        }
   	    else // if(TX_step==1)		//
	    {
	        //Ini_Plc_Tx();
            PLC_MOD=0x59;
            //   PLC_RX;
            Recv_25Pn();   
            PLC_MOD=0x00;       //0720
            TX_step=2;
       
            //   PLC_MOD=0x00;
 	        //    	Ini_Plc_Rec();
            //	  PLC_MOD=0x59;
            //   T16G1CL=0x21;		//
        }
        T16G2RH=T16G1R_S.NumChar[1];  //赋下次过零接收时间
        T16G2RL=T16G1R_S.NumChar[0];
    }
    else  if(Plc_Mode==0)
    {     
        PLC_MOD=0x59;
        //   PLC_RX;
        Recv_25Pn(); 
        T16G2RH=T16G1R_S.NumChar[1];  //赋下次过零接收时间
        T16G2RL=T16G1R_S.NumChar[0];
         
        PLC_MOD=0x00;    //0720
        Plc_Mode='F';  
        Rec_Zero_bz=1;
		T16G1IE=0;
		//	T16G1IF=0;	//0802
        //  TX_step=2;
        //    Rec_Zero_bz=1;
    }
    else  if(Plc_Mode=='F')
    {
        PLC_MOD=0x59;
        //   PLC_RX;
       
	    //  T16G2RH=T16G1R_S.NumChar[1];  //赋下次过零接收时间
        //  T16G2RL=T16G1R_S.NumChar[0];
        Recv_25Pn(); 
        PLC_MOD=0x00;		//0720
        Plc_Mode='F';     
        Rec_Zero_bz=1;   
        //  TX_step=2;
        //    Rec_Zero_bz=1;
    }
    
    if(T16G1IF)
    {   	
   	    T16G1R_old.NumInt=T16G1R_new.NumInt;
   	    T16G1R_new.NumChar[0]=T16G1RL;
   	    T16G1R_new.NumChar[1]=T16G1RH;
   	
   	    //Zero_cnt++; 
	    Zero_Time_hd();
		T16G1IF=0;   	
   	}
}

void Data_adjust_by_sync(void)
{
    uchar ucB,ucC;
    for(ucB=0;ucB<SYCl_offset;ucB++) 				//约13335T
    {
        for(ucC=SYM_offset;ucC<24;ucC++)
        {
            BitData_T[ucC]<<=1;
            if(BitData_T[ucC+1]&0x80) 
            {
                BitData_T[ucC]++;		//下一字节的高位移到当前的最低位
	            Bit1Num_T[ucC]++;
                Bit1Num_T[ucC+1]--;
            }  
        }
    }
}
void plc_recv_Proc(void)
{
    //if(1/*Sync_Step=='E'*/)

	if (r_sync_bit) {/* 接受正常数据，连续5个1后，丢掉后面的一个0*/
	    if (continue_1bit_cnt == INSERT_0BIT_CNT){
	        continue_1bit_cnt = 0;
	        return;
	    }
	}
	Data_adjust_by_sync();	
	Sum_DM21_Sync(SYM_offset);

    plc_byte_data<<=1;
    if(Recv_Bit()) {
        plc_byte_data|=0x01;
        continue_1bit_cnt ++;
    }else {
        continue_1bit_cnt = 0;
    }
     
	/* 等待 sync 字节 0x7e 0x7e 0xaa */
	if (!r_sync_bit){
	    ZXDM_buf[Sync_bit1_cnt]=ZXDM;
        RSSIV_buf[Sync_bit1_cnt]=RSSIV;
	    rcv_sync_data();
	    Sync_bit1_cnt++;
	}    

    if(r_sync_bit==1)    //如果收到桢同步信号
	{
	    rcv_normal_data();    //
	 	return;
	}

	if(Sync_data_byte_cnt == 3)
	{
	    if ((Sync_data[2]==0xaa)&&(Sync_data[1]==0x7e)) //Sync_Char
	    {
	        Plc_data_bit_cnt=8;  //收到位数标志置为9,除此位是先减一后接收外,其它都是先接收后减一,所以其它应为8;
		    r_sync_bit=1;   //收到桢同步标志置1
									//	r_func1_bit=0;	//准备收第一个字节FUNC
		    Plc_data_byte_cnt=0;
		    continue_1bit_cnt = 0;
		    Sync_data_byte_cnt = 0;
		    Sync_data_bit_cnt = 0;
		}else {
		    Sync_Step=0;
		    Plc_SyncBZ=0;
		    Plc_Mode=0;
		    R_LED=0;
		    IniT16G1(CCPMODE);
		    T16G1IE=1;
		    T16G2IE=0;		    
		}
	}       
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
 


//相位同步法1,每次移1BIT,移7次，每次都要按15个字节按字节移15次；共计算8*15=120次
void Sync1_Proc(void)
{
    uchar ucA,ucB,ucC;

    Sum_Max=0;
    Sum_FXMax=0;  //0720
    SYM_offset=0;
    SYCl_offset=0;
  
  
    BitData_T[26]=BitData_T[0];
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

        for(ucC=0;ucC<23;ucC++)
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
    }
 
    if(Sync_Step=='C')
    {
		if((Sum_Max>Sum_FXMax)&&(Sum_Max>Sync21_Set))
   	    {
   	        Plc_Mode='R';
    	    Sync_Step='E';
            Sync_bit1_cnt=0;            
            plc_byte_data=0;
            TX_step=1;
            R_LED=1;
            Sync_data_bit_cnt = 8;//recv the sync bytes 0x7e 0x7e 0xaa
    	}
 	    else
  	    {
  	        T16G2IE=0;
  	        Plc_Mode=0;
  	        TX_step=0;
  	        Sync_Step=0;
  	        Plc_SyncBZ=0;
            T16G1IF=0;
  	        T16G1IE=1;
  	        // IniT16G1(CCPMODE);
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
  	        T16G2IE=0;
            Plc_Mode=0;
            TX_step=0;
  	        Sync_Step=0;
  	        Plc_SyncBZ=0;
	        T16G1IF=0;
  	        T16G1IE=1;
  	        //  IniT16G1(CCPMODE);
   	    }
    }
}

/**/
BOOL plc_tx_idle()
{
    if (Plc_Mode=='T')//NOT IN TX
        return FALSE;
    else
	 return TRUE;
}

uchar plc_tx_bytes(uchar *pdata ,uchar num)
{  

   if (FALSE == plc_tx_idle())
        return 0;
	Plc_data[0]=0xaa;
	Plc_data[1]=num;
	MMemcpy(&Plc_data[2],pdata,num);
	plc_data_len=num+2;
   Plc_data_byte_cnt = num;
	plc_byte_data=0x7e;	//先发两个0x7e，用来同步
	T16G2IE=0;
	Ini_Plc_Tx();
    S_LED=1;
//	cal_chk_sum();
	//Plc_Mode='T';           
	//tmr_init=0xff;
	TX_step=0;
	t_end_bit=0;
	t_nor_bit=0;
	Pn15_cnt=0;
	Plc_data_bit_cnt=8;
	Plc_data_byte_cnt=0;
	continue_1bit_cnt = 0;
   tx_rx_byte='R';
	//SSC15=Pn15_Con1;
	IniT16G1(CCPMODE);
    T16G1IF=0;
	T16G1IE=1;
Plc_data_byte_cnt = num;
    return num;

}

int8u plc_rx_bytes(uchar *pdata)
{
    if(tx_rx_byte=='R')
    {
        MMemcpy(pdata, &Plc_data[2], Plc_data_byte_cnt);

        Plc_Mode=0;
        IniT16G1(CCPMODE);
 	    r_sync_bit=0;
        tx_rx_byte=0;
        Plc_SyncBZ=0;
	    Sync_Step=0;
	    T16G2IE=0;
  	  
  	    TX_step=0;
  	    R_LED=0;
	    T16G1IF=0;
  	    T16G1IE=1;
        S_LED=0;        
        
        return  Plc_data_byte_cnt;     
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
    continue_1bit_cnt = 0;         
}

void plc_driver_process(void)
{
    watchdog();
    
    if(Rec_Zero_bz)
    {	
        if(Plc_Mode=='R')
        {       
            if(Sync_Step=='E')
                plc_recv_Proc();
        }
        else
        {
            if(Plc_Mode=='F')
    	    {
    	        Sync1_Proc();    		  
    	    }
        }
        Rec_Zero_bz=0;
    }    
}
/************************************/
/************************************/
#if 0
void main(void)
{ 
    int8u i;

    // INI7p90();
    RAM_Clr();
    //Ini2571();
    IO_Init();
    //RAM_Clr();
    PLC_RX;
    R_LED=1;
    Delay(200);
    R_LED=0;
    watchdog();
    Delay(200);

    R_LED=1;
    Delay(200);
    watchdog();
    R_LED=0;
    //  Recv_Plc_PN(); 
    T16G1R_S.NumLong=50000;
    Ini_Plc_Rec();
    PLC_RX;
    watchdog();
  
    PLC_RW=0x01;
    PLC_ADDR=0x51;
    testbyte=PLC_DATA;
    PLC_RW=0x00;
 
    r_sync_bit=0;
    // Plc_SyncBZ=0;
    // SyncFind();
    watchdog();
    T16G2CH=0x0b;
    T16G2CL=0x10;		//4:1 
    IniT16G1(CCPMODE);
    T16G1IF=0;
    T16G1IE=1;
    continue_1bit_cnt = 0;
    //  PLC_MOD=0x59;		//关接收
    // INTG=0x84;
    do
    {
        ei(); 
        watchdog();

        if(Rec_Zero_bz)
        {	
            if(Plc_Mode=='R')
    	    {       
                if(Sync_Step=='E')
    	            plc_recv_Proc();
    	    }
    	    else
    	    {
    	        if(Plc_Mode=='F')
    		    {
    		        Sync1_Proc();    		  
    		    }
    	    }
    	    Rec_Zero_bz=0;
        }
        if(tx_rx_byte=='R')
        {
            //Led_Con();
            Plc_Mode=0;
            IniT16G1(CCPMODE);
            //全部接受ok，处理, call the linklay
            linklay_recv_data(Plc_data, Plc_data_byte_cnt);
            //MMemcpy(app_data, Plc_data, MaxPlcL);
	        r_sync_bit=0;
            tx_rx_byte=0;
            Plc_SyncBZ=0;
	        Sync_Step=0;
	        T16G2IE=0;
  	  
  	        TX_step=0;
  	        R_LED=0;
	        T16G1IF=0;
  	        T16G1IE=1;
            S_LED=0;
        }
        if(t_end_bit)
	    {
	    	t_end_bit=0;
	  	    Ini_Plc_Rec();	  	
	    }
	  
     }while(1);
}

#endif
