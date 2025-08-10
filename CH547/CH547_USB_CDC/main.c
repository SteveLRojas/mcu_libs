#include "CH547.H"
#include "CH547_RCC.h"
#include "CH547_GPIO.h"
#include "CH547_UART.h"
#include "CH547_TIMER.h"
#include "CH547_USB_CDC.h"

#define  BAUD_RATE  125000ul

char code unicorn_string[] = "Unicorn\n";
char code dragon_string[] = "Dragon\n";

//Pins:
// LED1 = P11
// TOGGLE = P14
// RXD1 = P16
// TXD1 = P17
// LED2 = P22
// LED3 = P34
// UDM  = P50
// UDP  = P51

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
	UINT8 prev_control_line_state;
	
	rcc_set_clk_freq(RCC_CLK_FREQ_24M);
	
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_7);	//LED1, TOGGLE, TXD1
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_1, GPIO_PIN_6);	//RXD1
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_2, GPIO_PIN_2);	//LED2
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_3, GPIO_PIN_4);	//LED3
	
	timer_init(TIMER_0, NULL);
	timer_set_period(TIMER_0, FREQ_SYS / 1000ul);	//period is 1ms
	uart1_init(BAUD_RATE, UART_1_P16_P17);
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_1);
	timer_long_delay(TIMER_0, 250);
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_1);
	timer_long_delay(TIMER_0, 250);
	if(RESET_KEEP)
		uart_write_string(UART_1, dragon_string);
	else
		uart_write_string(UART_1, unicorn_string);
	
	byte_to_hex(RESET_KEEP, last_keep_str);
	last_keep_str[2] = '\n';
	last_keep_str[3] = '\0';
	uart_write_string(UART_1, last_keep_str);
	
	cdc_init();
	cdc_set_serial_state(0x03);
	prev_control_line_state = cdc_control_line_state;

	while(TRUE)
	{
		RESET_KEEP = (IE & 0xF0) | (SCON & 0x0F);
		if(cdc_bytes_available())
		{
			gpio_toggle_pin(GPIO_PORT_1, GPIO_PIN_1);
			uart_write_byte(UART_1, cdc_read_byte());
			gpio_write_pin(GPIO_PORT_2, GPIO_PIN_2, gpio_read_pin(GPIO_PORT_1, GPIO_PIN_1));
		}
		
		if(uart_bytes_available(UART_1))
		{
			gpio_toggle_pin(GPIO_PORT_1, GPIO_PIN_1);
			cdc_write_byte(uart_read_byte(UART_1));
			gpio_write_pin(GPIO_PORT_2, GPIO_PIN_2, gpio_read_pin(GPIO_PORT_1, GPIO_PIN_1));
		}
		
		if(prev_control_line_state != cdc_control_line_state)
		{
			cdc_set_serial_state(cdc_control_line_state & 3);
			prev_control_line_state = cdc_control_line_state;
		}
	}
}
