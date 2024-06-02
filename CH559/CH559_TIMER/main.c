#include "CH559.H"
#include "DEBUG.H"
#include "CH559_GPIO.h"
#include "CH559_TIMER.h"

void tim_callback(void)
{
	gpio_toggle_pin(GPIO_PORT_1, GPIO_PIN_5);
}

int main()
{
	CfgFsys();	//CH559 clock selection configuration

	gpio_set_port_mode(GPIO_PORT_MODE_PP, GPIO_PORT_1);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_1);
	gpio_set_pin_mode(GPIO_MODE_OUTPUT_PP, GPIO_PORT_1, GPIO_PIN_4 | GPIO_PIN_5);
	
	timer_init(TIMER_2, tim_callback);
	timer_set_period(TIMER_2, FREQ_SYS / 1000ul);	//period is 1ms
	timer_start(TIMER_2);
	
	timer_init(TIMER_3, NULL);
	timer_set_period(TIMER_3, FREQ_SYS / 1000ul);	//period is 1ms
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	// blink led once
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_4);
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_5);
	timer_long_delay(TIMER_3, 250);
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_4);
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_5);
	timer_long_delay(TIMER_3, 250);

	while(TRUE)
	{
		timer_long_delay(TIMER_3, 125);
		gpio_toggle_pin(GPIO_PORT_1, GPIO_PIN_4);
	}
}

