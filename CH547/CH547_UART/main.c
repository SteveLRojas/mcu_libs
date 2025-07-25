#include "CH547.H"
#include "CH547_RCC.h"
#include "CH547_GPIO.h"
#include "CH547_UART.h"

#define  BAUD_RATE  125000ul

char code test_string[] = "Unicorn\n";
char code button_string[] = "Dragon\n";

//Pins:
// LED1 = P11
// RXD1 = P16
// TXD1 = P17
// LED2 = P22
// RXD0 = P30
// TXD0 = P31
// LED3 = P34
// UDM  = P50
// UDP  = P51
// SW1  = P57

int main()
{
	UINT8 time = 0;
	UINT8 temp = 0;
	UINT8 sw1_prev = 0;
	
	rcc_set_clk_freq(RCC_CLK_FREQ_24M);
	
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_1 | GPIO_PIN_7);	//LED1, TXD1
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_1, GPIO_PIN_6);	//RXD1
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_2, GPIO_PIN_2);	//LED2
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_3, GPIO_PIN_1 | GPIO_PIN_4);	//TXD0, LED3
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_3, GPIO_PIN_0);	//RXD0
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_5, GPIO_PIN_7);	//SW1
	
	uart0_init(TIMER_1, BAUD_RATE, UART_0_P30_P31);
	uart1_init(BAUD_RATE, UART_1_P16_P17);
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_1);
	rcc_delay_ms(250);
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_1);
	rcc_delay_ms(250);
	uart_write_string(UART_0, test_string);

	while(TRUE)
	{
		++time;
		gpio_write_pin(GPIO_PORT_1, GPIO_PIN_1, time & 0x80);
		
		if(uart_bytes_available(UART_0))
		{
			temp = uart_read_byte(UART_0);
			uart_write_byte(UART_0, temp);
			gpio_toggle_pin(GPIO_PORT_2, GPIO_PIN_2);
		}
		
		if(uart_bytes_available(UART_1))
		{
			temp = uart_read_byte(UART_1);
			uart_write_byte(UART_1, temp);
			gpio_toggle_pin(GPIO_PORT_3, GPIO_PIN_4);
		}
		
		temp = gpio_read_pin(GPIO_PORT_5, GPIO_PIN_7);
		if(temp & ~sw1_prev)
		{
			uart_write_string(UART_0, button_string);
			uart_write_string(UART_1, button_string);
		}
		sw1_prev = temp;
		
		rcc_delay_us(1000);
	}
}
