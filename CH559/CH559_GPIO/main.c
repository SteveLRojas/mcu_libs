#include "CH559.H"
#include "DEBUG.H"
#include "CH559_GPIO.h"

int main()
{
	CfgFsys();	//CH559 clock selection configuration

	gpio_set_port_mode(GPIO_PORT_MODE_PP, GPIO_PORT_1);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_1);
	gpio_set_pin_mode(GPIO_MODE_OUTPUT_PP, GPIO_PORT_1, GPIO_PIN_4 | GPIO_PIN_5);
	
	// blink led once
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_4);
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_5);
	mDelaymS(250);
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_4);
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_5);

	while(TRUE)
	{
		mDelaymS(125);
		gpio_write_pin(GPIO_PORT_1, GPIO_PIN_5, gpio_read_pin(GPIO_PORT_1, GPIO_PIN_4));
		gpio_toggle_pin(GPIO_PORT_1, GPIO_PIN_4);
	}
}

