/********************
uart.h
********************/
#ifndef __UART__H_
#define __UART__H_


void UartInit(uchar uartIdx, int16u baudrate);
#ifdef CONFIG_LINKLAY_UART
uchar uart_rx_bytes(uchar *pdata);
uchar uart_tx_bytes(uchar *pdata, uchar len);
#endif

#ifdef CONFIG_CONSOLE
uchar console_uart_rx_bytes(uchar *pdata);
uchar console_uart_tx_bytes(uchar *pdata, uchar len);
#endif
void serial_data_init(uchar idx);
void SetBaud(uchar uartIdx, int16u  baud);
uchar Uart0_Tx(uchar *pdata, uchar len);
uchar Uart1_Tx(uchar *pdata, uchar len);
uchar Uart2_Tx(uchar *pdata, uchar len);

uchar Uart0_Rx();
uchar Uart1_Rx();
uchar Uart2_Rx();
uchar Uart_Rx();








#endif /* __UART__H_ */


