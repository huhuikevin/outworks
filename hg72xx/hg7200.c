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

struct S_EmuParameter gs_Emu_Para;   //����оƬ��У����,��Ҫ����洢���������ݣ�������δ洢���û��ɾ�ʵ�ʿ�������������
struct  S_MeterVariable  gs_Meter_Variable; 
union  U_220VFlag   gu_220V_Flag;
float		g_Pav_Temp;	//����1����ƽ������
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

void Emu_Reg_Verify(void)			//����оƬ�Ĵ���У��
{;}
//********************************************************/
//**************EMU Test Program*********************/
//********************************************************/

/*****************����оƬSPI�����̶���ʱ*******************
�� �� ����  void Emu_Com_Op_Delay(void)                       
����������  ϵͳ��ʱ                           
���������  ����оƬSPI�����̶���ʱ                                       
��������ֵ����
***********************************************************/
void Emu_Com_Op_Delay(void)
{
	uint8_t  i;
	for(i = 0; i < 156; i++);
}

/*****************����оƬSPI�����̶���ʱ*******************
�� �� ����  void Emu_Com_Op_Delay(void)                       
����������  ϵͳ��ʱ                           
���������  ����оƬSPI�����̶���ʱ                                       
��������ֵ����
***********************************************************/
void Emu_Com_Op_Delay1(void)
{
	uint8_t  i;
	for(i = 0; i < 65; i++);
}
#if (CONFIG_HG7220_MODE==0)       //ѡ�����оƬ�˿�ͨ�ŷ�ʽ
/*****************����оƬSPI��д����**********************
�� �� ����  uint8_t Emu_Com_Op(uint8_t OpMode)                       
����������  ����оƬSPI��д����         
���������  ����ģʽ������д                                       
��������ֵ����
***********************************************************/
uint8_t Emu_Com_Op(uint8_t OpMode) 	//��������,0=>Write,1=>Read
					                     //�����ļĴ�����ַ,����������ֽ�,�������м��ֽ�,����������ֽ�
											   //����оƬ����Ϊ��˸�ʽ�����磺 *g_Emu_Reg->OpAddr,*(g_Emu_Reg+1)->MSB,*(g_Emu_Reg+2)->MidSB,*(g_Emu_Reg+3)->LSB
				        
				
{
    uint8_t  i,j,tempbuf[5],k,buf08;
    uint8_t  r,n,reg_addr;
	
	 memset00(tempbuf, 5);   //��ȶԼĴ���
	
    if(g_Emu_Reg[0] <= 0x0C)       //��ַ0X00~0X0C��Ϊ8λ�Ĵ���
        r = 1 + 1;
    else if((g_Emu_Reg[0] >= 0x10) && (g_Emu_Reg[0] <= 0x2F))  //��ַ0X10~0X2F��Ϊ16λ�Ĵ���
        r = 2 + 1;
    else if((g_Emu_Reg[0] >= 0x30) && (g_Emu_Reg[0] <= 0x32))  //��ַ0X30~0X32��Ϊ24λ�Ĵ���
        r = 3 + 1;
    else if((g_Emu_Reg[0] >= 0x40) && (g_Emu_Reg[0] <= 0x46))  //��ַ0X40~0X46��Ϊ32λ�Ĵ���
        r = 4 + 1;
    else if(g_Emu_Reg[0] == 0x50) //����Ĵ���
        r = 3 + 1;
    else if(g_Emu_Reg[0] == 0x70) //д�����Ĵ���
        r = 1 + 1;
    else if(g_Emu_Reg[0] == 0x71) //оƬID
        r = 2 + 1;
    else if(g_Emu_Reg[0] == 0x72) //��һ��SPIд�������
        r = 4 + 1;
    else if(g_Emu_Reg[0] == 0x73) //��һ��SPI����������
        r = 4 + 1;
    else if(g_Emu_Reg[0] == 0x7f) //����Ĵ���д�����Ĵ���
        r = 1 + 1;
    else 
        return 1;    //����

	 if(OpMode)
	 {
        reg_addr = 0x72;	//��һ��д����
        n = 4;
    }
	 else
    {
        reg_addr = 0x73;	//��һ�ζ�����
        n = 4;
    }

    for(k = 0; k < 3; k++)
    {
        watchdog();    //�幷
 		
        SPICS_EN;       //CSʹ��
        Emu_Com_Op_Delay();
        g_Emu_Reg[0] |= OpMode;	//���ö�д����		
		
        if(!OpMode)	//����Ƕ�����
        {
            for(i = 1; i < r; i++) //����ջ���
                g_Emu_Reg[i] = 0;
            for(i = 0; i < 8; i++) //���͵�ַ
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
						
            for(j = 1; j < r; j++) //��������
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
        else		//�����д����
        {
			
            for(j = 0; j < r; j++)  //���͵�ַ������
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
        SPICS_DIS;	  //����оƬ��ֹ
        Emu_Com_Op_Delay();
		

        if(!OpMode)    //����Ƕ�����
        {
            if(g_Emu_Reg[0] >= 0x70) 
                return 0;
        }
        else           //�����д����
        {
            if(g_Emu_Reg[0] >= 0xf0) 
                return 0;
        }
					
        SPICS_EN;     //����оƬʹ��
        Emu_Com_Op_Delay();
		
        *tempbuf = reg_addr;  //�ո�SPIд�����������ݵ�ַ
		
        for(i = 0; i < 8; i++)//���͵�ַ
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
        for(j = 1; j < (uint8_t)(n+1); j++) //��ȡ����
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
        SPICS_DIS;	//����оƬ��ֹ
        Emu_Com_Op_Delay();
				
        for(i = 1; i < r; i++)  //�Ƚ������Ƿ�һ��
        {
            if(g_Emu_Reg[i] != *(tempbuf + i + n - r + 1))  //72H��73H�Ĵ�����ֵΪ��λ���뷽ʽ
            {
                break;
            }
        }
        if( i == r)  //����ȫ��һ��
        {
            g_Emu_Reg[0] &= B0111_1111;
            return 0;	//��д�ɹ�
        }	 
	}
	return 1;			//��дʧ��
}
#else
/*****************��żУ�麯�� ***********************************
�� �� ����  uint8_t Check_OddEven1(uint8_t Data)      
����������  �ж�������ż                     
���������  ��������                        
��������ֵ����żλ                                
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
/*****************����оƬ����urat���ͺ��� **************************
�� �� ����  void HG7220_uart_tx(uint8_t senddata)      
����������  ���ڷ�������                     
���������  ����������                        
��������ֵ����                                
******************************************************************/
void HG7220_uart_tx(uint8_t senddata)
{
	uint8_t temp08;
 	temp08  = Check_OddEven1(senddata);//��żУ��
  
	UART_TX8_REG = temp08;  
	UART_TX_REG = senddata; 
	delay_ms(1);
	while (!UART_TRMT_REG);
	return;
}
/*****************������оƬ����urat���պ��� *********************
�� �� ����  uint8_t HG7220_uart_rx(void)     
����������  ���ڽ�������                    
���������  ��                     
��������ֵ�����յ�����                                
******************************************************************/
uint8_t HG7220_uart_rx(void) 
{ 
    uint8_t    recvdata;
    recvdata = UART_RX_REG;      //��UART2���ݼĴ�����ȡ��������  
   	return(recvdata);
}

/*****************����оƬͨ�Ŷ�д���� *********************
�� �� ����  uint8_t ATT7051SPIOp(uint8_t OpMode)     
����������  ʵ�ּ���оƬ���ݵĶ�д����                    
���������  ����д������ַ                     
��������ֵ��success or fail                                
******************************************************************/
uint8_t Emu_Com_Op(uint8_t OpMode 	//��������,0=>Write,1=>Read
					            //�����ļĴ�����ַ,����������ֽ�,�������м��ֽ�,����������ֽ�								   //*OpData->OpAddr,*(Opdata+1)->MSB,*(OpData+2)->MidSB,*(OpData+3)->LSB
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
	else if(g_Emu_Reg[0] == 0x7f) r = 1+1;      //��������
	else return 1;                              //����

	if(OpMode)
	{
        reg_Addr = 0x72;	                     //��һ��д����
        n = 4;
    }
	else
    {
        reg_Addr = 0x73;	                     //��һ�ζ�����
        n = 4;
    }

    for(k = 0; k < 3; k++)
    {
        watchdog();
        g_Emu_Reg[0] |= OpMode;			   //��д����
		
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
            return 0;	                        //��д�ɹ�
        }	 
	}
	return 1;			                  //��дʧ��
}
#endif
/*****************����оƬSPIдʹ��************************
�� �� ����  void Emu_En_Wr(void)                     
����������  ����оƬSPIдʹ��        
���������  ��                                      
��������ֵ����
***********************************************************/
void Emu_En_Wr(void)
{
    g_Emu_Reg[0] = WPCMD;  //д�����Ĵ���д0XE5ʹ��д����
    g_Emu_Reg[1] = 0xE5;	
    Emu_Com_Op(WRITE);
}
/*****************����оƬSPIд��ֹ************************
�� �� ����  void Emu_Dis_Wr(void)                    
����������  ����оƬSPIд��ֹ        
���������  ��                                
��������ֵ����
***********************************************************/
void Emu_Dis_Wr(void)
{
    g_Emu_Reg[0] = WPCMD;  //д�����Ĵ���д0XDC��ֹд����
    g_Emu_Reg[1] = 0xDC;
    Emu_Com_Op(WRITE);
}

/************����оƬSPI����Ĵ���(0x50)дʹ��*************
�� �� ����  void Emu_En_Wr_Special(void)
����������  ����оƬSPI����Ĵ���(0x50)дʹ��
���������  ��
��������ֵ����
***********************************************************/
void Emu_En_Wr_Special(void)
{
    g_Emu_Reg[0] = 0x7f;//��0X7F�Ĵ���д0X69��ʹ��д����Ĵ���0X50
    g_Emu_Reg[1] = 0x69;
    Emu_Com_Op(WRITE);
}

/***********����оƬSPI����Ĵ���(0x50)д��ֹ**************
�� �� ����  void Emu_Dis_Wr_Special(void)                    
����������  ����оƬSPI����Ĵ���(0x50)д��ֹ       
���������  ��                                
��������ֵ����
***********************************************************/
void Emu_Dis_Wr_Special(void)
{
    g_Emu_Reg[0] = 0x7f;//��0X7F�Ĵ���д0X96����ֹд����Ĵ���0X50
    g_Emu_Reg[1] = 0x96;			
    Emu_Com_Op(WRITE);
}




/***************����оƬ�Ĵ���������ʼ��******************
�� �� ����  void Emu_Initial(void)                  
����������  ����оƬ�Ĵ���������ʼ��      
���������  ��                                
��������ֵ����
***********************************************************/
void  Emu_initial(void)
{
    uint8_t  result;   

    Emu_En_Wr();  //����оƬSPIд�Ĵ���ʹ��
    
    //PFSET����(��Ƶ���峣���Ĵ���)		
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

    //EMOD����(����ģʽ���üĴ���)
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

    //PGAC����(��ͨ���������ú�HPFʹ��)
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
	 //PSTART����(����������������)
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
		
    Emu_Dis_Wr();   //����оƬSPIд�Ĵ�����ֹ

    Emu_En_Wr_Special();   //����оƬ����Ĵ���дʹ��
    //0x50�Ĵ���д�̶�ֵ
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
    Emu_Dis_Wr_Special();   //����оƬ����Ĵ���д��ֹ

    Emu_En_Wr();			//��д����
	
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
    
    //�����START����(��������������)
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
        //------����ѹƵ��
        g_Emu_Reg[0] = FREQ;
        result = Emu_Com_Op(READ);
        if(result)
            return;
        u_temp32.B32 = 0;
        u_temp32.B08[0] = g_Emu_Reg[2];
        u_temp32.B08[1] = g_Emu_Reg[1];
        u_temp32.B32 *= 100;         //DATA*100/(2^8)   //������λС��
        u_temp32.B32 = u_temp32.B32 >> 8;
	  
        Hex2Bcd(u_temp32.B08,gs_Meter_Variable.u_Freq.B08,2); 
	  
        //------����������
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
	    u_temp32.B32 = ((uint32_t)(u_temp16.B16))*1000/32768;   //DATA*(10^3)/(2^15)    3λС��
        u_temp32_cosa.B32 = u_temp32.B32;       //�ݴ湦������HEX�룬�������С������
        Hex2Bcd_4(u_temp32.B08, gs_Meter_Variable.u_Cosa.B08); 
        if(gs_Meter_Variable.u_Cosa.B32 > 0x1000)	
        {
            gs_Meter_Variable.u_Cosa.B32 = 0x1000;
        }
    
        //------���й�����	
        g_Emu_Reg[0] = PA_EMU;
        result = Emu_Com_Op(READ);
        if(result)
            return;
        u_temp32.B08[0] = g_Emu_Reg[4];
        u_temp32.B08[1] = g_Emu_Reg[3];
        u_temp32.B08[2] = g_Emu_Reg[2];
        u_temp32.B08[3] = g_Emu_Reg[1];
	
        if((g_Emu_Reg[1]&0x80)==0x80)   //���Ϊ������ȡ����ֵ
            u_temp32.B32 = (~u_temp32.B32) + 1;
        u_temp32.B32 = (u_temp32.B32 * K_V) / (R_I * 1288.49018888);//P=(DATA*K_V)/(R_I*(10^-6)*Gi*Gu*(2^29))=(DATA*K_V)*10/(R_I*(10^-6)*24*1*(2^29))=(DATA*K_V)/(R_I*1288.49018888)  ����1λС��
        u_temp32_p1.B32 =  u_temp32.B32;   //�ݴ��й����ʣ��������С������ 
        g_Pav_Temp += u_temp32.B32;   //�ۼ�ÿ����ƽ������ֵ
        g_Pav_Cnt ++;    

        Hex2Bcd_4(u_temp32.B08, gs_Meter_Variable.u_P1.B08);		//1λС��,��λW

        //------��IAͨ������
        g_Emu_Reg[0] = IARMS;
        result = Emu_Com_Op(READ);
        if(result)
            return;
        u_temp32.B08[0] = g_Emu_Reg[3];  //���ת����С��
        u_temp32.B08[1] = g_Emu_Reg[2];
        u_temp32.B08[2] = g_Emu_Reg[1];
        u_temp32.B08[3] = 0;
	
        u_temp32.B32 = (double)u_temp32.B32/(R_I * 0.201326592);  //IA=DATA/(R*Gi*2^23)=(DATA*10^3)/(350*10^(-6)*24*2^23)=DATA/(R_I*24*10^(-9)*8388608)=DATA/(R_I*0.201326592)
        u_temp32_i1.B32 =  u_temp32.B32;   //�ݴ�Aͨ������HEX�룬����������ڹ���
        Hex2Bcd_4(u_temp32.B08, gs_Meter_Variable.u_I1.B08);	 //3λС��
    
        if(gs_Meter_Variable.u_I1.B32 < 0X0200)    //��������ĵ���С��200mA I=P/(U*COSA)
        {
            u_temp32.B32 = (u_temp32_p1.B32 * 1000000)/(u_temp32_u.B32 * u_temp32_cosa.B32);
            u_temp32_i1.B32 =  u_temp32.B32;   //�ݴ�Aͨ������HEX�룬����������ڹ���
            Hex2Bcd_4(u_temp32.B08, gs_Meter_Variable.u_I1.B08);	 //3λС��
         }    
		
        //------��IBͨ������	  
        g_Emu_Reg[0] = IBRMS;
        result = Emu_Com_Op(READ);
        if(result)
            return;
        u_temp32.B08[0] = g_Emu_Reg[3];  //���ת����С��
        u_temp32.B08[1] = g_Emu_Reg[2];
        u_temp32.B08[2] = g_Emu_Reg[1];
        u_temp32.B08[3] = 0;
	  
        u_temp32.B32=(double)u_temp32.B32/(RO_B*8388.608); //IB=DATA/(RO_B*Gi*2^23)=(DATA*10^3)/(RO_B*1*2^23)=DATA/(RO_B*10^(-3)*2^(23))=DATA/(R_I*8388.608)
        Hex2Bcd_4(u_temp32.B08,gs_Meter_Variable.u_I2.B08);		//3λС��
	  
        //------����ѹͨ��
        g_Emu_Reg[0] = URMS;
        result = Emu_Com_Op(READ);
        if(result)
            return;
        u_temp32.B08[0] = g_Emu_Reg[3];   //���ת����С��
        u_temp32.B08[1] = g_Emu_Reg[2];
        u_temp32.B08[2] = g_Emu_Reg[1];
        u_temp32.B08[3] = 0;
		
        u_temp32.B32 = u_temp32.B32*((double)K_V/838860.8);			//U=(DATA*K_V)/(Gu*2^23)=DATA*(K_V/8388608)		
        u_temp32_u.B32 =  u_temp32.B32;   //�ݴ��ѹHEX�룬����������ڹ���
        Hex2Bcd_4(u_temp32.B08, gs_Meter_Variable.u_U.B08); 
		
        //------���ڹ��ʼ���  S = U * I
 
        u_temp32.B32 = u_temp32_u.B32 * u_temp32_i1.B32 / 1000;
        Hex2Bcd_4(u_temp32.B08, gs_Meter_Variable.u_P2.B08);

}



/*******��ȡ�������(PAGAIN��APHCAL��PAOFFУ׼ʱʹ��)******
�� �� ����  double Get_Err_Data(uint8_t *Buf)                
����������  ����оƬ�Ĵ���������ʼ��      
���������  ��̨���·��Ĺ����׵�ַ                                
��������ֵ����ǰ���
***********************************************************/

double Get_Err_Data(uint8_t *Buf)
{
    union    U_B32B08    u_temp32_p;      //̨���·�����
    union    U_B32B08    u_temp32;    
    double    err_data;                   //�������
    uint8_t   result;
    
    Bcd2Hex_4(&Buf[8], u_temp32_p.B08);	 //��ǰ����ת����HEX
    
    g_Emu_Reg[0] = PA_EMU;                //����ǰ����
    result = Emu_Com_Op(READ);
    if(result) 
        return 0;
    gs_Meter_Variable.u_P1.B08[0] = g_Emu_Reg[4];
    gs_Meter_Variable.u_P1.B08[1] = g_Emu_Reg[3];
    gs_Meter_Variable.u_P1.B08[2] = g_Emu_Reg[2];
    gs_Meter_Variable.u_P1.B08[3] = g_Emu_Reg[1];
	
    if((g_Emu_Reg[1] & 0x80) == 0x80)   //����Ǹ���
        gs_Meter_Variable.u_P1.B32 = (~gs_Meter_Variable.u_P1.B32) + 1;  //ȡ����ֵ
	
    //P=(DATA*K_V)/(R_I*(10^-6)*Gi*Gu*(2^29))=(DATA*K_V)*10/(R_I*(10^-6)*24*1*(2^29))=(DATA*K_V)/(R_I*1288.49018888)  ����1λС��
    u_temp32.B32 = 100 * (gs_Meter_Variable.u_P1.B32 * K_V) / (R_I * 1288.49018888);  //����100������3λС��

    err_data = (double)u_temp32.B32/u_temp32_p.B32 - 1 ;  //�����㣺   (ʵ�ʲ�������-̨�����۹���)/(̨�����۹���)
    return(err_data);
}


/*****************��ѹ��������Чֵ����У׼******************
�� �� ����  uint8_t UI_GAIN_Adjust(uint8_t *Buf)                       
����������  ��ѹ�������������                    
���������  ̨���·����ݵ�ַ                                           
��������ֵ���Ƿ�ɹ�                                          
***********************************************************/
uint8_t UI_GAIN_Adjust(uint8_t *Buf)
{
    double    err_data;   //���ֵ
    union    U_B32B08    u_temp32_Ib;  //̨���������
    union    U_B32B08    u_temp32_Un;  //̨���ѹ����
    union    U_B32B08    u_temp32;    //�������
    uint8_t    result;
    uint8_t    i;
    Bcd2Hex_4(Buf, u_temp32_Un.B08);		//��ѹ
    Bcd2Hex_4(&Buf[4], u_temp32_Ib.B08);  //����

    Emu_En_Wr();   //����оƬдʹ��
    g_Emu_Reg[0] = UGAIN; //�Ĵ�����ʼ��(����)
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
    delay_ms(400); //��ʱ
    g_Emu_Reg[0] = URMS;   //��У׼ǰ��ѹ
    result = Emu_Com_Op(READ);
    if(result)
        return FAILED;
    for(i = 0; i < 3; i++) 
        u_temp32.B08[2-i] = g_Emu_Reg[1+i];
    u_temp32.B08[3] = 0;
	
    //���϶�����ֵΪu_temp32.B32�� ����G=(2^23 * Gu * Un)/(u_temp32.B32 * k) ,UAGAIN��Ϊ(G-1)*(2e15)�Ĳ�����ʽ
    err_data = (double)(8388.608 * u_temp32_Un.B32) / (K_V * u_temp32.B32) - 1;   //G-1=(2^23*1*Un*10^(-3))/(u_temp32.B32 * k)=((2^23)*10^(-3)/k)*(Un/u_temp32.B32)-1
    
    if(err_data >= 0)    //ת���ɲ�����ʽ
        u_temp32.B32 = err_data * 32768;
    else 
        u_temp32.B32 = err_data * 32768 + 65536;
    g_Emu_Reg[0] = UGAIN;//дУ׼���������оƬ
    g_Emu_Reg[1] = u_temp32.B08[1];
    g_Emu_Reg[2] = u_temp32.B08[0];
    result = Emu_Com_Op(WRITE);
    if(result)
        return FAILED;

    gs_Emu_Para.u_UGAIN_Para.B08[0] = g_Emu_Reg[1];  //��У׼������µ�RAM
    gs_Emu_Para.u_UGAIN_Para.B08[1] = g_Emu_Reg[2];

    g_Emu_Reg[0] = IARMS;   //������
    result = Emu_Com_Op(READ);
    if(result) 
        return FAILED;        
    for(i = 0; i < 3; i++) //ת����С��
        u_temp32.B08[2-i] = g_Emu_Reg[1+i];
        
    u_temp32.B08[3] = 0;
	
    //���϶���ֵΪu_temp32.B32������G=((2^23)*R_I*Gi*Ib)/(u_temp32.B32),UAGAIN��Ϊ(G-1)*(2e15)�Ĳ�����ʽ
    err_data = (double)((201.326592 * R_I) * u_temp32_Ib.B32)/(1000 * u_temp32.B32) - 1;    //G-1=(2^23)*R_I*(10^-6)*24*(10^-3)*(Ib/u_temp32.B32)-1
    if(err_data >= 0)     //ת���ɲ�����ʽ
        u_temp32.B32 = err_data * 32768;
    else 
        u_temp32.B32 = err_data * 32768 + 65536;
	
    g_Emu_Reg[0] = IAGAIN;           //дУ׼���������оƬ
    g_Emu_Reg[1] = u_temp32.B08[1];
    g_Emu_Reg[2] = u_temp32.B08[0];
    result = Emu_Com_Op(WRITE);
    if(result) 
        return FAILED;
        
    gs_Emu_Para.u_IAGAIN_Para.B08[0] = g_Emu_Reg[1];  //��У׼������µ�RAM
    gs_Emu_Para.u_IAGAIN_Para.B08[1] = g_Emu_Reg[2];
    
    Emu_Dis_Wr();   //����оƬд��ֹ

    return 0;
}
/************A���й���������У׼PAGAIN(100%Ib,1.0)**********
�� �� ����  uint8_t  PAGAIN_Adjust(uint8_t *Buf) 
����������  A���й���������У׼PAGAIN(100%Ib,1.0) 
���������  ̨���·����ݵ�ַ                                
��������ֵ���Ƿ�ɹ�                                    
***********************************************************/
uint8_t  PAGAIN_Adjust(uint8_t *Buf)
{
    union    U_B32B08    u_temp32;
    double   err_data;
    uint8_t   result;
    Emu_En_Wr();  //����оƬдʹ��
    
    g_Emu_Reg[0] = PAGAIN; //�Ĵ�����ʼ��(����)
    g_Emu_Reg[1] = 0; 
    g_Emu_Reg[2] = 0;  
    result = Emu_Com_Op(WRITE);	
    if(result)
        return FAILED;    

    err_data = Get_Err_Data(Buf);//�������
    err_data = -err_data/(1+err_data);  //������Ϊ��G-1=(1/(1+err_data))-1
	
    //��(G-1)����Ϊ*2^15�Ĳ�����ʽ
    if(err_data >= 0) 
        u_temp32.B32 = 32768 * err_data;
    else 
        u_temp32.B32 = 65536 + 32768 * err_data;
        
    g_Emu_Reg[0] = PAGAIN;   //��У׼���д�ؼ���оƬ
    g_Emu_Reg[1] = u_temp32.B08[1];
    g_Emu_Reg[2] = u_temp32.B08[0];
    result = Emu_Com_Op(WRITE);
    if(result)
        return FAILED;     

    gs_Emu_Para.u_PAGAIN_Para.B08[0] = g_Emu_Reg[1];  //��У׼������µ�RAM
    gs_Emu_Para.u_PAGAIN_Para.B08[1] = g_Emu_Reg[2];
    Emu_Dis_Wr();

	return SUCCEED;	
}

/**************A�߽ǲ�У׼APHCAL(100%Ib,0.5L)***************
�� �� ����  uint8_t  PAGAIN_Adjust(uint8_t *Buf)                       
����������  A�߽ǲ�У׼APHCAL(100%Ib,0.5L)             
���������  ̨���·����ݵ�ַ                                          
��������ֵ���Ƿ�ɹ�                                           
***********************************************************/
uint8_t APHCAL_Adjust(uint8_t *Buf)
{
    union    U_B32B08  u_temp32;
    double   err_data;
    uint8_t   result;
	
    Emu_En_Wr();  //��дʹ��
    g_Emu_Reg[0] = APHCAL; //У׼�Ĵ�����ʼ��(����)
    g_Emu_Reg[1] = 0;  
	 result = Emu_Com_Op(WRITE);	
    if(result)
        return FAILED; 

    err_data = Get_Err_Data(Buf);//�������
    err_data = err_data*3763.74;    //��������Ϊ*3763.74�Ĳ�����ʽ
    if(err_data >= 0) 
        u_temp32.B32 = err_data;
    else 
        u_temp32.B32 = (256 + err_data);

    g_Emu_Reg[0] = APHCAL;          //��У׼���д�ؼ���оƬ
    g_Emu_Reg[1] = u_temp32.B08[0];
    result = Emu_Com_Op(WRITE);
    if(result)
        return FAILED; 
   
    gs_Emu_Para.u_APHCAL_Para.B08[0] = g_Emu_Reg[1];  //����У׼�����RAM
    Emu_Dis_Wr();
    return SUCCEED;
}

/**************A���й�����ʧ��У׼(5%Ib,1.0)***************
�� �� ����  uint8_t  PAOFF_Adjust(uint8_t *Buf)                       
����������  A���й�����ʧ��У׼(5%Ib,1.0)            
���������  ̨���·����ݵ�ַ                                          
��������ֵ���Ƿ�ɹ�                                          
***********************************************************/
uint8_t  PAOFF_Adjust(uint8_t *Buf)
{
    union  U_B32B08  u_temp32;
    union  U_B32B08  u_temp32_p;
    double  err_data;
    uint8_t  result;
    
    Bcd2Hex(&Buf[8], u_temp32_p.B08,4);		//��ǰ����ת����HEX
    Emu_En_Wr();    //��дʹ��
    g_Emu_Reg[0] = PAOFF; //У׼�Ĵ�����ʼ��(����)
    g_Emu_Reg[1] = 0;  
    g_Emu_Reg[2] = 0; 
    result = Emu_Com_Op(WRITE);	
    if(result)
        return FAILED; 
    delay_ms(400);
    err_data = Get_Err_Data(Buf);	//�������
		
    if(err_data >= 0) 
    {
        u_temp32.B32 = (double)u_temp32_p.B32 * err_data / 32;	
        u_temp32.B32 = (~u_temp32.B32) + 1;
    }
    else 
        u_temp32.B32 = (double)u_temp32_p.B32 * (-err_data) / 16;
	
    g_Emu_Reg[0] = PAOFF;       //��У׼���д�ص�����оƬ
    g_Emu_Reg[1] = u_temp32.B08[1];
    g_Emu_Reg[2] = u_temp32.B08[0];
    result = Emu_Com_Op(WRITE);
    if(result)
        return FAILED; 
    
    gs_Emu_Para.u_PAOFF_Para.B08[0] = g_Emu_Reg[1];  //����У׼�����RAM
    gs_Emu_Para.u_PAOFF_Para.B08[1] = g_Emu_Reg[2];

    Emu_Dis_Wr();  //����оƬд��ֹ
    
    return SUCCEED;
}

/*****************���ߵ�����Чֵ����У׼******************
�� �� ����  uint8_t IB_GAIN_Adjust(uint8_t *Buf)                       
����������  ���ߵ�����Чֵ����У׼                   
���������  ̨���·����ݵ�ַ                                           
��������ֵ���Ƿ�ɹ�                                           
***********************************************************/
uint8_t IB_GAIN_Adjust(uint8_t *Buf)
{
    double  err_data;
    union   U_B32B08  u_temp32_ib;
    union   U_B32B08  u_temp32;
	  uint8_t  result,i;

    Bcd2Hex(&Buf[4], u_temp32_ib.B08,4);  //̨���·������ߵ���ת����HEX
	 Emu_En_Wr();   //�򿪼���дʹ��
    g_Emu_Reg[0] = IBGAIN; //У׼�Ĵ�����ʼ��(����)
    g_Emu_Reg[1] = 0; 
    g_Emu_Reg[2] = 0;
    result = Emu_Com_Op(WRITE);	
    if(result)
        return FAILED; 
    delay_ms(400);
	
	 g_Emu_Reg[0] = IBRMS;  //����ǰB�ߵ���
    result = Emu_Com_Op(READ);
    if(result) 
         return FAILED;
         
	 for(i=0;i<3;i++)     //ת����С��
        u_temp32.B08[2-i] = g_Emu_Reg[1+i];
    u_temp32.B08[3] = 0;
	
    //���϶���ֵΪu_temp32.B32������G=((2^23)*R_I*Gi*Ib)/(u_temp32.B32),IBGAIN��Ϊ(G-1)*(2e15)�Ĳ�����ʽ
    err_data = (double)((8388.608*RO_B)*u_temp32_ib.B32)/(u_temp32.B32)-1;     //G-1=(2^23)*RO_B*1*(10^-3)*(Ib/u_temp32.B32)-1
    if(err_data >= 0)    //ת���ɲ�����ʽ
        u_temp32.B32 = err_data*32768;
    else    
        u_temp32.B32 = err_data*32768+65536;    
    
    err_data = (double)((201.326592 * R_I) * u_temp32_ib.B32)/(1000 * u_temp32.B32) - 1;    //G-1=(2^23)*RO_B*1*(10^-3)*(Ib/u_temp32.B32)-1
    if(err_data >= 0)     //ת���ɲ�����ʽ
        u_temp32.B32 = err_data * 32768;
    else 
        u_temp32.B32 = err_data * 32768 + 65536;
	
	g_Emu_Reg[0] = IBGAIN;    //дУ׼���������оƬ
	g_Emu_Reg[1] = u_temp32.B08[1];
	g_Emu_Reg[2] = u_temp32.B08[0];
	result = Emu_Com_Op(WRITE);
   if(result)
       return FAILED;     

	gs_Emu_Para.u_IBGAIN_Para.B08[0] = g_Emu_Reg[1];//����У׼���RAM
	gs_Emu_Para.u_IBGAIN_Para.B08[1] = g_Emu_Reg[2];
	Emu_Dis_Wr();

	return SUCCEED;
}

//**********************************************************/
