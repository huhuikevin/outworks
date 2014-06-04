/*******************************
File:	UART
Description: Define all the function which related to the serial communicaitons.

*******************************/
#include "P111.h"
#include "type.h"
#include "config.h"
#include "system.h"
#include "uart.h"
#include "timer16n.h"
#include "tool.h"


#define UART_BUFFER_LENGTH 	MSG_MAX_LEN

typedef struct
{
	uint8_t Recv_start:1;					//串口正在接收标识
	uint8_t Recv_finish:1;	                    //串口接收完成
    uint8_t inited;                                     //串口接收完成标志
	uint8_t Recv_Len;					//串口接收长度	
	uint8_t Recv_Buf[UART_BUFFER_LENGTH];		//串口接收buffer
}UARTRECV_BUFFER;



typedef struct {
	uint8_t RxB;
	uint8_t RxR8:1;
	uint8_t FERR:1;
	uint8_t OERR:1;
	uint8_t RRES:3;
	uint8_t RxM:1;
	uint8_t RxEN:1;
	uint8_t TxB;
	uint8_t TxR8:1;
	uint8_t TRMT:1;
	uint8_t TRES:3;
	uint8_t BRGH:1;
	uint8_t TxM:1;
	uint8_t TxEN:1;
	uint8_t Buad;
}uart_register;


#define UART_CHAR_SEND(i, b)\
do{\
	uart[i].TxEN = 1;\
	uart[i].TxB = b;\
	while(!uart[i].TRMT);\
}while(0);

#define uart_rx_error(i, b)\
do{\
	uint8_t c;\
	b = uart[i].FERR;\
	b <<= 1; \
	b |= uart[i].OERR;\
	if(b != 0){ \
		c = uart[i].RxB; \
		uart[i].RxEN = 0;\
	}\
}while(0);


#define UART_TIMEOUT 1//10MS

section64 volatile uart_register uart[3]@0xFFE0;
//uart_register uart[3];
section10 UARTRECV_BUFFER  UartRecv[3]@0X500;


uint8_t Calc_baud(int16u baud)
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
void UartInit(uint8_t uartIdx, uint16_t baudrate)						//Rcv function of Uart reset
{
  
	uart[uartIdx].TxM = 0;// 8bit data tx
	uart[uartIdx].RxM = 0;// 8bit data rx

	SetBaud(uartIdx, baudrate);
    uart[uartIdx].RxEN= 1; 
	serial_data_init(uartIdx);
	UartRecv[uartIdx].inited = 1;
}


#ifdef CONFIG_LINKLAY_UART
uint8_t uart_rx_bytes(uint8_t *pdata)
{
	uint8_t rlen = 0;
	if (0 == uart[CONFIG_LINKLAY_UART].RxEN)
	{
		uart[CONFIG_LINKLAY_UART].RxEN = 1;
	}
	if (UartRecv[CONFIG_LINKLAY_UART].Recv_finish)
	{
		rlen = UartRecv[CONFIG_LINKLAY_UART].Recv_Len-1;
		MMemcpy(pdata,&UartRecv[CONFIG_LINKLAY_UART].Recv_Buf[1],rlen);
		UartRecv[CONFIG_LINKLAY_UART].Recv_finish = 0;
	}
	return rlen;
}

uint8_t uart_tx_bytes(uint8_t *pdata, uint8_t len)
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
uint8_t console_uart_rx_bytes(uint8_t *pdata)
{
	uint8_t rlen = 0;
	if (0 == uart[CONFIG_CONSOLE_UART].RxEN)
	{
		uart[CONFIG_CONSOLE_UART].RxEN = 1;
	}	
	if (UartRecv[CONFIG_CONSOLE_UART].Recv_finish)
	{
		rlen = UartRecv[CONFIG_CONSOLE_UART].Recv_Len-1;
		MMemcpy(pdata,&UartRecv[CONFIG_CONSOLE_UART].Recv_Buf[1],rlen);
		UartRecv[CONFIG_CONSOLE_UART].Recv_finish = 0;
	}
	return rlen;
}

uint8_t console_uart_tx_bytes(uint8_t *pdata, uint8_t len)
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


uint8_t Uart0_Tx(uint8_t *pdata, uint8_t len)
{
	uint8_t send = 0;
	UART_CHAR_SEND(0, 0xaa);
	UART_CHAR_SEND(0, len+2);

	do {
		UART_CHAR_SEND(0, pdata[send++]);
	}while(send < len);

	return len;

}

uint8_t Uart1_Tx(uint8_t *pdata, uint8_t len)
{

	uint8_t send = 0;
	UART_CHAR_SEND(1, 0xaa);
	UART_CHAR_SEND(1, len+2);

	do {
		UART_CHAR_SEND(1, pdata[send++]);
	}while(send < len);

	return len;

}

uint8_t Uart2_Tx(uint8_t *pdata, uint8_t len)
{
	uint8_t send = 0;
	UART_CHAR_SEND(2, 0xaa);
	UART_CHAR_SEND(2, len+2);

	do {
		UART_CHAR_SEND(2, pdata[send++]);
	}while(send < len);

	return len;

}

uint8_t Uart_Tx(uint8_t uartIdx, uint8_t *pdata, uint8_t len)
{
	if (uartIdx == 0)
		return Uart0_Tx(pdata,len);
	else if (uartIdx == 1)
		return Uart1_Tx(pdata,len);
	else
		return Uart2_Tx(pdata,len);
}


uint8_t Uart_Rx()
{
	if (UartRecv[0].inited)
		Uart0_Rx();
	if (UartRecv[1].inited)
		Uart1_Rx();
	if (UartRecv[2].inited)
		Uart2_Rx();
}


uint8_t Uart0_Rx()
{
	uint8_t tmpr,rlen;
	int16u curt;
	
	if (RX1IF == 0) {
		return 0;
	}
	UartRecv[0].Recv_start = 0;
	UartRecv[0].Recv_finish = 0;
	rlen = 0;
	do {
		RX1IF = 0;
		uart_rx_error(0, tmpr);
		if (tmpr){
			UartRecv[0].Recv_start = 0;
			UartRecv[0].Recv_Len = 0;
			UartRecv[0].Recv_finish = 0;
			return 0;
		}
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
		curt = _sys_tick;
		while(RX1IF == 0){
			if ((curt+UART_TIMEOUT) <= _sys_tick){
				return 0;
			}
		}
	}while(1);
	UartRecv[0].Recv_Len = rlen;
	UartRecv[0].Recv_finish = 1;
}


uint8_t Uart1_Rx()
{
	uint8_t tmpr,rlen;
	int16u curt;
	
	if (RX2IF == 0) {
		return 0;
	}
	UartRecv[1].Recv_start = 0;
	UartRecv[1].Recv_finish = 0;
	rlen = 0;
	do {
		RX2IF = 0;
		uart_rx_error(1, tmpr);
		if (tmpr){
			UartRecv[1].Recv_start = 0;
			UartRecv[1].Recv_Len = 0;
			UartRecv[1].Recv_finish = 0;
			return 0;
		}		
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
		curt = _sys_tick;
		while(RX2IF == 0){
			if ((curt+UART_TIMEOUT) <= _sys_tick){
				return 0;
			}
		}

	}while(1);
	UartRecv[1].Recv_Len = rlen;
	UartRecv[1].Recv_finish = 1;
}

uint8_t Uart2_Rx()
{
	uint8_t tmpr,rlen;
	int16u curt;
	
	if (RX3IF == 0) {
		return 0;
	}
	UartRecv[2].Recv_start = 0;
	UartRecv[2].Recv_finish = 0;
	rlen = 0;
	do {
		RX3IF = 0;
		uart_rx_error(2, tmpr);
		if (tmpr){
			UartRecv[2].Recv_start = 0;
			UartRecv[2].Recv_Len = 0;
			UartRecv[2].Recv_finish = 0;
			return 0;
		}		
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
		curt = _sys_tick;
		while(RX3IF == 0){
			if ((curt+UART_TIMEOUT) <= _sys_tick){
				return 0;
			}
		}

	}while(1);
	UartRecv[2].Recv_Len = rlen;
	UartRecv[2].Recv_finish = 1;
}

void SetBaud(uint8_t uartIdx, uint16_t  baud)
{
    uint8_t baudreg = Calc_baud(baud);
	uart[uartIdx].Buad = baudreg;
	uart[uartIdx].BRGH = 0;//low bitrate mode	
}

void serial_data_init(uint8_t idx)
{
    MMemSet((int8u *)&UartRecv[idx],0,sizeof(UARTRECV_BUFFER));
}


uint8_t uart_rx_byte()
{
    char tmpr;
#if CONFIG_LINKLAY_UART==0    
    while(RX1IF == 0);
    RX1IF = 0;
	uart_rx_error(0, tmpr);
	if (tmpr) return 0;
    tmpr = uart[0].RxB;
#endif

#if CONFIG_LINKLAY_UART==1    
    while(RX2IF == 0);
    RX2IF = 0;
	uart_rx_error(1, tmpr);
	if (tmpr) return 0;	
    tmpr = uart[1].RxB;
#endif

#if CONFIG_LINKLAY_UART==2    
    while(RX3IF == 0);
    RX3IF = 0;
	uart_rx_error(2, tmpr);
	if (tmpr) return 0;	
    tmpr = uart[2].RxB;
#endif    
    return tmpr;
}
