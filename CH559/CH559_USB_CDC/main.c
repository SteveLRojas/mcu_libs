#include "CH559.H"
#include "DEBUG.H"
#include "CH559_GPIO.h"
#include "CH559_TIMER.h"
#include "CH559_UART.h"
#include "CH559_USB.h"
#include "usb_cdc.h"

char code test_string[] = "Unicorn\n";

//Pins:
// RXD0 = P02
// TXD0 = P03
// USB_DM = P50
// USB_DP = P51
// LED1 = P14
// LED2 = P15

void byte_to_hex(UINT8 value, char* buff)
{
	const char table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	buff[0] = table[(value >> 4) & 0x0f];
	buff[1] = table[(value) & 0x0f];
	buff[2] = '\0';
}

int main()
{
	char last_keep_str[4];
	
	CfgFsys();	//CH559 clock selection configuration
	
	// Configure UART pins
	gpio_set_port_mode(GPIO_PORT_MODE_OC, GPIO_PORT_0);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_0);
	gpio_set_pin_mode(GPIO_MODE_OC_PU, GPIO_PORT_0, GPIO_PIN_2);
	gpio_set_pin_mode(GPIO_MODE_OC_PU_PULSE, GPIO_PORT_0, GPIO_PIN_3);
	
	// Configure LED pins
	gpio_set_port_mode(GPIO_PORT_MODE_PP, GPIO_PORT_1);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_1);
	gpio_set_pin_mode(GPIO_MODE_OUTPUT_PP, GPIO_PORT_1, GPIO_PIN_4 | GPIO_PIN_5);
	
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
	
	byte_to_hex(RESET_KEEP, last_keep_str);
	last_keep_str[2] = '\n';
	last_keep_str[3] = '\0';
	uart_write_string(UART_0, last_keep_str);
	
	cdc_init();

	while(TRUE)
	{	
		if(cdc_bytes_available())
		{
			uart_write_byte(UART_0, cdc_read_byte());
			gpio_toggle_pin(GPIO_PORT_1, GPIO_PIN_4);
		}
		
		if(uart_bytes_available(UART_0))
		{
			cdc_write_byte(uart_read_byte(UART_0));
			gpio_toggle_pin(GPIO_PORT_1, GPIO_PIN_5);
		}
	}
}
