#ifndef _CH547_SPI_H_
#define _CH547_SPI_H_

#define SPI_MODE_0 0x00
#define SPI_MODE_3 0x08

void spi_init(UINT8 clk_div, UINT8 spi_mode);
void spi_disable();
UINT8 spi_transfer(UINT8 val);
void spi_bulk_transfer(UINT8* source, UINT8* dest, UINT16 len);
void spi_bulk_in(UINT8* dest, UINT8 out_val, UINT16 len);
UINT8 spi_bulk_out(UINT8* source, UINT16 len);

#endif
