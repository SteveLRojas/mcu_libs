#include "CH552.H"
#include "System.h"
#include "CH552_GPIO.h"
#include "CH552_UART.h"
#include "CH552_TIMER.h"
#include "CH552_ADC.h"
	
char code test_string[] = "Booba";

//Pins:
// LED = P11
// TEST = P14
// AIN2 = P15
// RXD = P30
// TXD = P31
// AIN3 = P32

int main()
{
	CfgFsys();	//CH559 clock selection configuration
	
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_1 | GPIO_PIN_4);
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_1, GPIO_PIN_5);
	
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_3, GPIO_PIN_1);
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_3, GPIO_PIN_0 | GPIO_PIN_2);
	uart0_init(TIMER_1, BUAD_RATE, UART_0_P30_P31);
	
	adc_init(ADC_FAST);
	adc_schedule[0] = ADC_AIN2;
	adc_schedule[1] = ADC_AIN3;
	adc_schedule_len = 2;
	adc_init_schedule();
	
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
	
	while(TRUE)
	{	
		adc_run_schedule(2);
		timer_long_delay(TIMER_0, 50);
		uart_write_byte(UART_0, adc_results[0]);
		uart_write_byte(UART_0, adc_results[1]);
		gpio_write_pin(GPIO_PORT_1, GPIO_PIN_1, !gpio_read_pin(GPIO_PORT_1, GPIO_PIN_1));
	}
}

