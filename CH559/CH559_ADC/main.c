#include "CH559.H"
#include "DEBUG.H"
#include "CH559_GPIO.h"
#include "CH559_TIMER.h"
#include "CH559_UART.h"
#include "CH559_ADC.h"

char code test_string[] = "Unicorn\n";

//Pins:
// RXD0 = P02
// TXD0 = P03
// AIN0 = P10
// AIN1 = P11
// AIN2 = P12
// AIN3 = P13
// LED1 = P14
// LED2 = P15
// AIN6 = P16
// AIN7 = P17

int main()
{
	UINT8 idx;
	
	CfgFsys();	//CH559 clock selection configuration
	
	// Configure UART pins
	gpio_set_port_mode(GPIO_PORT_MODE_OC, GPIO_PORT_0);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_0);
	gpio_set_pin_mode(GPIO_MODE_OC_PU, GPIO_PORT_0, GPIO_PIN_2);
	gpio_set_pin_mode(GPIO_MODE_OC_PU_PULSE, GPIO_PORT_0, GPIO_PIN_3);
	
	// Configure LED and ADC pins
	gpio_set_port_mode(GPIO_PORT_MODE_PP, GPIO_PORT_1);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_1);
	gpio_set_pin_mode(GPIO_MODE_INPUT, GPIO_PORT_1, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_6 | GPIO_PIN_7);
	gpio_set_pin_mode(GPIO_MODE_OUTPUT_PP, GPIO_PORT_1, GPIO_PIN_4 | GPIO_PIN_5);
	
	uart0_init(TIMER_1, BAUD_RATE, UART_0_P02_P03);
	timer_init(TIMER_0, NULL);
	timer_set_period(TIMER_0, FREQ_SYS / 1000ul);	//period is 1ms
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	adc_init(ADC_SLOW | 0x02, ADC_AIN0 | ADC_AIN1 | ADC_AIN2 | ADC_AIN3 | ADC_AIN6 | ADC_AIN7);
	adc_schedule[0] = ADC_AIN0;
	adc_schedule[1] = ADC_AIN1;
	adc_schedule[2] = ADC_AIN2;
	adc_schedule[3] = ADC_AIN3;
	adc_schedule[4] = ADC_AIN6;
	adc_schedule[5] = ADC_AIN7;
	adc_schedule_len = 6;
	adc_init_schedule();
	
	//Blink LED once
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_4);
	timer_long_delay(TIMER_0, 250);
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_4);
	timer_long_delay(TIMER_0, 250);
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_4);
	uart_write_string(UART_0, test_string);

	while(TRUE)
	{	
		adc_run_schedule(6);
		timer_long_delay(TIMER_0, 250);
		for(idx = 0; idx < 6; ++idx)
		{
			uart_write_byte(UART_0, (UINT8)(adc_results[idx] >> 3));
		}
		
		gpio_toggle_pin(GPIO_PORT_1, GPIO_PIN_5);
	}
}
