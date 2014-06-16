/********************
uart.h
********************/
#ifndef __UART__H_
#define __UART__H_


void uart_init(uint8_t uartIdx, uint16_t baudrate, uint8_t tx8, uint8_t rx8);
void uart_driver_process(void);

uint8_t uart_rx_bytes(uint8_t *pdata);
uint8_t uart_tx_bytes(uint8_t *pdata, uint8_t len);

uint8_t console_rx_one_byte();
void console_tx_one_byte(uint8_t b);

void serial_data_init(uint8_t idx);
void SetBaud(uint8_t uartIdx, uint16_t  baud);
uint8_t uart_rx(uint8_t idx);
uint8_t uart_tx(uint8_t idx, uint8_t *pdata, uint8_t len);
uint8_t _uart_rx_bytes(uint8_t idx, uint8_t *pdata);
uint8_t uart_send_frame(uint8_t *pdata, uint8_t len);
int8_t uart_rx_frame(uint8_t *pdata);
#endif /* __UART__H_ */


