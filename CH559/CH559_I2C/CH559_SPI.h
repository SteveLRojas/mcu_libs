#ifndef _CH559_SPI_H_
#define _CH559_SPI_H_

#define SPI_0 0
#define SPI_1 1

#define SPI_MODE_0 0x00
#define SPI_MODE_3 0x08

void spi_init(UINT8 spi, UINT8 clk_div, UINT8 spi_mode);
void spi_disable(UINT8 spi);
UINT8 spi_transfer(UINT8 spi, UINT8 val);

#endif
