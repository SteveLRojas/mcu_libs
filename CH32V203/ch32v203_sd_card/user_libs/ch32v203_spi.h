/*
 * ch32v203_spi.h
 *
 *  Created on: Jul 1, 2024
 *      Author: Steve
 */

#ifndef _CH32V203_SPI_H_
#define _CH32V203_SPI_H_

#define SPI_8_BIT 0x0000
#define SPI_16_BIT 0x0800

#define SPI_CLK_DIV_2 0x0000
#define SPI_CLK_DIV_4 0x0008
#define SPI_CLK_DIV_8 0x0010
#define SPI_CLK_DIV_16 0x0018
#define SPI_CLK_DIV_32 0x0020
#define SPI_CLK_DIV_64 0x0028
#define SPI_CLK_DIV_128 0x0030
#define SPI_CLK_DIV_256 0x0038

#define SPI_MODE_0 0x0000
#define SPI_MODE_1 0x0001
#define SPI_MODE_2 0x0002
#define SPI_MODE_3 0x0003

#define spi_disable(spi) ((spi)->CTLR1 &= ~SPI_CTLR1_SPE)
#define spi_enable(spi) ((spi)->CTLR1 |= SPI_CTLR1_SPE)

void spi_init(SPI_TypeDef* spi, uint16_t config);
uint16_t spi_transfer(SPI_TypeDef* spi, uint16_t val);

#endif /* _CH32V203_SPI_H_ */
