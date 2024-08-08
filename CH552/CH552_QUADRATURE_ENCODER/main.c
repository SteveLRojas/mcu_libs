#include "CH552.H"
#include "System.h"
#include "CH552_GPIO.h"
#include "CH552_UART.h"
#include "CH552_TIMER.h"
#include "CH552_QUADRATURE_ENCODER.h"
	
char code test_string[] = "Unicorn\n";
char code debounced_str[] = "Debounced count: ";
char code raw_str[] = "Raw count: ";

//Pins:
// LED1 = P11
// LED2 = P14
// RXD = P30
// TXD = P31
// ENCA = P32
// ENCB = P33
// ENCSW = P34
// UDP = P36
// UDM = P37

void byte_to_hex(UINT8 value, char* buff)
{
	const char table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	buff[0] = table[(value >> 4) & 0x0f];
	buff[1] = table[(value) & 0x0f];
	buff[2] = '\0';
}

int main()
{
	char hex_str[4];
	UINT8 prev_count = 0;
	UINT8 prev_raw_count = 0;
	
	CfgFsys();	//CH559 clock selection configuration
	
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_1 | GPIO_PIN_4);
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_3, GPIO_PIN_1);
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_3, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4);
	
	uart0_init(TIMER_1, BAUD_RATE, UART_0_P30_P31);
	timer_init(TIMER_0, NULL);
	timer_set_period(TIMER_0, FREQ_SYS / 1000ul);	//period is 1ms
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	//Blink LED once
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_1);
	timer_long_delay(TIMER_0, 250);
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_1);
	timer_long_delay(TIMER_0, 250);
	uart_write_string(UART_0, test_string);
	
	byte_to_hex(RESET_KEEP, hex_str);
	hex_str[2] = '\n';
	hex_str[3] = '\0';
	uart_write_string(UART_0, hex_str);
	
	qenc_init(QENC_MODE_INTERRUPT);
	
	while(TRUE)
	{
		gpio_write_pin(GPIO_PORT_1, GPIO_PIN_1, INT0);
		gpio_write_pin(GPIO_PORT_1, GPIO_PIN_4, INT1);
		
		if((UINT8)qenc_count != prev_count)
		{
			prev_count = (UINT8)qenc_count;
			uart_write_string(UART_0, debounced_str);
			byte_to_hex(prev_count, hex_str);
			hex_str[2] = '\n';
			hex_str[3] = '\0';
			uart_write_string(UART_0, hex_str);	
		}
		
		if((UINT8)qenc_raw_count != prev_raw_count)
		{
			prev_raw_count = (UINT8)qenc_raw_count;
			uart_write_string(UART_0, raw_str);
			byte_to_hex(prev_raw_count, hex_str);
			hex_str[2] = '\n';
			hex_str[3] = '\0';
			uart_write_string(UART_0, hex_str);
		}
	}
}

