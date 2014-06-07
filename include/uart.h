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
uint8_t uart_rx(uint8_t idx);
uint8_t uart_tx(uint8_t idx, uint8_t *pdata, uint8_t len);
uint8_t _uart_rx_bytes(uint8_t idx, uint8_t *pdata);
uint8_t uart_rx_one_byte();
void uart_tx_one_byte(uint8_t b);
uint8_t uart_send_frame(uint8_t *pdata, uint8_t len);
int8_t uart_rx_frame(uint8_t *pdata);
#endif /* __UART__H_ */


