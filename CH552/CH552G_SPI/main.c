#include "CH552.H"
#include "System.h"
#include "CH552_GPIO.h"
#include "CH552_UART.h"
#include "CH552_SPI.h"

char code test_string[] = "Booba";

int main()
{
	UINT8 time = 0;
	UINT8 temp = 0;
	
	CfgFsys();	//CH559 clock selection configuration
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_1, GPIO_PIN_6);
	spi_init(5, SPI_MODE_0);
	
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_3, GPIO_PIN_1);
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_3, GPIO_PIN_0);
	uart0_init(TIMER_1, BUAD_RATE, UART_0_P30_P31);
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_4);
	mDelaymS(250);
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_4);
	mDelaymS(250);
	uart_write_string(UART_0, test_string);
	spi_transfer(0xAA);
	spi_transfer(0x55);

	while(TRUE)
	{
		++time;
		gpio_write_pin(GPIO_PORT_1, GPIO_PIN_4, time & 0x80);
		
		if(uart_bytes_available(UART_0))
		{
			temp = uart_read_byte(UART_0);
			temp = spi_transfer(temp);
			uart_write_byte(UART_0, temp);
		}
		
		mDelayuS(1000);
	}
}

