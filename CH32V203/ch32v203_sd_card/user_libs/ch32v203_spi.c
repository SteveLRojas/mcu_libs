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

void spi_bulk_transfer_8b(SPI_TypeDef* spi, uint8_t* source, uint8_t* dest, uint16_t len)
{
	uint16_t idx = 0;

	while(len--)
	{
		spi->DATAR = source[idx];
		while(!(spi->STATR & SPI_STATR_RXNE));
		dest[idx] = (uint8_t)(spi->DATAR);
		idx += 1;
	}
}

void spi_bulk_transfer_16b(SPI_TypeDef* spi, uint16_t* source, uint16_t* dest, uint16_t len)
{
	uint16_t idx = 0;

	while(len--)
	{
		spi->DATAR = source[idx];
		while(!(spi->STATR & SPI_STATR_RXNE));
		dest[idx] = spi->DATAR;
		idx += 1;
	}
}

void spi_bulk_in_8b(SPI_TypeDef* spi, uint8_t* dest, uint8_t out_val, uint16_t len)
{
	uint16_t idx = 0;

	while(len--)
	{
		spi->DATAR = out_val;
		while(!(spi->STATR & SPI_STATR_RXNE));
		dest[idx] = (uint8_t)(spi->DATAR);
		idx += 1;
	}
}

void spi_bulk_in_16b(SPI_TypeDef* spi, uint16_t* dest, uint16_t out_val, uint16_t len)
{
	uint16_t idx = 0;

	while(len--)
	{
		spi->DATAR = out_val;
		while(!(spi->STATR & SPI_STATR_RXNE));
		dest[idx] = spi->DATAR;
		idx += 1;
	}
}

uint8_t spi_bulk_out_8b(SPI_TypeDef* spi, uint8_t* source, uint16_t len)
{
	uint16_t idx = 0;

	while(len--)
	{
		spi->DATAR = source[idx];
		while(!(spi->STATR & SPI_STATR_TXE));
		idx += 1;
	}
	while(spi->STATR & SPI_STATR_BSY);
	return (uint8_t)(spi->DATAR);
}

uint16_t spi_bulk_out_16b(SPI_TypeDef* spi, uint16_t* source, uint16_t len)
{
	uint16_t idx = 0;

	while(len--)
	{
		spi->DATAR = source[idx];
		while(!(spi->STATR & SPI_STATR_TXE));
		idx += 1;
	}
	while(spi->STATR & SPI_STATR_BSY);
	return spi->DATAR;
}
