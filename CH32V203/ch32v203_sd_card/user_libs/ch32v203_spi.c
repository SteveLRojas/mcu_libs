#include "ch32v20x.h"
#include "ch32v203_spi.h"

void spi_init(SPI_TypeDef* spi, uint16_t config)
{
	spi->CTLR1 = 0;
	spi->CTLR1 = SPI_CTLR1_SSM | SPI_CTLR1_SSI | SPI_CTLR1_MSTR;
	spi->CTLR1 |= config;
	spi->CTLR2 = SPI_CTLR2_SSOE;
	spi->I2SCFGR = 0;
	spi->CTLR1 |= SPI_CTLR1_SPE;
}

uint16_t spi_transfer(SPI_TypeDef* spi, uint16_t val)
{
	spi->DATAR = val;
	while(!(spi->STATR & SPI_STATR_RXNE));
	return spi->DATAR;
}
