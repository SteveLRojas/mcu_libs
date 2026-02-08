#include "CH552.H"
#include "CH552_SPI.h"

//The CH552 datsheet does not say how to set the clock divider... Nor does it say which values are valid. Here are my findings:
//Setting clk_div to 0 sets the spi clock freq to sys_clk / 256, wrong data is output om MOSI
//Setting clk_div to 1 makes the spi module produce no output at all, MOSI and SCK are driven low. reading SPI0_DATA gives the last value that was written to it.
//Setting clk_div to 2 gives an spi frequency of sys_clk / 2.
//Setting clk_div to 3 gives an spi frequency of sys_clk / 3*. The high time is 2 sys_clk cycles, the low time is 1 sys_clk cycle...
//Setting clk_div to 5 gives an spi frequency of sys_clk / 5*. The high time is 3 sys_clk cycles, the low time is 2 sys_clk cycles.
//I recommend to use only even values for the clock divider. spi_freq = sys_clk / clk_div.
void spi_init(UINT8 clk_div, UINT8 spi_mode)
{
	SPI0_CK_SE = clk_div;
	SPI0_SETUP = 0x00;	//bS0_MODE_SLV = 0, bS0_IE_BYTE = 0, bS0_BIT_ORDER = 0
	SPI0_CTRL = bS0_MOSI_OE | bS0_SCK_OE;
	SPI0_CTRL |= spi_mode;
}

void spi_disable()
{
	SPI0_CTRL = bS0_CLR_ALL;
}

UINT8 spi_transfer(UINT8 val)
{
	SPI0_DATA = val;
	while(!S0_FREE) {}
	return SPI0_DATA;
}
