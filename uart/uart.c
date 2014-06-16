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
	uint8_t rx_start:1;					//串口正在接收标识
	uint8_t rx_finish:1;	                    //串口接收完成
    uint8_t inited;                                     //串口接收完成标志
	uint8_t rx_Len;					//串口接收长度	
	uint8_t rx_buf[UART_BUFFER_LENGTH];		//串口接收buffer
}uart_rx_buffer;



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


#define UART_TIMEOUT 10//100MS

section64 volatile uart_register uart[3]@0xFFE0;
//uart_register uart[3];
section10 uart_rx_buffer  uart_recv[3]@0X500;


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
void uart_init(uint8_t uartIdx, uint16_t baudrate, uint8_t tx8, uint8_t rx8)						//Rcv function of Uart reset
{
  
	uart[uartIdx].TxM = tx8;// 8bit data tx
	uart[uartIdx].RxM = rx8;// 8bit data rx

	SetBaud(uartIdx, baudrate);
    uart[uartIdx].RxEN= 1; 
	serial_data_init(uartIdx);
	uart_recv[uartIdx].inited = 1;
}


#ifdef CONFIG_LINKLAY_UART
uint8_t uart_rx_bytes(uint8_t *pdata)
{
	return _uart_rx_bytes(CONFIG_LINKLAY_UART, pdata);
}

uint8_t uart_tx_bytes(uint8_t *pdata, uint8_t len)
{
	return uart_tx(CONFIG_LINKLAY_UART, pdata, len);
}
#endif

uint8_t _uart_rx_bytes(uint8_t idx, uint8_t *pdata)
{
	uint8_t rlen = 0;
	if (0 == uart[idx].RxEN)
	{
		uart[idx].RxEN = 1;
	}	
	if (uart_recv[idx].rx_finish)
	{
		rlen = uart_recv[idx].rx_Len-1;
		MMemcpy(pdata,&uart_recv[idx].rx_buf[1],rlen);
		uart_recv[idx].rx_finish = 0;
	}
	return rlen;
}

void uart_driver_process(void)
{
	if (uart_recv[0].inited)
		uart_rx(0);
	if (uart_recv[1].inited)
		uart_rx(1);
	if (uart_recv[2].inited)
		uart_rx(2);
}


uint8_t uart_tx(uint8_t idx, uint8_t *pdata, uint8_t len)
{
	uint8_t send = 0;

	do {
		UART_CHAR_SEND(idx, pdata[send++]);
	}while(send < len);

	return len;

}


uint8_t uart_rx(uint8_t idx)
{
	uint8_t tmpr,rlen;
	int16u curt;
	uint8_t mask = 1 << (idx*2 + 1);
	
	if ((INTF2  & mask) == 0) {
		return 0;
	}
	uart_recv[idx].rx_start = 0;
	uart_recv[idx].rx_finish = 0;
	rlen = 0;
	do {
		INTF2 &= ~mask;
		uart_rx_error(idx, tmpr);
		if (tmpr){
			uart_recv[idx].rx_start = 0;
			return 0;
		}
		tmpr = uart[idx].RxB;
		if ((uart_recv[idx].rx_start != 1) && (tmpr == 0xaa))
		{
			uart_recv[idx].rx_start = 1;
		}else{
			if (uart_recv[idx].rx_start){
				uart_recv[idx].rx_buf[rlen++] = tmpr;
			}
		
			if (rlen >= UART_BUFFER_LENGTH){
				uart_recv[idx].rx_start = 0;
				return 0;
			}
			if (rlen >= uart_recv[idx].rx_buf[0]){//rx_buf[0] is data len
				break;
			}
		}
		curt = _sys_tick;
		while((INTF2  & mask) == 0){
			if ((curt+UART_TIMEOUT) <= _sys_tick){
				return 0;
			}
		}
	}while(1);
	uart_recv[idx].rx_Len = rlen;
	uart_recv[idx].rx_finish = 1;
}



void SetBaud(uint8_t uartIdx, uint16_t  baud)
{
    uint8_t baudreg = Calc_baud(baud);
	uart[uartIdx].Buad = baudreg;
	uart[uartIdx].BRGH = 0;//low bitrate mode	
}

void serial_data_init(uint8_t idx)
{
    MMemSet((int8u *)&uart_recv[idx],0,sizeof(uart_rx_buffer));
}

uint8_t console_rx_one_byte()
{
    char tmpr = 0;
#ifdef CONFIG_CONSOLE	
	uint8_t mask = 1 << (CONFIG_CONSOLE_UART*2 + 1);
	if ( 0 == uart[CONFIG_CONSOLE_UART].RxEN)
		uart[CONFIG_CONSOLE_UART].RxEN = 1;
	while ((INTF2  & mask) == 0) ;
       INTF2 &= ~mask;
       uart_rx_error(CONFIG_CONSOLE_UART, tmpr);
       tmpr = uart[CONFIG_CONSOLE_UART].RxB;
#endif	   
       return tmpr;
}

void console_tx_one_byte(uint8_t b)
{
#ifdef CONFIG_CONSOLE
	UART_CHAR_SEND(CONFIG_CONSOLE_UART, b);
#endif

}


uint16_t uart_crc(uint8_t data, uint8_t regval)
{ 
    uint8_t i;
 
    for (i = 0; i < 8; i++) 
    { 
        if (((regval & 0x8000) >> 8) ^ (data & 0x80) ) 
            regval = (regval << 1) ^ 0x8005; 
        else 
            regval = (regval << 1); 
        
        data <<= 1; 
    } 
    
    return regval; 
}

uint8_t uart_send_frame(uint8_t *pdata, uint8_t len)
{
#ifdef CONFIG_LINKLAY_UART

	uint16_t crc;
	uint8_t i;

	crc = uart_crc(pdata[0], 0xFFFF);
	for (i = 1; i < len; i++){
		crc = uart_crc(pdata[i], crc);
	}
	UART_CHAR_SEND(CONFIG_LINKLAY_UART, 0xaa);
	UART_CHAR_SEND(CONFIG_LINKLAY_UART, len+4);// 0xaa len crc[0] crc[1]
	UART_CHAR_SEND(CONFIG_LINKLAY_UART, crc>>8);
	UART_CHAR_SEND(CONFIG_LINKLAY_UART, crc%256);	

	uart_tx_bytes(pdata, len);

	return len;
#else
	return 0;
#endif
}

int8_t uart_rx_frame(uint8_t *pdata)
{
#ifdef CONFIG_LINKLAY_UART
	uint8_t i, rlen = 0;
	uint8_t *pbuf;
	uint16_t crc;
	if (0 == uart[CONFIG_LINKLAY_UART].RxEN)
	{
		uart[CONFIG_LINKLAY_UART].RxEN = 1;
	}	
	if (uart_recv[CONFIG_LINKLAY_UART].rx_finish == 0)
	{
		return 0;
	}
	rlen = uart_recv[CONFIG_LINKLAY_UART].rx_Len-3;
	pbuf = &uart_recv[CONFIG_LINKLAY_UART].rx_buf[0];
	crc = uart_crc(pbuf[3], 0xffff);
	for (i = 1; i< rlen; i++){
		crc = uart_crc(pbuf[i+3], crc);
	}
	uart_recv[CONFIG_LINKLAY_UART].rx_finish = 0;
	if (crc != pbuf[1]*256 + pbuf[2]){
		return -1;
	}
	
	MMemcpy(pdata,&pbuf[3],rlen);
	return rlen;
#else
	return 0;
#endif
}

