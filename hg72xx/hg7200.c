#include <hic.h>
#include "config.h"
#include "type.h"
#include "system.h"
#include "timer16n.h"
#include "tool.h"
//#include "hr.h"
#include "macro_define.h"
//#include "function_state.h"
#include "type_define.h"
#include "variable_state.h"
#include <math.h>

struct S_EmuParameter gs_Emu_Para;   //计量芯片调校参数,需要存入存储器保存数据，具体如何存储，用户可据实际开发情况具体而定
struct  S_MeterVariable  gs_Meter_Variable; 
union  U_220VFlag   gu_220V_Flag;
float		g_Pav_Temp;	//计算1分钟平均功率
uint8_t  	g_Pav_Cnt;
uint8_t	g_Reverse_Filter;
uint8_t   g_Emu_Reg[5]; 


#ifdef CONFIG_HG72xx_UART
#if CONFIG_HG72xx_UART==0
#define UART_TX_REG TX1B
#define UART_TX8_REG TX1R8
#define UART_TRMT_REG TRMT1
#define UART_RX_REG RX1B
#define UART_RXIF_REG RX1IF
#define UART_TX_EN TX1EN
#endif
#if CONFIG_HG72xx_UART==1
#define UART_TX_REG TX2B
#define UART_TX8_REG TX2R8
#define UART_TRMT_REG TRMT2
#define UART_RX_REG RX2B
#define UART_RXIF_REG RX2IF
#define UART_TX_EN TX2EN

#endif
#if CONFIG_HG72xx_UART==2
#define UART_TX_REG TX3B
#define UART_TX8_REG TX3R8
#define UART_TRMT_REG TRMT3
#define UART_RX_REG RX3B
#define UART_RXIF_REG RX3IF
#define UART_TX_EN TX3EN

#endif

#endif

void Emu_Reg_Verify(void)			//计量芯片寄存器校验
{;}
//********************************************************/
//**************EMU Test Program*********************/
//********************************************************/

/*****************计量芯片SPI操作固定延时*******************
函 数 名：  void Emu_Com_Op_Delay(void)                       
功能描述：  系统延时                           
输入参数：  计量芯片SPI操作固定延时                                       
函数返回值：无
***********************************************************/
void Emu_Com_Op_Delay(void)
{
	uint8_t  i;
	for(i = 0; i < 156; i++);
}

/*****************计量芯片SPI操作固定延时*******************
函 数 名：  void Emu_Com_Op_Delay(void)                       
功能描述：  系统延时                           
输入参数：  计量芯片SPI操作固定延时                                       
函数返回值：无
***********************************************************/
void Emu_Com_Op_Delay1(void)
{
	uint8_t  i;
	for(i = 0; i < 65; i++);
}
#if (CONFIG_HG7220_MODE==0)       //选择计量芯片端口通信方式
/*****************计量芯片SPI读写操作**********************
函 数 名：  uint8_t Emu_Com_Op(uint8_t OpMode)                       
功能描述：  计量芯片SPI读写操作         
输入参数：  操作模式：读或写                                       
函数返回值：无
***********************************************************/
uint8_t Emu_Com_Op(uint8_t OpMode) 	//操作类型,0=>Write,1=>Read
					                     //操作的寄存器地址,操作的最高字节,操作的中间字节,操作的最低字节
											   //计量芯片数据为大端格式，例如： *g_Emu_Reg->OpAddr,*(g_Emu_Reg+1)->MSB,*(g_Emu_Reg+2)->MidSB,*(g_Emu_Reg+3)->LSB
				        
				
{
    uint8_t  i,j,tempbuf[5],k,buf08;
    uint8_t  r,n,reg_addr;
	
	 memset00(tempbuf, 5);   //清比对寄存器
	
    if(g_Emu_Reg[0] <= 0x0C)       //地址0X00~0X0C的为8位寄存器
        r = 1 + 1;
    else if((g_Emu_Reg[0] >= 0x10) && (g_Emu_Reg[0] <= 0x2F))  //地址0X10~0X2F的为16位寄存器
        r = 2 + 1;
    else if((g_Emu_Reg[0] >= 0x30) && (g_Emu_Reg[0] <= 0x32))  //地址0X30~0X32的为24位寄存器
        r = 3 + 1;
    else if((g_Emu_Reg[0] >= 0x40) && (g_Emu_Reg[0] <= 0x46))  //地址0X40~0X46的为32位寄存器
        r = 4 + 1;
    else if(g_Emu_Reg[0] == 0x50) //特殊寄存器
        r = 3 + 1;
    else if(g_Emu_Reg[0] == 0x70) //写保护寄存器
        r = 1 + 1;
    else if(g_Emu_Reg[0] == 0x71) //芯片ID
        r = 2 + 1;
    else if(g_Emu_Reg[0] == 0x72) //上一次SPI写入的数据
        r = 4 + 1;
    else if(g_Emu_Reg[0] == 0x73) //上一次SPI读出的数据
        r = 4 + 1;
    else if(g_Emu_Reg[0] == 0x7f) //特殊寄存器写保护寄存器
        r = 1 + 1;
    else 
        return 1;    //出错

	 if(OpMode)
	 {
        reg_addr = 0x72;	//上一次写操作
        n = 4;
    }
	 else
    {
        reg_addr = 0x73;	//上一次读操作
        n = 4;
    }

    for(k = 0; k < 3; k++)
    {
        watchdog();    //清狗
 		
        SPICS_EN;       //CS使能
        Emu_Com_Op_Delay();
        g_Emu_Reg[0] |= OpMode;	//设置读写操作		
		
        if(!OpMode)	//如果是读操作
        {
            for(i = 1; i < r; i++) //清接收缓存
                g_Emu_Reg[i] = 0;
            for(i = 0; i < 8; i++) //发送地址
            {
                if(g_Emu_Reg[0] & (0x80 >> i))
                    SPIMO1;
                else
                    SPIMO0;
                Emu_Com_Op_Delay1();
                SPICLK1;
                Emu_Com_Op_Delay();
                SPICLK0;
                Emu_Com_Op_Delay1();	
            }
						
            for(j = 1; j < r; j++) //接收数据
            {
                for(i = 0; i < 8; i++)
                {
                    SPICLK1;
                    Emu_Com_Op_Delay();
                    SPICLK0;
                    buf08=(0x80 >> i);
                    if(SPIMI)
                        g_Emu_Reg[j] |= buf08;
 					     else
                        g_Emu_Reg[j] &= ~buf08;
                    Emu_Com_Op_Delay1();
                    Emu_Com_Op_Delay1();
                }
             }
        }
        else		//如果是写操作
        {
			
            for(j = 0; j < r; j++)  //发送地址和数据
            {
                for(i = 0; i < 8; i++)
                {
                    if(g_Emu_Reg[j] & (0x80 >> i))
                        SPIMO1;
                    else
                        SPIMO0;
                    Emu_Com_Op_Delay1();
                    SPICLK1;
                    Emu_Com_Op_Delay();
                    SPICLK0;			
                    Emu_Com_Op_Delay1();	
                }
            }
        }
        Emu_Com_Op_Delay();
        SPICS_DIS;	  //计量芯片禁止
        Emu_Com_Op_Delay();
		

        if(!OpMode)    //如果是读操作
        {
            if(g_Emu_Reg[0] >= 0x70) 
                return 0;
        }
        else           //如果是写操作
        {
            if(g_Emu_Reg[0] >= 0xf0) 
                return 0;
        }
					
        SPICS_EN;     //计量芯片使能
        Emu_Com_Op_Delay();
		
        *tempbuf = reg_addr;  //刚刚SPI写入或读出的数据地址
		
        for(i = 0; i < 8; i++)//发送地址
        {
            if(*tempbuf & (0x80 >> i))
                SPIMO1;
            else
                SPIMO0;
            Emu_Com_Op_Delay1();
            SPICLK1;
            Emu_Com_Op_Delay();
            SPICLK0;
            Emu_Com_Op_Delay1();	
        }
        for(j = 1; j < (uint8_t)(n+1); j++) //读取数据
        {
            for(i = 0; i < 8; i++)
            {
                SPICLK1;
                Emu_Com_Op_Delay();
                SPICLK0;
                buf08=(0x80 >> i);
                if(SPIMI)
                    *(tempbuf + j) |= buf08;
                else
                    *(tempbuf + j) &= ~buf08;
				    Emu_Com_Op_Delay1();
				    Emu_Com_Op_Delay1();
			   }
        }
        SPICS_DIS;	//计量芯片禁止
        Emu_Com_Op_Delay();
				
        for(i = 1; i < r; i++)  //比较数据是否一致
        {
            if(g_Emu_Reg[i] != *(tempbuf + i + n - r + 1))  //72H和73H寄存器的值为高位对齐方式
            {
                break;
            }
        }
        if( i == r)  //数据全部一致
        {
            g_Emu_Reg[0] &= B0111_1111;
            return 0;	//读写成功
        }	 
	}
	return 1;			//读写失败
}
#else
/*****************奇偶校验函数 ***********************************
函 数 名：  uint8_t Check_OddEven1(uint8_t Data)      
功能描述：  判断数据奇偶                     
输入参数：  待判数据                        
函数返回值：奇偶位                                
******************************************************************/
uint8_t Check_OddEven1(uint8_t Data)
{
    uint8_t i, j;
    j = 0;
    for(i=0; i<8; i++)
    {
        if(Data & 0x01)
        {
            j++;
        }
        Data = Data>>1; 
    }
    return(j);
}
/*****************计量芯片数据urat发送函数 **************************
函 数 名：  void HG7220_uart_tx(uint8_t senddata)      
功能描述：  串口发送数据                     
输入参数：  待发送数据                        
函数返回值：无                                
******************************************************************/
void HG7220_uart_tx(uint8_t senddata)
{
	uint8_t temp08;
 	temp08  = Check_OddEven1(senddata);//奇偶校验
  
	UART_TX8_REG = temp08;  
	UART_TX_REG = senddata; 
	delay_ms(1);
	while (!UART_TRMT_REG);
	return;
}
/*****************读计量芯片数据urat接收函数 *********************
函 数 名：  uint8_t HG7220_uart_rx(void)     
功能描述：  串口接收数据                    
输入参数：  无                     
函数返回值：接收的数据                                
******************************************************************/
uint8_t HG7220_uart_rx(void) 
{ 
    uint8_t    recvdata;
    recvdata = UART_RX_REG;      //从UART2数据寄存器中取接收数据  
   	return(recvdata);
}

/*****************计量芯片通信读写函数 *********************
函 数 名：  uint8_t ATT7051SPIOp(uint8_t OpMode)     
功能描述：  实现计量芯片数据的读写操作                    
输入参数：  读或写计量地址                     
函数返回值：success or fail                                
******************************************************************/
uint8_t Emu_Com_Op(uint8_t OpMode 	//操作类型,0=>Write,1=>Read
					            //操作的寄存器地址,操作的最高字节,操作的中间字节,操作的最低字节								   //*OpData->OpAddr,*(Opdata+1)->MSB,*(OpData+2)->MidSB,*(OpData+3)->LSB
				       ) 
				
{
	uint8_t  i,j,tempBuf[5],k,buf08;
    uint8_t  r,n,reg_Addr,m;
	
	memset00(tempBuf, 5);
	
	if(g_Emu_Reg[0] <= 0x0C) r = 1+1;
	else if(g_Emu_Reg[0] <= 0x2F) r = 2+1;
	else if(g_Emu_Reg[0] <= 0x32) r = 3+1;
	else if(g_Emu_Reg[0] <= 0x46) r = 4+1;
	else if(g_Emu_Reg[0] <= 0x56) r = 3+1;	
	else if(g_Emu_Reg[0] == 0x70) r = 1+1;
	else if(g_Emu_Reg[0] == 0x71) r = 2+1;
	else if(g_Emu_Reg[0] <= 0x73) r = 4+1;
	else if(g_Emu_Reg[0] == 0x7f) r = 1+1;      //特殊命令
	else return 1;                              //出错

	if(OpMode)
	{
        reg_Addr = 0x72;	                     //上一次写操作
        n = 4;
    }
	else
    {
        reg_Addr = 0x73;	                     //上一次读操作
        n = 4;
    }

    for(k = 0; k < 3; k++)
    {
        watchdog();
        g_Emu_Reg[0] |= OpMode;			   //读写控制
		
        if(!OpMode)	                           //Read
        {
            for(i = 1; i < r; i++)
                g_Emu_Reg[i] = 0;
		    HG7220_uart_tx(g_Emu_Reg[0]);
			delay_ms(1);	
            for(j = 1; j < r; j++)
            { 
              	m=100;
              	while(!UART_RXIF_REG)
              	{
              		m--;
              		delay_ms(1);
              		if(m==0)
                   		break;
              	}
             	UART_RXIF_REG = 0;
              	g_Emu_Reg[j] = HG7220_uart_rx();
		  	}
		 	if(r==3)
	     	{
              	buf08=g_Emu_Reg[1];
              	g_Emu_Reg[1]=g_Emu_Reg[2];
              	g_Emu_Reg[2]=buf08;
            } 
	       	if(r==4)
			{
              	buf08=g_Emu_Reg[1];
              	g_Emu_Reg[1]=g_Emu_Reg[3];
              	g_Emu_Reg[3]=buf08;
            } 
		 	if(r==5)
	    	{
              	buf08=g_Emu_Reg[1];
              	g_Emu_Reg[1]=g_Emu_Reg[4];
              	g_Emu_Reg[4]=buf08;
              	buf08=g_Emu_Reg[2];
              	g_Emu_Reg[2]=g_Emu_Reg[3];
              	g_Emu_Reg[3]=buf08;
            }
        }
        else		                                  //Write
        {
          	if(r==3)
 			{
              	buf08=g_Emu_Reg[1];
              	g_Emu_Reg[1]=g_Emu_Reg[2];
              	g_Emu_Reg[2]=buf08;
            } 
	     	if(r==4)
			{
              	buf08=g_Emu_Reg[1];
              	g_Emu_Reg[1]=g_Emu_Reg[3];
              	g_Emu_Reg[3]=buf08;
            } 
			if(r==5)
			{
              	buf08=g_Emu_Reg[1];
              	g_Emu_Reg[1]=g_Emu_Reg[4];
              	g_Emu_Reg[4]=buf08;
              	buf08=g_Emu_Reg[2];
              	g_Emu_Reg[2]=g_Emu_Reg[3];
              	g_Emu_Reg[3]=buf08;
            }
			for(j = 0; j < r; j++)
            {
               HG7220_uart_tx(g_Emu_Reg[j]);
               delay_ms(1);
            }
        }
		
        if(!OpMode)
        {
            if(g_Emu_Reg[0] >= 0x70) return 0;
        }
        else
        {
            if(g_Emu_Reg[0] >= 0xf0) return 0;
        }
					
		
        *tempBuf = reg_Addr;
		
        HG7220_uart_tx(*tempBuf);
        for(j = 1; j < (uint8_t)(n+1); j++)
        {
            m=100;
           	while(!UART_RXIF_REG)
            {
                m--;
              	delay_ms(1);
              	if(m==0)
                   break;
            }
			UART_RXIF_REG = 0;
			*(tempBuf + j) = HG7220_uart_rx();
        }
	
				
        for(i = 1; i < r; i++)
        {
            if(g_Emu_Reg[i] != *(tempBuf + i+ n - r+1))
            {
                break;
            }
        }
        if( i == r) 
        {
            g_Emu_Reg[0] &= B0111_1111;
            return 0;	                        //读写成功
        }	 
	}
	return 1;			                  //读写失败
}
#endif
/*****************计量芯片SPI写使能************************
函 数 名：  void Emu_En_Wr(void)                     
功能描述：  计量芯片SPI写使能        
输入参数：  无                                      
函数返回值：无
***********************************************************/
void Emu_En_Wr(void)
{
    g_Emu_Reg[0] = WPCMD;  //写保护寄存器写0XE5使能写操作
    g_Emu_Reg[1] = 0xE5;	
    Emu_Com_Op(WRITE);
}
/*****************计量芯片SPI写禁止************************
函 数 名：  void Emu_Dis_Wr(void)                    
功能描述：  计量芯片SPI写禁止        
输入参数：  无                                
函数返回值：无
***********************************************************/
void Emu_Dis_Wr(void)
{
    g_Emu_Reg[0] = WPCMD;  //写保护寄存器写0XDC禁止写操作
    g_Emu_Reg[1] = 0xDC;
    Emu_Com_Op(WRITE);
}

/************计量芯片SPI特殊寄存器(0x50)写使能*************
函 数 名：  void Emu_En_Wr_Special(void)
功能描述：  计量芯片SPI特殊寄存器(0x50)写使能
输入参数：  无
函数返回值：无
***********************************************************/
void Emu_En_Wr_Special(void)
{
    g_Emu_Reg[0] = 0x7f;//往0X7F寄存器写0X69，使能写特殊寄存器0X50
    g_Emu_Reg[1] = 0x69;
    Emu_Com_Op(WRITE);
}

/***********计量芯片SPI特殊寄存器(0x50)写禁止**************
函 数 名：  void Emu_Dis_Wr_Special(void)                    
功能描述：  计量芯片SPI特殊寄存器(0x50)写禁止       
输入参数：  无                                
函数返回值：无
***********************************************************/
void Emu_Dis_Wr_Special(void)
{
    g_Emu_Reg[0] = 0x7f;//往0X7F寄存器写0X96，禁止写特殊寄存器0X50
    g_Emu_Reg[1] = 0x96;			
    Emu_Com_Op(WRITE);
}




/***************计量芯片寄存器常数初始化******************
函 数 名：  void Emu_Initial(void)                  
功能描述：  计量芯片寄存器常数初始化      
输入参数：  无                                
函数返回值：无
***********************************************************/
void  Emu_initial(void)
{
    uint8_t  result;   

    Emu_En_Wr();  //计量芯片SPI写寄存器使能
    
    //PFSET设置(高频脉冲常数寄存器)		
    g_Emu_Reg[0] = PFSET;
    result = Emu_Com_Op(READ);
    if(result) 
        return;
    if((g_Emu_Reg[1] != PFSETHHB) ||(g_Emu_Reg[2] != PFSETHLB)
		|| (g_Emu_Reg[3] != PFSETLHB) ||(g_Emu_Reg[4] != PFSETLLB))
    {
        g_Emu_Reg[0] = PFSET;
        g_Emu_Reg[1] = PFSETHHB;
        g_Emu_Reg[2] = PFSETHLB;
        g_Emu_Reg[3] = PFSETLHB;
        g_Emu_Reg[4] = PFSETLLB;
        Emu_Com_Op(WRITE);
    }

    //EMOD设置(计量模式配置寄存器)
    g_Emu_Reg[0] = EMOD;
    result = Emu_Com_Op(READ);
    if(result) 
        return;
    if((g_Emu_Reg[1] != EMODHB) ||(g_Emu_Reg[2] != EMODLB))
    {
        g_Emu_Reg[0] = EMOD;
        g_Emu_Reg[1] = EMODHB;
        g_Emu_Reg[2] = EMODLB;
        Emu_Com_Op(WRITE);
    }

    //PGAC设置(各通道增益设置和HPF使能)
    g_Emu_Reg[0] = PGAC;
    result = Emu_Com_Op(READ);
    if(result) 
        return;
    if(g_Emu_Reg[1] != PGACB) 
    {
        g_Emu_Reg[0] = PGAC;
        g_Emu_Reg[1] = PGACB;
        Emu_Com_Op(WRITE);
    }	
	 //PSTART设置(启动功率门限设置)
    g_Emu_Reg[0] = PSTART;
    result = Emu_Com_Op(READ);
    if(result) 
        return;		
    if((g_Emu_Reg[1] != PSTARTHB) ||(g_Emu_Reg[2] != PSTARTLB))
    {
        g_Emu_Reg[0] = PSTART;
        g_Emu_Reg[1] = PSTARTHB;
        g_Emu_Reg[2] = PSTARTLB;
        Emu_Com_Op(WRITE);
    }
		
    Emu_Dis_Wr();   //计量芯片SPI写寄存器禁止

    Emu_En_Wr_Special();   //计量芯片特殊寄存器写使能
    //0x50寄存器写固定值
    g_Emu_Reg[0] = 0x50;
    result = Emu_Com_Op(READ);     				
    if(result) 
        return;
    if((g_Emu_Reg[1] != 0x00) ||(g_Emu_Reg[2] != 0xc8)
		    || (g_Emu_Reg[3] != 0x28))
    {
        g_Emu_Reg[0] = 0x50;
        g_Emu_Reg[1] = 0x00;
        g_Emu_Reg[2] = 0xc8;
        g_Emu_Reg[3] = 0x28;
        Emu_Com_Op(WRITE);
    }
    Emu_Dis_Wr_Special();   //计量芯片特殊寄存器写禁止

    Emu_En_Wr();			//开写保护
	
    g_Emu_Reg[0] = PAGAIN;
	  g_Emu_Reg[1] = gs_Emu_Para.u_PAGAIN_Para.B08[0];
	  g_Emu_Reg[2] = gs_Emu_Para.u_PAGAIN_Para.B08[1];      
    Emu_Com_Op(WRITE);

    g_Emu_Reg[0] = APHCAL;
    g_Emu_Reg[1] = gs_Emu_Para.u_APHCAL_Para.B08[0];
    Emu_Com_Op(WRITE);
    
    g_Emu_Reg[0] = PAOFF;
    g_Emu_Reg[1] = gs_Emu_Para.u_PAOFF_Para.B08[0];
    g_Emu_Reg[2] = gs_Emu_Para.u_PAOFF_Para.B08[1];
    Emu_Com_Op(WRITE);
	
    g_Emu_Reg[0] = UOFF;
    g_Emu_Reg[1] = gs_Emu_Para.u_UOFF_Para.B08[0];
    g_Emu_Reg[2] = gs_Emu_Para.u_UOFF_Para.B08[1];
    Emu_Com_Op(WRITE);    
    
    g_Emu_Reg[0] = UGAIN;
    g_Emu_Reg[1] = gs_Emu_Para.u_UGAIN_Para.B08[0];
    g_Emu_Reg[2] = gs_Emu_Para.u_UGAIN_Para.B08[1];
    Emu_Com_Op(WRITE);

    g_Emu_Reg[0] = IAOFF;
    g_Emu_Reg[1] = gs_Emu_Para.u_IAOFF_Para.B08[0];
    g_Emu_Reg[2] = gs_Emu_Para.u_IAOFF_Para.B08[1];
    Emu_Com_Op(WRITE);    
    
    g_Emu_Reg[0] = IAGAIN;
    g_Emu_Reg[1] = gs_Emu_Para.u_IAGAIN_Para.B08[0];
    g_Emu_Reg[2] = gs_Emu_Para.u_IAGAIN_Para.B08[1];
    Emu_Com_Op(WRITE);

	 g_Emu_Reg[0] = IBOFF;
    g_Emu_Reg[1] = gs_Emu_Para.u_IBOFF_Para.B08[0];
    g_Emu_Reg[2] = gs_Emu_Para.u_IBOFF_Para.B08[1];
    Emu_Com_Op(WRITE);

    g_Emu_Reg[0] = IBGAIN;
    g_Emu_Reg[1] = gs_Emu_Para.u_IBGAIN_Para.B08[0];
    g_Emu_Reg[2] = gs_Emu_Para.u_IBGAIN_Para.B08[1];
    Emu_Com_Op(WRITE);	
    
    //最后经行START设置(计量启动命令字)
    g_Emu_Reg[0] = START;
    result = Emu_Com_Op(READ);
    if(result) 
        return;
    if(g_Emu_Reg[1] != STARTB) 
    {
        g_Emu_Reg[0] = START;
        g_Emu_Reg[1] = STARTB;
        Emu_Com_Op(WRITE);
    }   
    Emu_Dis_Wr();	

}
    union    U_B16B08    u_temp16;
    union    U_B32B08    u_temp32;
    union    U_B32B08    u_temp32_i1;
    union    U_B32B08    u_temp32_u;
    union    U_B32B08    u_temp32_cosa;
    union    U_B32B08    u_temp32_p1;
void Read_Emu(void) 
{


    uint8_t   result;

        watchdog();
        //------读电压频率
        g_Emu_Reg[0] = FREQ;
        result = Emu_Com_Op(READ);
        if(result)
            return;
        u_temp32.B32 = 0;
        u_temp32.B08[0] = g_Emu_Reg[2];
        u_temp32.B08[1] = g_Emu_Reg[1];
        u_temp32.B32 *= 100;         //DATA*100/(2^8)   //保留两位小数
        u_temp32.B32 = u_temp32.B32 >> 8;
	  
        Hex2Bcd(u_temp32.B08,gs_Meter_Variable.u_Freq.B08,2); 
	  
        //------读功率因数
        g_Emu_Reg[0] = AFAC;
        result = Emu_Com_Op(READ);
        if(result)
            return;
        u_temp16.B08[0] = g_Emu_Reg[2];
        u_temp16.B08[1] = g_Emu_Reg[1];

        if(u_temp16.B08[1] & B1000_0000)
        {
            u_temp16.B16 = ~u_temp16.B16 + 1;
        }
	    u_temp32.B32 = ((uint32_t)(u_temp16.B16))*1000/32768;   //DATA*(10^3)/(2^15)    3位小数
        u_temp32_cosa.B32 = u_temp32.B32;       //暂存功率因数HEX码，后面计算小电流用
        Hex2Bcd_4(u_temp32.B08, gs_Meter_Variable.u_Cosa.B08); 
        if(gs_Meter_Variable.u_Cosa.B32 > 0x1000)	
        {
            gs_Meter_Variable.u_Cosa.B32 = 0x1000;
        }
    
        //------读有功功率	
        g_Emu_Reg[0] = PA_EMU;
        result = Emu_Com_Op(READ);
        if(result)
            return;
        u_temp32.B08[0] = g_Emu_Reg[4];
        u_temp32.B08[1] = g_Emu_Reg[3];
        u_temp32.B08[2] = g_Emu_Reg[2];
        u_temp32.B08[3] = g_Emu_Reg[1];
	
        if((g_Emu_Reg[1]&0x80)==0x80)   //如果为负，则取绝对值
            u_temp32.B32 = (~u_temp32.B32) + 1;
        u_temp32.B32 = (u_temp32.B32 * K_V) / (R_I * 1288.49018888);//P=(DATA*K_V)/(R_I*(10^-6)*Gi*Gu*(2^29))=(DATA*K_V)*10/(R_I*(10^-6)*24*1*(2^29))=(DATA*K_V)/(R_I*1288.49018888)  保留1位小数
        u_temp32_p1.B32 =  u_temp32.B32;   //暂存有功功率，后面计量小电流用 
        g_Pav_Temp += u_temp32.B32;   //累计每分钟平均功率值
        g_Pav_Cnt ++;    

        Hex2Bcd_4(u_temp32.B08, gs_Meter_Variable.u_P1.B08);		//1位小数,单位W

        //------读IA通道电流
        g_Emu_Reg[0] = IARMS;
        result = Emu_Com_Op(READ);
        if(result)
            return;
        u_temp32.B08[0] = g_Emu_Reg[3];  //大端转换成小端
        u_temp32.B08[1] = g_Emu_Reg[2];
        u_temp32.B08[2] = g_Emu_Reg[1];
        u_temp32.B08[3] = 0;
	
        u_temp32.B32 = (double)u_temp32.B32/(R_I * 0.201326592);  //IA=DATA/(R*Gi*2^23)=(DATA*10^3)/(350*10^(-6)*24*2^23)=DATA/(R_I*24*10^(-9)*8388608)=DATA/(R_I*0.201326592)
        u_temp32_i1.B32 =  u_temp32.B32;   //暂存A通道电流HEX码，后面计算视在功率
        Hex2Bcd_4(u_temp32.B08, gs_Meter_Variable.u_I1.B08);	 //3位小数
    
        if(gs_Meter_Variable.u_I1.B32 < 0X0200)    //如果读出的电流小于200mA I=P/(U*COSA)
        {
            u_temp32.B32 = (u_temp32_p1.B32 * 1000000)/(u_temp32_u.B32 * u_temp32_cosa.B32);
            u_temp32_i1.B32 =  u_temp32.B32;   //暂存A通道电流HEX码，后面计算视在功率
            Hex2Bcd_4(u_temp32.B08, gs_Meter_Variable.u_I1.B08);	 //3位小数
         }    
		
        //------读IB通道电流	  
        g_Emu_Reg[0] = IBRMS;
        result = Emu_Com_Op(READ);
        if(result)
            return;
        u_temp32.B08[0] = g_Emu_Reg[3];  //大端转换成小端
        u_temp32.B08[1] = g_Emu_Reg[2];
        u_temp32.B08[2] = g_Emu_Reg[1];
        u_temp32.B08[3] = 0;
	  
        u_temp32.B32=(double)u_temp32.B32/(RO_B*8388.608); //IB=DATA/(RO_B*Gi*2^23)=(DATA*10^3)/(RO_B*1*2^23)=DATA/(RO_B*10^(-3)*2^(23))=DATA/(R_I*8388.608)
        Hex2Bcd_4(u_temp32.B08,gs_Meter_Variable.u_I2.B08);		//3位小数
	  
        //------读电压通道
        g_Emu_Reg[0] = URMS;
        result = Emu_Com_Op(READ);
        if(result)
            return;
        u_temp32.B08[0] = g_Emu_Reg[3];   //大端转换成小端
        u_temp32.B08[1] = g_Emu_Reg[2];
        u_temp32.B08[2] = g_Emu_Reg[1];
        u_temp32.B08[3] = 0;
		
        u_temp32.B32 = u_temp32.B32*((double)K_V/838860.8);			//U=(DATA*K_V)/(Gu*2^23)=DATA*(K_V/8388608)		
        u_temp32_u.B32 =  u_temp32.B32;   //暂存电压HEX码，后面计算视在功率
        Hex2Bcd_4(u_temp32.B08, gs_Meter_Variable.u_U.B08); 
		
        //------视在功率计算  S = U * I
 
        u_temp32.B32 = u_temp32_u.B32 * u_temp32_i1.B32 / 1000;
        Hex2Bcd_4(u_temp32.B08, gs_Meter_Variable.u_P2.B08);

}



/*******获取功率误差(PAGAIN、APHCAL、PAOFF校准时使用)******
函 数 名：  double Get_Err_Data(uint8_t *Buf)                
功能描述：  计量芯片寄存器常数初始化      
输入参数：  无台体下发的功率首地址                                
函数返回值：当前误差
***********************************************************/

double Get_Err_Data(uint8_t *Buf)
{
    union    U_B32B08    u_temp32_p;      //台体下发功率
    union    U_B32B08    u_temp32;    
    double    err_data;                   //功率误差
    uint8_t   result;
    
    Bcd2Hex_4(&Buf[8], u_temp32_p.B08);	 //当前功率转换成HEX
    
    g_Emu_Reg[0] = PA_EMU;                //读当前功率
    result = Emu_Com_Op(READ);
    if(result) 
        return 0;
    gs_Meter_Variable.u_P1.B08[0] = g_Emu_Reg[4];
    gs_Meter_Variable.u_P1.B08[1] = g_Emu_Reg[3];
    gs_Meter_Variable.u_P1.B08[2] = g_Emu_Reg[2];
    gs_Meter_Variable.u_P1.B08[3] = g_Emu_Reg[1];
	
    if((g_Emu_Reg[1] & 0x80) == 0x80)   //如果是负数
        gs_Meter_Variable.u_P1.B32 = (~gs_Meter_Variable.u_P1.B32) + 1;  //取绝对值
	
    //P=(DATA*K_V)/(R_I*(10^-6)*Gi*Gu*(2^29))=(DATA*K_V)*10/(R_I*(10^-6)*24*1*(2^29))=(DATA*K_V)/(R_I*1288.49018888)  保留1位小数
    u_temp32.B32 = 100 * (gs_Meter_Variable.u_P1.B32 * K_V) / (R_I * 1288.49018888);  //乘以100，保留3位小数

    err_data = (double)u_temp32.B32/u_temp32_p.B32 - 1 ;  //误差计算：   (实际测量功率-台体理论功率)/(台体理论功率)
    return(err_data);
}


/*****************电压、电流有效值增益校准******************
函 数 名：  uint8_t UI_GAIN_Adjust(uint8_t *Buf)                       
功能描述：  电压、电流增益计算                    
输入参数：  台体下发数据地址                                           
函数返回值：是否成功                                          
***********************************************************/
uint8_t UI_GAIN_Adjust(uint8_t *Buf)
{
    double    err_data;   //误差值
    union    U_B32B08    u_temp32_Ib;  //台体电流缓存
    union    U_B32B08    u_temp32_Un;  //台体电压缓存
    union    U_B32B08    u_temp32;    //结果缓存
    uint8_t    result;
    uint8_t    i;
    Bcd2Hex_4(Buf, u_temp32_Un.B08);		//电压
    Bcd2Hex_4(&Buf[4], u_temp32_Ib.B08);  //电流

    Emu_En_Wr();   //计量芯片写使能
    g_Emu_Reg[0] = UGAIN; //寄存器初始化(清零)
    g_Emu_Reg[1] = 0;
    g_Emu_Reg[2] = 0;  
    result = Emu_Com_Op(WRITE);
    if(result)
        return FAILED;
        
    g_Emu_Reg[0] = IAGAIN;
    g_Emu_Reg[1] = 0;
    g_Emu_Reg[2] = 0;  
    result = Emu_Com_Op(WRITE);	
    if(result)
        return FAILED;
    delay_ms(400); //延时
    g_Emu_Reg[0] = URMS;   //读校准前电压
    result = Emu_Com_Op(READ);
    if(result)
        return FAILED;
    for(i = 0; i < 3; i++) 
        u_temp32.B08[2-i] = g_Emu_Reg[1+i];
    u_temp32.B08[3] = 0;
	
    //以上读到的值为u_temp32.B32， 再求G=(2^23 * Gu * Un)/(u_temp32.B32 * k) ,UAGAIN则为(G-1)*(2e15)的补码形式
    err_data = (double)(8388.608 * u_temp32_Un.B32) / (K_V * u_temp32.B32) - 1;   //G-1=(2^23*1*Un*10^(-3))/(u_temp32.B32 * k)=((2^23)*10^(-3)/k)*(Un/u_temp32.B32)-1
    
    if(err_data >= 0)    //转换成补码形式
        u_temp32.B32 = err_data * 32768;
    else 
        u_temp32.B32 = err_data * 32768 + 65536;
    g_Emu_Reg[0] = UGAIN;//写校准结果到计量芯片
    g_Emu_Reg[1] = u_temp32.B08[1];
    g_Emu_Reg[2] = u_temp32.B08[0];
    result = Emu_Com_Op(WRITE);
    if(result)
        return FAILED;

    gs_Emu_Para.u_UGAIN_Para.B08[0] = g_Emu_Reg[1];  //将校准结果更新到RAM
    gs_Emu_Para.u_UGAIN_Para.B08[1] = g_Emu_Reg[2];

    g_Emu_Reg[0] = IARMS;   //读电流
    result = Emu_Com_Op(READ);
    if(result) 
        return FAILED;        
    for(i = 0; i < 3; i++) //转换成小端
        u_temp32.B08[2-i] = g_Emu_Reg[1+i];
        
    u_temp32.B08[3] = 0;
	
    //以上读到值为u_temp32.B32，再求G=((2^23)*R_I*Gi*Ib)/(u_temp32.B32),UAGAIN则为(G-1)*(2e15)的补码形式
    err_data = (double)((201.326592 * R_I) * u_temp32_Ib.B32)/(1000 * u_temp32.B32) - 1;    //G-1=(2^23)*R_I*(10^-6)*24*(10^-3)*(Ib/u_temp32.B32)-1
    if(err_data >= 0)     //转换成补码形式
        u_temp32.B32 = err_data * 32768;
    else 
        u_temp32.B32 = err_data * 32768 + 65536;
	
    g_Emu_Reg[0] = IAGAIN;           //写校准结果到计量芯片
    g_Emu_Reg[1] = u_temp32.B08[1];
    g_Emu_Reg[2] = u_temp32.B08[0];
    result = Emu_Com_Op(WRITE);
    if(result) 
        return FAILED;
        
    gs_Emu_Para.u_IAGAIN_Para.B08[0] = g_Emu_Reg[1];  //将校准结果更新到RAM
    gs_Emu_Para.u_IAGAIN_Para.B08[1] = g_Emu_Reg[2];
    
    Emu_Dis_Wr();   //计量芯片写禁止

    return 0;
}
/************A线有功功率增益校准PAGAIN(100%Ib,1.0)**********
函 数 名：  uint8_t  PAGAIN_Adjust(uint8_t *Buf) 
功能描述：  A线有功功率增益校准PAGAIN(100%Ib,1.0) 
输入参数：  台体下发数据地址                                
函数返回值：是否成功                                    
***********************************************************/
uint8_t  PAGAIN_Adjust(uint8_t *Buf)
{
    union    U_B32B08    u_temp32;
    double   err_data;
    uint8_t   result;
    Emu_En_Wr();  //计量芯片写使能
    
    g_Emu_Reg[0] = PAGAIN; //寄存器初始化(清零)
    g_Emu_Reg[1] = 0; 
    g_Emu_Reg[2] = 0;  
    result = Emu_Com_Op(WRITE);	
    if(result)
        return FAILED;    

    err_data = Get_Err_Data(Buf);//计算误差
    err_data = -err_data/(1+err_data);  //误差调整为：G-1=(1/(1+err_data))-1
	
    //将(G-1)调整为*2^15的补码形式
    if(err_data >= 0) 
        u_temp32.B32 = 32768 * err_data;
    else 
        u_temp32.B32 = 65536 + 32768 * err_data;
        
    g_Emu_Reg[0] = PAGAIN;   //将校准结果写回计量芯片
    g_Emu_Reg[1] = u_temp32.B08[1];
    g_Emu_Reg[2] = u_temp32.B08[0];
    result = Emu_Com_Op(WRITE);
    if(result)
        return FAILED;     

    gs_Emu_Para.u_PAGAIN_Para.B08[0] = g_Emu_Reg[1];  //将校准结果更新到RAM
    gs_Emu_Para.u_PAGAIN_Para.B08[1] = g_Emu_Reg[2];
    Emu_Dis_Wr();

	return SUCCEED;	
}

/**************A线角差校准APHCAL(100%Ib,0.5L)***************
函 数 名：  uint8_t  PAGAIN_Adjust(uint8_t *Buf)                       
功能描述：  A线角差校准APHCAL(100%Ib,0.5L)             
输入参数：  台体下发数据地址                                          
函数返回值：是否成功                                           
***********************************************************/
uint8_t APHCAL_Adjust(uint8_t *Buf)
{
    union    U_B32B08  u_temp32;
    double   err_data;
    uint8_t   result;
	
    Emu_En_Wr();  //打开写使能
    g_Emu_Reg[0] = APHCAL; //校准寄存器初始化(清零)
    g_Emu_Reg[1] = 0;  
	 result = Emu_Com_Op(WRITE);	
    if(result)
        return FAILED; 

    err_data = Get_Err_Data(Buf);//计算误差
    err_data = err_data*3763.74;    //将误差调整为*3763.74的补码形式
    if(err_data >= 0) 
        u_temp32.B32 = err_data;
    else 
        u_temp32.B32 = (256 + err_data);

    g_Emu_Reg[0] = APHCAL;          //将校准结果写回计量芯片
    g_Emu_Reg[1] = u_temp32.B08[0];
    result = Emu_Com_Op(WRITE);
    if(result)
        return FAILED; 
   
    gs_Emu_Para.u_APHCAL_Para.B08[0] = g_Emu_Reg[1];  //更新校准结果到RAM
    Emu_Dis_Wr();
    return SUCCEED;
}

/**************A线有功功率失调校准(5%Ib,1.0)***************
函 数 名：  uint8_t  PAOFF_Adjust(uint8_t *Buf)                       
功能描述：  A线有功功率失调校准(5%Ib,1.0)            
输入参数：  台体下发数据地址                                          
函数返回值：是否成功                                          
***********************************************************/
uint8_t  PAOFF_Adjust(uint8_t *Buf)
{
    union  U_B32B08  u_temp32;
    union  U_B32B08  u_temp32_p;
    double  err_data;
    uint8_t  result;
    
    Bcd2Hex(&Buf[8], u_temp32_p.B08,4);		//当前功率转换成HEX
    Emu_En_Wr();    //打开写使能
    g_Emu_Reg[0] = PAOFF; //校准寄存器初始化(清零)
    g_Emu_Reg[1] = 0;  
    g_Emu_Reg[2] = 0; 
    result = Emu_Com_Op(WRITE);	
    if(result)
        return FAILED; 
    delay_ms(400);
    err_data = Get_Err_Data(Buf);	//计算误差
		
    if(err_data >= 0) 
    {
        u_temp32.B32 = (double)u_temp32_p.B32 * err_data / 32;	
        u_temp32.B32 = (~u_temp32.B32) + 1;
    }
    else 
        u_temp32.B32 = (double)u_temp32_p.B32 * (-err_data) / 16;
	
    g_Emu_Reg[0] = PAOFF;       //将校准结果写回到计量芯片
    g_Emu_Reg[1] = u_temp32.B08[1];
    g_Emu_Reg[2] = u_temp32.B08[0];
    result = Emu_Com_Op(WRITE);
    if(result)
        return FAILED; 
    
    gs_Emu_Para.u_PAOFF_Para.B08[0] = g_Emu_Reg[1];  //更新校准结果到RAM
    gs_Emu_Para.u_PAOFF_Para.B08[1] = g_Emu_Reg[2];

    Emu_Dis_Wr();  //计量芯片写禁止
    
    return SUCCEED;
}

/*****************零线电流有效值增益校准******************
函 数 名：  uint8_t IB_GAIN_Adjust(uint8_t *Buf)                       
功能描述：  零线电流有效值增益校准                   
输入参数：  台体下发数据地址                                           
函数返回值：是否成功                                           
***********************************************************/
uint8_t IB_GAIN_Adjust(uint8_t *Buf)
{
    double  err_data;
    union   U_B32B08  u_temp32_ib;
    union   U_B32B08  u_temp32;
	  uint8_t  result,i;

    Bcd2Hex(&Buf[4], u_temp32_ib.B08,4);  //台体下发的零线电流转换成HEX
	 Emu_En_Wr();   //打开计量写使能
    g_Emu_Reg[0] = IBGAIN; //校准寄存器初始化(清零)
    g_Emu_Reg[1] = 0; 
    g_Emu_Reg[2] = 0;
    result = Emu_Com_Op(WRITE);	
    if(result)
        return FAILED; 
    delay_ms(400);
	
	 g_Emu_Reg[0] = IBRMS;  //读当前B线电流
    result = Emu_Com_Op(READ);
    if(result) 
         return FAILED;
         
	 for(i=0;i<3;i++)     //转换成小端
        u_temp32.B08[2-i] = g_Emu_Reg[1+i];
    u_temp32.B08[3] = 0;
	
    //以上读到值为u_temp32.B32，再求G=((2^23)*R_I*Gi*Ib)/(u_temp32.B32),IBGAIN则为(G-1)*(2e15)的补码形式
    err_data = (double)((8388.608*RO_B)*u_temp32_ib.B32)/(u_temp32.B32)-1;     //G-1=(2^23)*RO_B*1*(10^-3)*(Ib/u_temp32.B32)-1
    if(err_data >= 0)    //转换成补码形式
        u_temp32.B32 = err_data*32768;
    else    
        u_temp32.B32 = err_data*32768+65536;    
    
    err_data = (double)((201.326592 * R_I) * u_temp32_ib.B32)/(1000 * u_temp32.B32) - 1;    //G-1=(2^23)*RO_B*1*(10^-3)*(Ib/u_temp32.B32)-1
    if(err_data >= 0)     //转换成补码形式
        u_temp32.B32 = err_data * 32768;
    else 
        u_temp32.B32 = err_data * 32768 + 65536;
	
	g_Emu_Reg[0] = IBGAIN;    //写校准结果到计量芯片
	g_Emu_Reg[1] = u_temp32.B08[1];
	g_Emu_Reg[2] = u_temp32.B08[0];
	result = Emu_Com_Op(WRITE);
   if(result)
       return FAILED;     

	gs_Emu_Para.u_IBGAIN_Para.B08[0] = g_Emu_Reg[1];//更新校准结果RAM
	gs_Emu_Para.u_IBGAIN_Para.B08[1] = g_Emu_Reg[2];
	Emu_Dis_Wr();

	return SUCCEED;
}

//**********************************************************/
