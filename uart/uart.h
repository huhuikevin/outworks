/********************
uart.h
2011-04-25
********************/

#include "type.h"
/*-------------------------------------------------
Defines
-------------------------------------------------*/
#define UART_BUFFER_LENGTH 	200
#define BAUD 				9600		//UART Baud Rate
#define ASC_ MODE			1
#define HEX_MODE			0
#define __DEBUG 0

//#define COLLECTOR
#define TEST_FLASH
//#define CONTINUE_SEND
#define METER 
//#define MONITOR
#ifdef METER
#define RESENDTIMES 0
#else
#define RESENDTIMES 0
#endif
#define USART_SendData	UartSendByte
#define  UART_TIMEOUT  2//20ms   
#define NONE 	0										//无校验
#define EVEN 	1										//偶校验
#define ODD 	2										//奇校验
#define UART_CHECK EVEN

/*-------------------------------------------------
External Variances
-------------------------------------------------*/


typedef struct
{
	unsigned char UartRecv_wait:1;					//串口正在等待接收
	unsigned char UartRecv_fail:1;					//接收失败
	unsigned char UartRecv_flag:1;					//串口正在接收标识
	unsigned char UartRecv_finish:1;	                    //串口接收完成
	unsigned char  timeoutflag:1; 
   unsigned char  SendFlag:1; 
   unsigned char  ReceiveFlag:1; 
                                          //串口接收完成标志
	unsigned int UartRecv_timeout;			       //串口接收超时
	unsigned char  UartRecv_Len;					//串口接收长度	
	unsigned char index;
	unsigned char UartRecv_Buf[UART_BUFFER_LENGTH];		//串口接收buffer
	
	
}UARTRECV_BUFFER;

section10 UARTRECV_BUFFER  UartRecv_Buf@0X500;

/*-------------------------------------------------
External Interfaces
-------------------------------------------------*/
unsigned char UartRcv();				// check uart buffer in main routine. return 0: uart buf empty;	else: uart buffer received bytes.
void UartRcvResume();
void UartInit();
void UartTick(void);
int8u JiaoY(int8u ucJ);
void Uart_Receive_Deal(void);
unsigned char AccountOneNum0(unsigned char AccountData);
void serial_data_init(void);
unsigned char UartSendByte(unsigned char byte);
void EmergencyUartSend(unsigned char * pHead, unsigned char bytes, unsigned char errorCode);
void UartIntSvr(void) ;
unsigned char UartIntSend(unsigned char *pbuf,unsigned char len);
void TransData_Meter(unsigned char *pPayload, unsigned char pPayloadLen);
void SetBaud(unsigned char  baud);
void IntSendByte(void);
unsigned char UartIntSend(unsigned char *pbuf,unsigned char len);