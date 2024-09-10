#define NO_XSFR_DEFINE
#include "CH559.H"
#include "CH559_LED.h"

UINT8 xdata led_buf[LED_BUF_SIZE] _at_ 0x0180;	//must be placed at an even address
volatile UINT8 led_transfering;

//HINT: INT0 interrupt becomes the LED interrupt when bLED_OUT_EN is set
void led_isr(void) interrupt INT_NO_INT0
{
	LED_CTRL &= ~bLED_DMA_EN;
	led_transfering = 0;
	LED_STAT = bLED_IF_DMA_END | bLED_IF_FIFO_REQ;
}

//The CH559 datsheet does not say how to set the clock divider... Nor does it say which values are valid. Here are my findings:
//Setting clk_div to 0 results in a clock frequency of sys_clk / 256, wrong data is output on the LED lines
//Setting clk_div to 1 causes the module to stop working
//Setting clk_div > 1 the resulting clock frequency is clk_sys / clk_div
//Setting clk_div to 255 results in the expected clock frequency, but wrong data output
//HINT: Due to the pre-processing of the data 4 clock cycles are needed to shift out each symbol. Set the clock speed to 4 times the desired bit rate.
void led_init(UINT8 clk_div, UINT8 led_mode)
{
	LED_CTRL = led_mode | bLED_OUT_EN;
	LED_CK_SE = clk_div;
	LED_DMA_AH = (UINT8)((UINT16)led_buf >> 8);
	LED_DMA_AL = (UINT8)led_buf;
	led_transfering = 0;
	EX0 = 1;
}

void led_update(UINT8 num_leds)
{
	UINT8 num_bytes;
	num_bytes = (num_leds << 1) + num_leds;
	num_bytes = num_bytes << 2;
	
	LED_DMA_AH = (UINT8)((UINT16)led_buf >> 8);
	LED_DMA_AL = (UINT8)led_buf;
	led_transfering = 1;
	LED_DMA_CN = num_bytes;
	LED_CTRL |= bLED_DMA_EN;
}

void led_set_color(UINT8 led, UINT8 red, UINT8 green, UINT8 blue)
{
	UINT8 idx;
	UINT8 count;
	UINT8 symbol_pair;	
	
	idx	= (led << 1 ) + led;
	idx = idx << 2;
	
	//Process green
	for(count = 0; count < 4; ++count)
	{
		symbol_pair = (green & 0x80) ? 0xE0 : 0x80;
		symbol_pair |= (green & 0x40) ? 0x0E : 0x08;
		
		led_buf[idx] = symbol_pair;
		++idx;
		green = green << 2;
	}
	
	//Process red
	for(count = 0; count < 4; ++count)
	{
		symbol_pair = (red & 0x80) ? 0xE0 : 0x80;
		symbol_pair |= (red & 0x40) ? 0x0E : 0x08;
		
		led_buf[idx] = symbol_pair;
		++idx;
		red = red << 2;
	}
	
	//Process blue
	for(count = 0; count < 4; ++count)
	{
		symbol_pair = (blue & 0x80) ? 0xE0 : 0x80;
		symbol_pair |= (blue & 0x40) ? 0x0E : 0x08;
		
		led_buf[idx] = symbol_pair;
		++idx;
		blue = blue << 2;
	}
}

void led_set_color_single(UINT8 led, UINT8 channel, UINT8 value)
{
	UINT8 idx;
	UINT8 count;
	UINT8 symbol_pair;
	
	idx	= (led << 1 ) + led;
	idx = (idx << 2) + channel;
	
	for(count = 0; count < 4; ++count)
	{
		symbol_pair = (value & 0x80) ? 0xE0 : 0x80;
		symbol_pair |= (value & 0x40) ? 0x0E : 0x08;
		
		led_buf[idx] = symbol_pair;
		++idx;
		value = value << 2;
	}
}

UINT8 led_get_color_single(UINT8 led, UINT8 channel)
{
	UINT8 color = 0;
	UINT8 idx;
	UINT8 count;
	UINT8 symbol_pair;
	
	idx	= (led << 1 ) + led;
	idx = (idx << 2) + channel;
	
	for(count = 0; count < 4; ++count)
	{
		symbol_pair = led_buf[idx];
		++idx;
		color = color << 2;
		
		if((symbol_pair & 0xF0) == 0xE0)
			color |= 0x02;
		if((symbol_pair & 0x0F) == 0x0E)
			color |= 0x01;
	}
	
	return color;
}
