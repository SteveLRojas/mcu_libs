#include "CH559.H"
#include "CH559_RCC.h"
#include "CH559_GPIO.h"
#include "CH559_TIMER.h"
#include "CH559_UART.h"
#include "CH559_LED.h"
#include "pseudo_random.h"

#define BAUD_RATE 125000ul
#define USE_EXT_CLK 0

char code test_string[] = "Unicorn\n";

char line_buf[10];

//Pins:
// RXD0 = P02
// TXD0 = P03
// DBGLED1 = P14
// DBGLED2 = P15
// LED0 = P32
// LED1 = P33
// LEDC = P34
// LED2 = P40
// LED3 = P44
// USB_DM = P50
// USB_DP = P51


char code hex_table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
void byte_to_hex(UINT8 value, char* buff)
{
	buff[0] = hex_table[(value >> 4) & 0x0f];
	buff[1] = hex_table[(value) & 0x0f];
	buff[2] = '\0';
}

void print_bytes_as_hex(UINT8* source, UINT8 num_bytes)
{
	UINT8 idx;

	for(idx = 0; idx < num_bytes; ++idx)
	{
		line_buf[0] = hex_table[idx >> 4];
		line_buf[1] = hex_table[idx & 0x0F];
		line_buf[2] = ':';
		line_buf[3] = ' ';
		line_buf[4] = hex_table[source[idx] >> 4];
		line_buf[5] = hex_table[source[idx] & 0x0F];
		line_buf[6] = '\n';
		line_buf[7] = '\0';
		(void)uart_write_string(UART_0, line_buf);
	}
}

int main()
{
	UINT8 reset_type;
	UINT8 temp;
	UINT8 idx;
	
	rcc_set_clk_freq(RCC_CLK_FREQ_48M);
	reset_type = rcc_get_rst_typ();
	
#if USE_EXT_CLK	
	rcc_set_clk_src(RCC_CLK_OSC_EN);
	rcc_delay_ms(30);
	rcc_set_clk_src(RCC_CLK_SRC_EXT);
#endif
	
	// Configure UART pins
	gpio_set_port_mode(GPIO_PORT_MODE_OC, GPIO_PORT_0);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_0);
	gpio_set_pin_mode(GPIO_MODE_OC_PU, GPIO_PORT_0, GPIO_PIN_2);
	gpio_set_pin_mode(GPIO_MODE_OC_PU_PULSE, GPIO_PORT_0, GPIO_PIN_3);
	
	// Configure debug LED pins
	gpio_set_port_mode(GPIO_PORT_MODE_PP, GPIO_PORT_1);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_1);
	gpio_set_pin_mode(GPIO_MODE_OUTPUT_PP, GPIO_PORT_1, GPIO_PIN_4 | GPIO_PIN_5);
	
	// Configure LED pins
	gpio_set_port_mode(GPIO_PORT_MODE_PP, GPIO_PORT_3);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_3);
	gpio_set_pin_mode(GPIO_MODE_OUTPUT_PP, GPIO_PORT_3, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4);
	gpio_set_port_mode(GPIO_PORT_MODE_PP, GPIO_PORT_4);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_4);
	gpio_set_pin_mode(GPIO_MODE_OUTPUT_PP, GPIO_PORT_4, GPIO_PIN_0 | GPIO_PIN_4);

	led_init(15, LED_MODE_1_CHANNEL | LED_POL_NORMAL | LED_ORDER_LSB_FIRST);
	for(idx = 0; idx < LED_NUM_LEDS; ++idx)
	{
		led_set_color(idx, 0x20, 0x00, 0x00);
	}
	led_update();
	
	if((reset_type == RCC_RST_TYP_WDOG) || (reset_type == RCC_RST_TYP_SOFT))
	{
		rcc_delay_ms(500);
	}
	else
	{
		rcc_delay_ms(50);
	}
	
	uart0_init(TIMER_1, BAUD_RATE, UART_0_P02_P03);
	timer_init(TIMER_0, NULL);
	timer_set_period(TIMER_0, FREQ_SYS / 1000ul);	//period is 1ms
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	//Blink LED once
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_4);
	timer_long_delay(TIMER_0, 250);
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_4);
	timer_long_delay(TIMER_0, 250);
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_4);
	uart_write_string(UART_0, test_string);
	
	byte_to_hex(RESET_KEEP, line_buf);
	line_buf[2] = '\n';
	line_buf[3] = '\0';
	uart_write_string(UART_0, line_buf);
	
	pseudo_random_seed(0xDEADBEEF);
	for(idx = 0; idx < LED_NUM_LEDS; ++idx)
	{
		led_set_color(idx, 0x20, 0x00, 0x00);
	}
	led_update();
	timer_long_delay(TIMER_0, 250);
	idx = 0;
	
	while(TRUE)
	{
		if(uart_bytes_available(UART_0))
		{
			gpio_toggle_pin(GPIO_PORT_1, GPIO_PIN_5);
			temp = uart_read_byte(UART_0);
			led_init(temp, LED_MODE_1_CHANNEL | LED_POL_NORMAL | LED_ORDER_LSB_FIRST);
		}
		
		pseudo_random_generate(8);
		temp = pseudo_random_get_byte();
		temp = temp >> 2;	//Dividing by 4 because the lights are blindingly bright
		led_set_color_single(idx, LED_CHANNEL_RED, temp);
		pseudo_random_generate(8);
		temp = pseudo_random_get_byte();
		temp = temp >> 2;
		led_set_color_single(idx, LED_CHANNEL_GREEN, temp);
		pseudo_random_generate(8);
		temp = pseudo_random_get_byte();
		temp = temp >> 2;
		led_set_color_single(idx, LED_CHANNEL_BLUE, temp);
		
		led_update();
		while(led_transfering);
		++idx;
		if(idx == LED_NUM_LEDS)
			idx = 0;
		
		gpio_toggle_pin(GPIO_PORT_1, GPIO_PIN_4);
		timer_long_delay(TIMER_0, 100);
	}
}
