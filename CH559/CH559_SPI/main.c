#include "CH559.H"
#include "DEBUG.H"
#include "CH559_GPIO.h"
#include "CH559_TIMER.h"
#include "CH559_UART.h"
#include "CH559_SPI.h"

char code test_string[] = "Unicorn";

//Pins:
// RXD0 = P02
// TXD0 = P03
// LED1 = P14
// SPI0_MOSI = P15
// SPI0_MISO = P16
// SPI0_SCK = P17
// SPI1_MOSI = P21
// SPI1_MISO = P22
// SPI1_SCK = P23

int main()
{
	UINT8 time = 0;
	UINT8 temp = 0;
	
	CfgFsys();	//CH559 clock selection configuration
	
	gpio_set_port_mode(GPIO_PORT_MODE_OC, GPIO_PORT_0);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_0);
	gpio_set_pin_mode(GPIO_MODE_OC_PU, GPIO_PORT_0, GPIO_PIN_2);
	gpio_set_pin_mode(GPIO_MODE_OC_PU_PULSE, GPIO_PORT_0, GPIO_PIN_3);
	
	gpio_set_port_mode(GPIO_PORT_MODE_PP, GPIO_PORT_1);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_1);
	gpio_set_pin_mode(GPIO_MODE_INPUT, GPIO_PORT_1, GPIO_PIN_6);
	gpio_set_pin_mode(GPIO_MODE_OUTPUT_PP, GPIO_PORT_1, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);
	
	gpio_set_port_mode(GPIO_PORT_MODE_PP, GPIO_PORT_2);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_2);
	gpio_set_pin_mode(GPIO_MODE_INPUT, GPIO_PORT_2, GPIO_PIN_2);
	gpio_set_pin_mode(GPIO_MODE_OUTPUT_PP, GPIO_PORT_2, GPIO_PIN_1 | GPIO_PIN_3);
	
	uart0_init(TIMER_1, BAUD_RATE, UART_0_P02_P03);
	timer_init(TIMER_0, NULL);
	timer_set_period(TIMER_0, FREQ_SYS / 1000ul);	//period is 1ms
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	spi_init(SPI_0, 5, SPI_MODE_0);
	spi_init(SPI_1, 3, SPI_MODE_0);
	
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_4);
	timer_long_delay(TIMER_0, 250);
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_4);
	timer_long_delay(TIMER_0, 250);
	
	uart_write_string(UART_0, test_string);
	spi_transfer(SPI_0, 0xAA);
	spi_transfer(SPI_0, 0x55);

	while(TRUE)
	{
		++time;
		gpio_write_pin(GPIO_PORT_1, GPIO_PIN_4, time & 0x80);
		
		if(uart_bytes_available(UART_0))
		{
			temp = uart_read_byte(UART_0);
			uart_write_byte(UART_0, spi_transfer(SPI_0, temp));
			uart_write_byte(UART_0, spi_transfer(SPI_1, temp));
		}
		
		timer_long_delay(TIMER_0, 1);
	}
}
