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

#define spi_rx_bit()  spi_bit=CONFIG_SPI_MISO
#define spi_tx_bit(b)  CONFIG_SPI_MOSI=b

sbit spi_bit;

/*接收方使用下降沿接收，所以这里要给一个下降沿*/
#define spi_tx_bit(b)\
do{\
	spi_clk_h();\
	spi_tx_bit(b);\
	Delay1us();\
	spi_clk_l();\
	Delay1us();\
}while(0);

/**/
#define spi_rx_bit()\
do{\
	spi_clk_l();\
	Delay1us();\
	spi_clk_h();\ 
	Delay1us();\
	spi_rx_bit();\
}while(0);

void Delay1us()// 10 T
{
	NOP();NOP();NOP();NOP();NOP();
}

void spi_tx_one_byte(uchar c)
{
	uchar i;
	for (i = 0; i< sizeof(uchar); i++){
		if (c&0x80)
			spi_tx_bit(1);
		else
			spi_tx_bit(0);
		c <<= 1;
	}
}

void spi_rx_one_byte(uchar *c)
{
	uchar i;
	for (i = 0; i< sizeof(uchar); i++){
		spi_rx_bit();
		c <<= 1;
		if (spi_bit)
			c |= 1;
	}
}


void spi_write(uchar addr , uchar *data, uchar len)
{
	uchar i;
	spi_start();
	spi_tx_one_byte((0x80|addr));

	for (i = 0; i < len; i++){
		spi_tx_one_byte(*(data + i));
	}
	spi_stop();
}

void spi_read(uchar addr , uchar *data, uchar len)
{
	uchar i;
	spi_start();
	spi_tx_one_byte((0x7e&addr));

	for (i = 0; i < len; i++){
		spi_rx_one_byte((data + i));
	}
	spi_stop();
}

void spi_init(void)
{
	spi_stop();
	spi_clk_l();
}

#endif/*#ifdef SPI_GPIO */

