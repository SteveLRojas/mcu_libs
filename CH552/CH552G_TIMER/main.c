#include "CH552.H"
#include "System.h"
#include "CH552_GPIO.h"
#include "CH552_UART.h"
#include "CH552_TIMER.h"

char code test_string[] = "Booba";
UINT8 test = 0;

void tim_callback(void)
{
	test = !test;
	gpio_write_pin(GPIO_PORT_1, GPIO_PIN_4, test);
}

int main()
{
	UINT8 led_state = 0;
	UINT8 delay_cycles = 24;
	UINT8 temp = 0;
	
	CfgFsys();	//CH559 clock selection configuration
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_1 | GPIO_PIN_4);
	
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_3, GPIO_PIN_1);
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_3, GPIO_PIN_0);
	uart0_init(TIMER_1, BUAD_RATE, UART_0_P30_P31);
	
	timer_init(TIMER_0, tim_callback);
	timer_set_period(TIMER_0, FREQ_SYS / 1000ul);	//period is 1ms
	
	timer_init(TIMER_2, NULL);
	timer_set_interrupt_enable(TIMER_2, 0);	//disable interrupts for timer 2
	
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_4);
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_1);
	timer_long_delay(TIMER_0, 250);
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_1);
	timer_long_delay(TIMER_0, 250);
	uart_write_string(UART_0, test_string);
	
	while(TRUE)
	{	
		if(uart_bytes_available(UART_0))
		{
			delay_cycles = uart_read_byte(UART_0);
			uart_write_byte(UART_0, delay_cycles);
			gpio_write_pin(GPIO_PORT_1, GPIO_PIN_1, led_state);
			led_state = !led_state;
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
				//mDelayuS(1);
				SCS = 0;
				//mDelayuS(1);
			}
		}
	}
}

