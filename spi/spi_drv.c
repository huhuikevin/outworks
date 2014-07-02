/* gpio 模拟spi 驱动*/
#include <hic.h>
#include "type.h"
#include "config.h"
#include "system.h"
#include "tool.h"

#ifdef SPI_GPIO

#define spi_start() CONFIG_SPI_CSN = 0 // 0 CS ENBALE
#define spi_stop() CONFIG_SPI_CSN = 1

#define spi_clk_h() CONFIG_SPI_SCK = 1
#define spi_clk_l() CONFIG_SPI_SCK = 0

#define miso(b)  b = CONFIG_SPI_MISO
#define mosi(b)  CONFIG_SPI_MOSI = b

/*接收方使用下降沿接收，所以这里要给一个下降沿*/
#define spi_tx_bit(b)\
do{\
	mosi(b);\
	spi_clk_h();\
	Delay1us();\
	spi_clk_l();\
	Delay1us();\
}while(0);

#define spi_rx_bit(b)\
do{\
	spi_clk_h();\
	Delay1us();\
	spi_clk_l();\
	Delay1us();\
	miso(b);\
}while(0);

void Delay1us()// 10 T
{
	NOP();NOP();NOP();NOP();NOP();
}

void spi_write_byte(uint8_t c)
{
	uint8_t i;
	for (i = 0; i < 8; i++){
		if (c&0x80) {
			spi_tx_bit(1);
		}
		else {
			spi_tx_bit(0);
		}	
		c <<= 1;
	}
}

uint8_t spi_read_byte()
{
	uint8_t i;
	uint8_t c = 0;
	uint8_t spi_bit;
	for (i = 0; i< 8; i++){
		spi_rx_bit(spi_bit);
		c <<= 1;
		if (spi_bit)
			c |= 1;
	}
	return c;
}


void spi_tx_word(uint8_t addr, uint16_t w)
{
	uint8_t i;
	disable_irq();
	spi_start();
	spi_write_byte((0x80|addr));//send addr
	for (i = 0; i< 16; i++){
		if (w & 0x8000) {
			spi_tx_bit(1);
		}
		else {
			spi_tx_bit(0);
		}
		w <<= 1;
	}
	spi_stop();
	enable_irq();
}

uint16_t spi_rx_word(uint8_t addr)
{
	uint8_t i;
	uint16_t w = 0;
	uint8_t spi_bit;
	disable_irq();
	spi_start();
	spi_write_byte(addr);// send addr
	for (i = 0; i< 16; i++){
		spi_rx_bit(spi_bit);
		w <<= 1;
		if (spi_bit)
			w |= 1;
	}
	spi_stop();
	enable_irq();
	return w;
}


void spi_write(uint8_t addr , uint8_t *data, uint8_t len)
{
	uint8_t i;
	disable_irq();
	spi_start();
	spi_write_byte((0x80|addr));

	for (i = 0; i < len; i++){
		spi_write_byte(*(data + i));
	}
	spi_stop();
	enable_irq();
}

void spi_read(uint8_t addr , uint8_t *data, uint8_t len)
{
	uint8_t i;
	disable_irq();
	spi_start();
	spi_write_byte(addr);

	for (i = 0; i < len; i++){
		*((data + i)) = spi_read_byte();
	}
	spi_stop();
	enable_irq();
}

void spi_init(void)
{
	spi_stop();
	spi_clk_l();
}

#endif/*#ifdef SPI_GPIO */

