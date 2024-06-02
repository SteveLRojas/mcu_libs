#include "CH559.H"
#include "DEBUG.H"
#include "CH559_GPIO.h"
#include "CH559_TIMER.h"
#include "CH559_UART.h"
#include "CH559_FIFO.h"

extern volatile fifo_t uart0_rx_fifo;
extern volatile fifo_t uart0_tx_fifo;

char code test_string[] = "Booba";

//Pins:
// RXD0 = P02
// TXD0 = P03
// LED1 = P14
// LED2 = P15
// LED3 = P16
// RXD1 = P26
// TXD1 = P27

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
	gpio_set_pin_mode(GPIO_MODE_OUTPUT_PP, GPIO_PORT_1, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);
	
	gpio_set_port_mode(GPIO_PORT_MODE_OC, GPIO_PORT_2);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_2);
	gpio_set_pin_mode(GPIO_MODE_OC_PU, GPIO_PORT_2, GPIO_PIN_6);
	gpio_set_pin_mode(GPIO_MODE_OC_PU_PULSE, GPIO_PORT_2, GPIO_PIN_7);
	
	uart0_init(TIMER_1, BAUD_RATE, UART_0_P02_P03);
	uart1_init(BAUD_RATE, UART_1_P26_P27);
	timer_init(TIMER_0, NULL);
	timer_set_period(TIMER_0, FREQ_SYS / 1000ul);	//period is 1ms
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_4);
	timer_long_delay(TIMER_0, 250);
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_4);
	timer_long_delay(TIMER_0, 250);
	uart_write_string(UART_0, test_string);

	while(TRUE)
	{
		++time;
		gpio_write_pin(GPIO_PORT_1, GPIO_PIN_4, time & 0x80);
		
		if(uart_bytes_available(UART_0))
		{
			temp = uart_read_byte(UART_0);
			uart_write_byte(UART_0, temp);
			gpio_toggle_pin(GPIO_PORT_1, GPIO_PIN_5);
		}
		
		if(uart_bytes_available(UART_1))
		{
			temp = uart_read_byte(UART_1);
			uart_write_byte(UART_1, temp);
			gpio_toggle_pin(GPIO_PORT_1, GPIO_PIN_6);
		}
		
		timer_long_delay(TIMER_0, 1);
	}
}
