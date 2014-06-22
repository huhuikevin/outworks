#ifndef SPI_DRV__H_
#define SPI_DRV__H_

void spi_init();
void spi_tx_word(uint8_t addr, uint16_t w);
uint16_t spi_rx_word(uint8_t addr);
void spi_read(uint8_t addr , uint8_t *data, uint8_t len);
void spi_write(uint8_t addr , uint8_t *data, uint8_t len);

#endif /*SPI_DRV__H_*/


