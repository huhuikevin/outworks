/*******************************
File:	UART
Description: Define all the function which related to the serial communicaitons.

*******************************/
#include "P111.h"
#include "type.h"
#include "system.h"
#include "uart.h"
#include "timer8n.h"
#include "tool.h"
#include "config.h"

typedef struct
{
	uchar Recv_start:1;					//串口正在接收标识
	uchar Recv_finish:1;	                    //串口接收完成
    uchar inited;                                     //串口接收完成标志
	uchar Recv_Len;					//串口接收长度	
	uchar Recv_Buf[UART_BUFFER_LENGTH];		//串口接收buffer
}UARTRECV_BUFFER;



typedef struct {
	uchar RxB;
	uchar RxR8:1;
	uchar FERR:1;
	uchar OERR:1;
	uchar RRES:3;
	uchar RxM:1;
	uchar RxEN:1;
	uchar TxB;
	uchar TxR8:1;
	uchar TRMT:1;
	uchar TRES:3;
	uchar BRGH:1;
	uchar TxM:1;
	uchar TxEN:1;
	uchar Buad;
}uart_register;


#define UART_CHAR_SEND(i, b)\
do{\
	uart[i].TxEN = 1;\
	uart[i].TxB = b;\
	while(!uart[i].TRMT);\
}while(0);

#define UART_TIMEOUT 1//MS
#define UART_BUFFER_LENGTH 	MSG_MAX_LEN

//section64 volatile uart_register uart[3]@0xFFE0;
uart_register uart[3];
section10 UARTRECV_BUFFER  UartRecv[3]@0X500;


uchar Calc_baud(int16u baud)
{
	switch(baud)
	{
		case 2400:
			return 129;
		case 4800:
			return 54;
		case 9600:
			return 32;
		case 19200:
			return 17;
		case 38400:
			return 7;
		default:
			return 54;
	}
}
void UartInit(uchar uartIdx, int16u baudrate)						//Rcv function of Uart reset
{
	uart[uartIdx].TXM = 0;// 8bit data tx
	uart[uartIdx].RxM= 0;// 8bit data rx
	uart[uartIdx].RxEN= 1;
	uart[uartIdx].TxEN = 1;
	SetBaud(uartIdx, baudrate);
 
	serial_data_init(uartIdx);
	UartRecv[uartIdx].inited = 1;
}


#ifdef CONFIG_LINKLAY_UART
uchar uart_rx_bytes(uchar *pdata)
{
	uchar rlen = 0;
	if (UartRecv[CONFIG_LINKLAY_UART].Recv_finish)
	{
		rlen = UartRecv[CONFIG_LINKLAY_UART].Recv_Len-1;
		MMemcpy(pdata,&UartRecv[CONFIG_LINKLAY_UART]Recv_Buf[1],rlen)
		UartRecv[CONFIG_LINKLAY_UART].Recv_finish = 0;
	}
	return rlen;
}

uchar uart_tx_bytes(uchar *pdata, uchar len)
{
#if CONFIG_LINKLAY_UART==0
	return Uart0_Tx(pdata, len);
#endif

#if CONFIG_LINKLAY_UART==1
	return Uart1_Tx(pdata, len);
#endif

#if CONFIG_LINKLAY_UART==2
	return Uart2_Tx(pdata, len);
#endif
}
#endif


#ifdef CONFIG_CONSOLE
uchar console_uart_rx_bytes(uchar *pdata)
{
	uchar rlen = 0;
	if (UartRecv[CONFIG_CONSOLE_UART].Recv_finish)
	{
		rlen = UartRecv[CONFIG_CONSOLE_UART].Recv_Len-1;
		MMemcpy(pdata,&UartRecv[CONFIG_CONSOLE_UART]Recv_Buf[1],rlen)
		UartRecv[CONFIG_CONSOLE_UART].Recv_finish = 0;
	}
	return rlen;
}

uchar console_uart_tx_bytes(uchar *pdata, uchar len)
{
#if CONFIG_CONSOLE_UART==0
	return Uart0_Tx(pdata, len);
#endif

#if CONFIG_CONSOLE_UART==1
	return Uart1_Tx(pdata, len);
#endif

#if CONFIG_CONSOLE_UART==2
	return Uart2_Tx(pdata, len);
#endif
}

#endif

void uart_driver_process(void)
{
	Uart_Rx();
}


uchar Uart0_Tx(uchar *pdata, uchar len)
{
	uchar send = 0;
	UART_CHAR_SEND(0, 0xaa);
	UART_CHAR_SEND(0, len+2);

	do {
		UART_CHAR_SEND(0, pdata[send++]);
	}while(send < len);

	return len;

}

uchar Uart1_Tx(uchar *pdata, uchar len)
{
	uchar send = 0;
	UART_CHAR_SEND(1, 0xaa);
	UART_CHAR_SEND(1, len+2);

	do {
		UART_CHAR_SEND(1, pdata[send++]);
	}while(send < len);

	return len;

}

uchar Uart2_Tx(uchar *pdata, uchar len)
{
	uchar send = 0;
	UART_CHAR_SEND(2, 0xaa);
	UART_CHAR_SEND(2, len+2);

	do {
		UART_CHAR_SEND(2, pdata[send++]);
	}while(send < len);

	return len;

}

uchar Uart_Tx(uchar uartIdx, uchar *pdata, uchar len)
{
	if (uartIdx == 0)
		return Uart0_Tx(pdata,len);
	else if (uartIdx == 1)
		return Uart1_Tx(pdata,len);
	else
		return Uart2_Tx(pdata,len);
}


uchar Uart_Rx()
{
	if (UartRecv[0].inited)
		Uart0_Rx();
	if (UartRecv[1].inited)
		Uart1_Rx();
	if (UartRecv[2].inited)
		Uart2_Rx();
}


uchar Uart0_Rx()
{
	uchar tmpr,rlen;
	int16u curt;
	
	if (RX1IF == 0) {
		return 0;
	}
	UartRecv[0].UartRecv_start = 0;
	UartRecv[0].Recv_finish = 0;
	rlen = 0;
	do {
		RX1IF = 0;
		tmpr = uart[0].RxB;
		if (tmpr == 0xaa)
		{
			UartRecv[0].Recv_start = 1;
		}else{
			if (UartRecv[0].Recv_start){
				UartRecv[0].Recv_Buf[rlen++] = tmpr;
			}
		}
		if (rlen >= UART_BUFFER_LENGTH)
			return 0;
		if (rlen >= UartRecv[0].Recv_Buf[0]){//Recv_Buf[0] is data len
			break;
		}
		curt = Timetick();
		while(RX1IF == 0){
			if (IsTimeOut(curt+UART_TIMEOUT)){
				return 0;
			}
		}
	}while(1);
	UartRecv[0].Recv_Len = rlen;
	UartRecv[0].Recv_finish = 1;
}

uchar Uart1_Rx()
{
	uchar tmpr,rlen;
	int16u curt;
	
	if (RX1IF == 0) {
		return 0;
	}
	UartRecv[1].UartRecv_start = 0;
	UartRecv[1].Recv_finish = 0;
	rlen = 0;
	do {
		RX1IF = 0;
		tmpr = uart[1].RxB;
		if (tmpr == 0xaa)
		{
			UartRecv[0].Recv_start = 1;
		}else{
			if (UartRecv[1].Recv_start){
				UartRecv[1].Recv_Buf[rlen++] = tmpr;
			}
		}
		if (rlen >= UART_BUFFER_LENGTH)
			return 0;
		if (rlen >= UartRecv[1].Recv_Buf[0]){//Recv_Buf[0] is data len
			break;
		}
		curt = Timetick();
		while(RX1IF == 0){
			if (IsTimeOut(curt+UART_TIMEOUT)){
				return 0;
			}
		}
	}while(1);
	UartRecv[1].Recv_Len = rlen;
	UartRecv[1].Recv_finish = 1;
}

uchar Uart2_Rx()
{
	uchar tmpr,rlen;
	int16u curt;
	
	if (RX1IF == 0) {
		return 0;
	}
	UartRecv[2].UartRecv_start = 0;
	UartRecv[2].Recv_finish = 0;
	rlen = 0;
	do {
		RX1IF = 0;
		tmpr = uart[2].RxB;
		if (tmpr == 0xaa)
		{
			UartRecv[2].Recv_start = 1;
		}else{
			if (UartRecv[2].Recv_start){
				UartRecv[2].Recv_Buf[rlen++] = tmpr;
			}
		}
		if (rlen >= UART_BUFFER_LENGTH)
			return 0;
		if (rlen >= UartRecv[2].Recv_Buf[0]){//Recv_Buf[0] is data len
			break;
		}
		curt = Timetick();
		while(RX1IF == 0){
			if (IsTimeOut(curt+UART_TIMEOUT)){
				return 0;
			}
		}
	}while(1);
	UartRecv[2].Recv_Len = rlen;
	UartRecv[2].Recv_finish = 1;
}

void SetBaud(uchar uartIdx, int16u  baud)
{
	uchar baudreg = Calc_baud(baud);
	uart[uartIdx].Buad = baudreg;
	uart[uartIdx].BRGH = 0;//low bitrate mode
}

void serial_data_init(uchar idx)
{
    MMemSet((int8u *)&UartRecv[idx],0,sizeof(UartRecv_Buf));
   
}
