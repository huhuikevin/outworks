/********************
uart.h
********************/
#ifndef __UART__H_
#define __UART__H_


void UartInit(uint8_t uartIdx, uint16_t baudrate);
void uart_driver_process(void);

#ifdef CONFIG_LINKLAY_UART
uint8_t uart_rx_bytes(uint8_t *pdata);
uint8_t uart_tx_bytes(uint8_t *pdata, uint8_t len);
#endif

#ifdef CONFIG_CONSOLE
uint8_t console_uart_rx_bytes(uint8_t *pdata);
uint8_t console_uart_tx_bytes(uint8_t *pdata, uint8_t len);
#endif
void serial_data_init(uint8_t idx);
void SetBaud(uint8_t uartIdx, uint16_t  baud);

uint8_t Uart0_Tx(uint8_t *pdata, uint8_t len);
uint8_t Uart1_Tx(uint8_t *pdata, uint8_t len);
uint8_t Uart2_Tx(uint8_t *pdata, uint8_t len);

uint8_t Uart0_Rx();
uint8_t Uart1_Rx();
uint8_t Uart2_Rx();
uint8_t Uart_Rx();
uint8_t uart_rx_byte();


#endif /* __UART__H_ */


