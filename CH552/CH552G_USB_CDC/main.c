#include "CH552.H"
#include "System.h"
#include "CH552_GPIO.h"
#include "CH552_UART.h"
#include "CH552_TIMER.h"
#include "usb_cdc.h"
	
char code test_string[] = "Unicorn\n";

//Pins:
// LED = P11
// TEST = P14
// RXD = P30
// TXD = P31
// UDP = P36
// UDM = P37

void usb_halt(UINT8 keep)
{
	RESET_KEEP = keep;
	USB_CTRL = 0;
	IE_USB = 0;
	
	while(TRUE)
	{
		gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_1);
		mDelaymS(50);
		gpio_set_pin(GPIO_PORT_1, GPIO_PIN_1);
		mDelaymS(50);
	}
}

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
	
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_1 | GPIO_PIN_4);
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_3, GPIO_PIN_1);
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_3, GPIO_PIN_0);
	
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
	
	byte_to_hex(RESET_KEEP, last_keep_str);
	last_keep_str[2] = '\n';
	last_keep_str[3] = '\0';
	uart_write_string(UART_0, last_keep_str);
	
	cdc_init();
	//cdc_set_serial_state(cdc_control_line_state);
	
	while(TRUE)
	{	
		if(cdc_bytes_available())
		{
			uart_write_byte(UART_0, cdc_read_byte());
			gpio_write_pin(GPIO_PORT_1, GPIO_PIN_1, !gpio_read_pin(GPIO_PORT_1, GPIO_PIN_1));
			//cdc_set_serial_state(cdc_control_line_state);
		}
		
		if(uart_bytes_available(UART_0))
		{
			cdc_write_byte(uart_read_byte(UART_0));
			gpio_write_pin(GPIO_PORT_1, GPIO_PIN_4, !gpio_read_pin(GPIO_PORT_1, GPIO_PIN_4));
		}
	}
}

