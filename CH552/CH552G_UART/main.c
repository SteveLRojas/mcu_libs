#include "CH552.H"
#include "System.h"
#include "CH552_GPIO.h"
#include "CH552_UART.h"

char code test_string[] = "Unicorn";

//Pins:
// RXD0 = P30
// TXD0 = P31
// LED1 = P11
// LED2 = P14
// LED3 = P15
// RXD1 = P16
// TXD1 = P17

int main()
{
	UINT8 time = 0;
	UINT8 temp = 0;
	
	CfgFsys();
	
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5);
	
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_3, GPIO_PIN_0);
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_3, GPIO_PIN_1);
	
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_1, GPIO_PIN_6);
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_7);
	
	uart0_init(TIMER_1, BAUD_RATE, UART_0_P30_P31);
	uart1_init(BAUD_RATE, UART_1_P16_P17);
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_1);
	mDelaymS(250);
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_1);
	mDelaymS(250);
	uart_write_string(UART_0, test_string);

	while(TRUE)
	{
		++time;
		gpio_write_pin(GPIO_PORT_1, GPIO_PIN_1, time & 0x80);
		
		if(uart_bytes_available(UART_0))
		{
			temp = uart_read_byte(UART_0);
			uart_write_byte(UART_0, temp);
			gpio_write_pin(GPIO_PORT_1, GPIO_PIN_4, !gpio_read_pin(GPIO_PORT_1, GPIO_PIN_4));
		}
		
		if(uart_bytes_available(UART_1))
		{
			temp = uart_read_byte(UART_1);
			uart_write_byte(UART_1, temp);
			gpio_write_pin(GPIO_PORT_1, GPIO_PIN_5, !gpio_read_pin(GPIO_PORT_1, GPIO_PIN_5));
		}
		
		mDelayuS(1000);
	}
}

