#include "CH559.H"
#include "CH559_RCC.h"
#include "CH559_GPIO.h"

void on_gpio_int(void)
{
	gpio_toggle_pin(GPIO_PORT_1, GPIO_PIN_4);
}

int main()
{
	rcc_set_clk_freq(RCC_CLK_FREQ_12M);

	gpio_set_port_mode(GPIO_PORT_MODE_PP, GPIO_PORT_1);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_1);
	gpio_set_pin_mode(GPIO_MODE_OUTPUT_PP, GPIO_PORT_1, GPIO_PIN_4 | GPIO_PIN_5);
	gpio_set_port_mode(GPIO_PORT_MODE_PP, GPIO_PORT_4);
	gpio_set_pin_mode(GPIO_MODE_INPUT, GPIO_PORT_4, GPIO_PIN_1);
	
	gpio_callback = on_gpio_int;
	gpio_set_interrupt_mode(GPIO_INT_MODE_EDGE);
	gpio_enable_interrupts(GPIO_INT_P4_1_FALL);
	gpio_global_interrupt_enable();
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	// blink led once
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_4);
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_5);
	rcc_delay_ms(250);
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_4);
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_5);

	while(TRUE)
	{
		rcc_delay_ms(125);
		gpio_toggle_pin(GPIO_PORT_1, GPIO_PIN_5);
	}
}
