/*******************************
File:	UART
Description: Define all the function which related to the serial communicaitons.

*******************************/
#include "P111.h"
#include "uart.h"
#include "timer8n.h"
#include "tool.h"
#include "type.h"
#include "ap_data.h"

extern void DelayBit();
//const int8u Baud[2]={0xEA,0xF5}; 
const int8u Baud[5]={255,129,64,129,64};  //1200,2400,4800,9600,19200 ,38400fosc

void UartInit(void)						//Rcv function of Uart reset
{

   
   TX1M=1; 
   RX1M=1; 
   RX1EN=1;			//允许串口1 接收  
   //BR1R=129;
   //BRGH1=1;
   TX1EN=1;
   #ifndef METER
   SetBaud(1);//setbaud 3;
   #else
   SetBaud(1);
   #endif
	//RX1IE=1;
	RX1EN=1;			//允许串口1 接收 
	serial_data_init();
	//MMemSet((void *)&UartRecv_Buf, 0, sizeof(UARTRECV_BUFFER));
	
}

void UartIntSvr(void) 
{
	//static int16u asciiCode = 0;
    int8u P100= 0;
	 int8u temp;
	
		
	    temp = RX1B;
	   // P100=JY(temp);  
	    	 
 
	#if (UART_CHECK==EVEN)
	/*
	if(P100!=RX1R8)
		{
		UartRecv_Buf.UartRecv_flag = 0;	
		UartRecv_Buf.UartRecv_fail=1;	
		return;
		}
	*/
	#endif

		UartRecv_Buf.UartRecv_wait=0;
		UartRecv_Buf.UartRecv_timeout = UART_TIMEOUT;
      
		  
		if(UartRecv_Buf.UartRecv_flag == 0)
		{
		      
			UartRecv_Buf.UartRecv_flag = 1;
			UartRecv_Buf.UartRecv_Buf[0] = temp;
			UartRecv_Buf.UartRecv_Len = 1;
			UartRecv_Buf.UartRecv_finish = 0;
		}
		else
		{
		
			UartRecv_Buf.UartRecv_Buf[UartRecv_Buf.UartRecv_Len] = temp;		
			UartRecv_Buf.UartRecv_Len++;
			if(UartRecv_Buf.UartRecv_Len>=UART_BUFFER_LENGTH)
			 UartRecv_Buf.UartRecv_Len=0;
		}	 
	// #endif
		
		return;

 } 




#if 0
void EmergencyUartSend(int8u * pHead, int8u bytes, int8u errorCode)
// Disable Interrupts and send bytes by uart
// Using in Error Reports
{
	int8u bIE, bEIE;
	int8u i;
	int8u h;
	bIE = IE;
	bEIE = EIE;

	IE = 0;
	IE2 = 0;
	
	for(i=0;i<bytes;i++)
	{
		TI = 0;
		SBUF = *pHead;
		while(!TI);
		TI = 0;
		pHead++;
		WDTREL = 0;
	}
	
	h = errorCode>>4;
	if(h<=9)
		h += 0x30;
	else
		h += 0x37;

	SBUF = h;
	while(!TI);
	TI = 0;

	h = errorCode & 0x0F;
	if(h<=9)
		h += 0x30;
	else
		h += 0x37;

	SBUF = h;
	while(!TI);
	TI = 0;
	WDTREL = 0;

	EIE = bEIE;
	IE = bIE;
}
#endif
#if 0
int16u Hex2AscII(int8u num)
// convert a hex num into two ascii code to output by uart
{
	int16u str = 0;
	int8u h = 0;
	h = num>>4;
	if(h<=9)
		h += 0x30;
	else
		h += 0x37;
	*(int8u *)(&(str)) = h;
	
	h = num & 0x0F;
	if(h<=9)
		h += 0x30;
	else
		h += 0x37;
	str += h;
	return str;
}
#endif

int8u UartIntSend(int8u *pbuf,int8u len)
{
int8u counter;
int8u i=0;

int8u *d;
d=pbuf;

if(!UartRecv_Buf.UartRecv_wait)
{
counter=0;
UartRecv_Buf.UartRecv_timeout=0;
UartRecv_Buf.UartRecv_Len=0;
}
else
{
counter=UartRecv_Buf.UartRecv_Len;
}

if((UartRecv_Buf.UartRecv_Len+len)>=UART_BUFFER_LENGTH)
{
UartRecv_Buf.UartRecv_wait=0;
UartRecv_Buf.UartRecv_Len=0;
UartRecv_Buf.UartRecv_timeout=0;
counter=0;
}

while(i<len)
{

UartRecv_Buf.UartRecv_Buf[i+counter]=*d;
i++;
d++;

}
d=pbuf;
UartRecv_Buf.UartRecv_Len+=len;
UartRecv_Buf.UartRecv_timeout+=len*UART_TIMEOUT;
UartRecv_Buf.UartRecv_wait=1;
TX1R8=AccountOneNum0(0xFE);
TX1B=0xFE;
TX1EN=1;
//TX1IE=1;

}

int8u UartSendByte(int8u byte)
{

TX1EN=1;
#if UART_CHECK==EVEN
	//TX1R8=AccountOneNum0(byte);
#endif
#if UART_CHECK==ODD
	//TX1R8=!AccountOneNum0(byte);
#endif		
	TX1B=byte;	
	while(!TRMT1);
   DelayBit();
   DelayBit();
	return 0;
}

void TransData_Meter(int8u *pPayload, int8u pPayloadLen)
 {
 //int8u i=0;
   unsigned  char *d;
   d=pPayload;
     while(pPayloadLen > 0)
 	{
        UartSendByte(*d);
        d++;
	   // i++;	        
	    pPayloadLen--;	 
	   
	 }
	
	UartRecv_Buf.UartRecv_finish = 0;
	UartRecv_Buf.UartRecv_flag = 0;
	UartRecv_Buf.UartRecv_fail = 0;
	//UartRecv_Buf.UartRecv_wait = 1;
//	#ifndef METER
	UartRecv_Buf.UartRecv_timeout = UART_TIMEOUT*pPayloadLen;	   
	//#else
	//UartRecv_Buf.UartRecv_timeout = UART_TIMEOUT*40;
	//#endif
    
	 
 }

void SetBaud(int8u  baud)
{

	if(baud>4)
		{
		 return;
		}
  BR1R=Baud[baud];
  BRGH1=0;
   if(baud>2)
   BRGH1=1;

	  

}
 #if __DEBUG >0
void setbaud(void)
{
     int16u parameter;  
     blprintf("setBaud");
     getint16uparameter(&parameter, 1);   
     blprintf("parameter %d %x\r\n",parameter,parameter); 
     SetBaud(parameter);
}																		   
 #endif

int8u AccountOneNum0(int8u AccountData)
{
	int8u i,j=AccountData;
	for(i=0;i<7;i++)
	{
		AccountData >>= 1;		
		j += AccountData;
	}
	return (j&0x1);
}

int8u JiaoY(int8u ucJ)
{int8u ucA,ucB;
 ucB=0;
 for(ucA=0;ucA<8;ucA++)
 { if(ucJ&0x01) ucB++;
   ucJ>>=1;
 }
 if(ucB&0x01)  
  return 1;
 else 
return 0;	
}

void serial_data_init(void)
{
    MMemSet((int8u *)&UartRecv_Buf,0,sizeof(UartRecv_Buf));
   
}

void UartTick(void)
{
 if(!TRMT1)
{
  UartRecv_Buf.SendFlag=0;
  IntSendByte(); 
}
if(UartRecv_Buf.ReceiveFlag)
{
  UartRecv_Buf.ReceiveFlag=0;
  UartIntSvr();
}


}


void Uart_Receive_Deal(void)
{
  
  if(UartRecv_Buf.UartRecv_finish == 1)
	  {
           
		Decode_MeterData(READMETER_DATA);										
		UartRecv_Buf.UartRecv_finish = 0;
		UartRecv_Buf.UartRecv_Len = 0;
		UartRecv_Buf.UartRecv_flag=0;
    }
  if((UartRecv_Buf.UartRecv_flag == 1 )|| (UartRecv_Buf.UartRecv_wait == 1) )
		 {
		  if(UartRecv_Buf.UartRecv_timeout>0)
			UartRecv_Buf.UartRecv_timeout--;
		  else
		  {
		  
			   if(UartRecv_Buf.UartRecv_flag == 1)
				    {
				    UartRecv_Buf.UartRecv_finish=1;
				    UartRecv_Buf.UartRecv_flag=0;
				   }
				  if(UartRecv_Buf.UartRecv_wait == 1)
				  {
				    TX1IE=0;
                TX1EN =0;
                TX1IF=0;
				    UartRecv_Buf.UartRecv_wait=0;
                UartRecv_Buf.index=0;
             
				  }
		  
		  }
		  
	  }
		
}

void IntSendByte(void)
{
   int8u temp; 
  if(UartRecv_Buf.UartRecv_wait==1)
    
   {
     if(UartRecv_Buf.index>=UartRecv_Buf.UartRecv_Len)
     {
     UartRecv_Buf.UartRecv_wait=0;
     UartRecv_Buf.index=0;
     TX1IE=0;
    UartRecv_Buf.UartRecv_timeout=0;
     }
     else
     {
			
			  #if UART_CHECK==EVEN
			  TX1R8 =JiaoY(UartRecv_Buf.UartRecv_Buf[UartRecv_Buf.index]);
			  #endif
			  TX1B=UartRecv_Buf.UartRecv_Buf[UartRecv_Buf.index++]; 
        
     }
   }
   else
   TX1IE=0;
}

