#include "CH559.H"
#include "DEBUG.H"
#include "CH559_GPIO.h"
#include "CH559_TIMER.h"
#include "CH559_UART.h"
#include "CH559_PWM.h"

char code test_string[] = "Unicorn";

//Pins:
// RXD0 = P02
// TXD0 = P03
// LED1 = P14
// LED2 = P15
// PWM1 = P24
// PWM2 = P25
// PWM3 = P42

int main()
{
	UINT8 pwm_clk_div;
	UINT8 pwm_max_count;
	UINT8 pwm_1_duty;
	UINT8 pwm_2_duty;
	
	CfgFsys();	//CH559 clock selection configuration
	
	// Configure UART pins
	gpio_set_port_mode(GPIO_PORT_MODE_OC, GPIO_PORT_0);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_0);
	gpio_set_pin_mode(GPIO_MODE_OC_PU, GPIO_PORT_0, GPIO_PIN_2);
	gpio_set_pin_mode(GPIO_MODE_OC_PU_PULSE, GPIO_PORT_0, GPIO_PIN_3);
	
	// Configure LED pins
	gpio_set_port_mode(GPIO_PORT_MODE_PP, GPIO_PORT_1);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_1);
	gpio_set_pin_mode(GPIO_MODE_OUTPUT_PP, GPIO_PORT_1, GPIO_PIN_4 | GPIO_PIN_5);
	
	// Configure PWM pins
	gpio_set_port_mode(GPIO_PORT_MODE_PP, GPIO_PORT_2);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_2);
	gpio_set_pin_mode(GPIO_MODE_OUTPUT_PP, GPIO_PORT_2, GPIO_PIN_4 | GPIO_PIN_5);
	
	gpio_set_port_mode(GPIO_PORT_MODE_PP, GPIO_PORT_4);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_4);
	gpio_set_pin_mode(GPIO_MODE_OUTPUT_PP, GPIO_PORT_4, GPIO_PIN_2);
	
	uart0_init(TIMER_1, BAUD_RATE, UART_0_P02_P03);
	timer_init(TIMER_0, NULL);
	timer_set_period(TIMER_0, FREQ_SYS / 1000ul);	//period is 1ms
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	pwm1_2_init(PWM_1_ACTIVE_HIGH | PWM_2_ACTIVE_HIGH, 1, 0, PWM_1_2_P24_P25);
	pwm3_init(PWM_3_ACTIVE_HIGH, 1, 0, PWM_3_P42);
	pwm3_set_duty_cycle(0x7FFF);
	
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_4);
	timer_long_delay(TIMER_0, 250);
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_4);
	timer_long_delay(TIMER_0, 250);
	uart_write_string(UART_0, test_string);

	while(TRUE)
	{	
		if(uart_bytes_available(UART_0) > 3)
		{
			pwm_clk_div = uart_read_byte(UART_0);
			pwm_max_count = uart_read_byte(UART_0);
			pwm_1_duty = uart_read_byte(UART_0);
			pwm_2_duty = uart_read_byte(UART_0);
			pwm1_2_init(PWM_1_ACTIVE_HIGH | PWM_2_ACTIVE_HIGH, pwm_clk_div, pwm_max_count, PWM_1_2_P24_P25);
			pwm_set_duty_cycle(PWM_1, pwm_1_duty);
			pwm_set_duty_cycle(PWM_2, pwm_2_duty);
			
			gpio_toggle_pin(GPIO_PORT_1, GPIO_PIN_5);
		}
	}
}
