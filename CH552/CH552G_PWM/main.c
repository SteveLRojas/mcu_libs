#include "CH552.H"
#include "System.h"
#include "CH552_GPIO.h"
#include "CH552_UART.h"
#include "CH552_TIMER.h"
#include "CH552_PWM.h"
	
char code test_string[] = "Booba";

//Pins:
// LED = P11
// PWM_1 = P15
// RXD = P30
// TXD = P31
// PWM_2 = P34

int main()
{
	UINT8 pwm_clk_div;
	UINT8 pwm_1_duty;
	UINT8 pwm_2_duty;
	UINT8 cmd_count = 0;
	
	CfgFsys();	//CH559 clock selection configuration
	
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_1 | GPIO_PIN_5);
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_3, GPIO_PIN_1 | GPIO_PIN_4);
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_3, GPIO_PIN_0);
	
	uart0_init(TIMER_1, BUAD_RATE, UART_0_P30_P31);
	pwm_init(PWM_1 | PWM_2, PWM_ACTIVE_HIGH, 0, PWM_1_P15 | PWM_2_P34);
	pwm_set_duty_cycle(PWM_1 | PWM_2, 0x7F);
	
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
		if(uart_bytes_available(UART_0) > 2)
		{
			pwm_clk_div = uart_read_byte(UART_0);
			pwm_1_duty = uart_read_byte(UART_0);
			pwm_2_duty = uart_read_byte(UART_0);
			pwm_init(PWM_1 | PWM_2, PWM_ACTIVE_HIGH, pwm_clk_div, PWM_1_P15 | PWM_2_P34);
			pwm_set_duty_cycle(PWM_1, pwm_1_duty);
			pwm_set_duty_cycle(PWM_2, pwm_2_duty);
			
			++cmd_count;
			gpio_write_pin(GPIO_PORT_1, GPIO_PIN_1, ~cmd_count & 0x01);
		}
	}
}

