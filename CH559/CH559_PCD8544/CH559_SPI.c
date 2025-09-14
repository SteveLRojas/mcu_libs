#define NO_XSFR_DEFINE
#include "CH559.H"
#include "CH559_SPI.h"

//The CH552 datsheet does not say how to set the clock divider... Nor does it say which values are valid. Here are my findings:
//Setting clk_div to 0 sets the spi clock freq to sys_clk / 256, wrong data is output om MOSI
//Setting clk_div to 1 makes the spi module produce no output at all, MOSI and SCK are driven low. reading SPI0_DATA gives the last value that was written to it.
//Setting clk_div to 2 gives an spi frequency of sys_clk / 2.
//Setting clk_div to 3 gives an spi frequency of sys_clk / 3*. The high time is 2 sys_clk cycles, the low time is 1 sys_clk cycle...
//Setting clk_div to 5 gives an spi frequency of sys_clk / 5*. The high time is 3 sys_clk cycles, the low time is 2 sys_clk cycles.
//I recommend to use only even values for the clock divider. spi_freq = sys_clk / clk_div.
// -> The above comment presumably also applies to SPI0 of the CH559...
void spi_init(UINT8 spi, UINT8 clk_div, UINT8 spi_mode)
{
	if(spi)
	{
		SPI1_CK_SE = clk_div;
		SPI1_CTRL = bS1_SCK_OE | spi_mode;
	}
	else
	{
		SPI0_CK_SE = clk_div;
		SPI0_SETUP = 0x00;	//bS0_MODE_SLV = 0, bS0_IE_BYTE = 0, bS0_BIT_ORDER = 0
		SPI0_CTRL = bS0_MOSI_OE | bS0_SCK_OE;
		SPI0_CTRL |= spi_mode;
	}
}

void spi_disable(UINT8 spi)
{
	if(spi)
	{
		SPI1_CTRL = bS1_CLR_ALL;
	}
	else
	{
		SPI0_CTRL = bS0_CLR_ALL;
	}
}

UINT8 spi_transfer(UINT8 spi, UINT8 val)
{
	if(spi)
	{
		SPI1_DATA = val;
		while(!(SPI1_STAT & bS1_FREE)) {}
		return SPI1_DATA;
	}
	else
	{
		SPI0_DATA = val;
		while(!S0_FREE) {}
		return SPI0_DATA;
	}
}
