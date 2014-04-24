#ifndef SPI_DRV__H_
#define SPI_DRV__H_

void spi_init();
void spi_read(uchar addr , uchar *data, uchar len);
void spi_write(uchar addr , uchar *data, uchar len);

#endif /*SPI_DRV__H_*/


