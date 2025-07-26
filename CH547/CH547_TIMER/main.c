#include "CH547.H"
#include "CH547_RCC.h"
#include "CH547_GPIO.h"
#include "CH547_UART.h"
#include "CH547_TIMER.h"

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

UINT8 delay;

void on_timer(void)
{
	gpio_toggle_pin(GPIO_PORT_1, GPIO_PIN_4);
}

int main()
{
	UINT16 delay_cycles = 24;
	UINT8 temp = 0;
	
	rcc_set_clk_freq(RCC_CLK_FREQ_24M);
	
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_7);	//LED1, TOGGLE, TXD1
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_1, GPIO_PIN_6);	//RXD1
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_2, GPIO_PIN_2);	//LED2
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_3, GPIO_PIN_4);	//LED3
	
	timer_init(TIMER_0, on_timer);
	timer_set_period(TIMER_0, FREQ_SYS / 1000ul);	//period is 1ms
	timer_init(TIMER_2, NULL);
	timer_set_interrupt_enable(TIMER_2, 0);	//disable interrupts for timer 2
	
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

	while(TRUE)
	{
		if(uart_bytes_available(UART_1))
		{
			temp = uart_read_byte(UART_1);
			uart_write_byte(UART_1, temp);
			delay_cycles = temp;
			delay_cycles = delay_cycles << 4;
			gpio_toggle_pin(GPIO_PORT_1, GPIO_PIN_1);
			RESET_KEEP = !RESET_KEEP;
		}
		
		if(delay_cycles)
		{
			for(temp = 0; temp < 100; ++temp)
			{
				//gpio_set_pin(GPIO_PORT_1, GPIO_PIN_4);
				SCS = 1;
				timer_short_delay(TIMER_2, delay_cycles);
				//gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_4);
				SCS = 0;
				timer_short_delay(TIMER_2, delay_cycles);
			}
		}
		else
		{
			for(temp = 0; temp < 100; ++temp)
			{
				SCS = 1;
				rcc_delay_us(1);
				SCS = 0;
				rcc_delay_us(1);
			}
		}
	}
}
