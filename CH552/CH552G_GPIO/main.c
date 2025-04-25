#include "CH552.H"
#include "CH552_RCC.h"
#include "CH552_GPIO.h"

void on_gpio_int(void)
{
	gpio_write_pin(GPIO_PORT_1, GPIO_PIN_6, !gpio_read_pin(GPIO_PORT_1, GPIO_PIN_6));
}

int main()
{
	rcc_set_clk_freq(RCC_CLK_FREQ_16M);
	
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_6 | GPIO_PIN_7);
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_1, GPIO_PIN_3);
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_6);
	
	gpio_callback = on_gpio_int;
	gpio_set_interrupt_mode(GPIO_INT_MODE_EDGE);
	gpio_enable_interrupts(GPIO_INT_P1_3_FALL);
	gpio_global_interrupt_enable();
	EA = 1;	//enable interupts
	E_DIS = 0;

	while (TRUE)
	{
		gpio_set_pin(GPIO_PORT_1, GPIO_PIN_7);
		rcc_delay_ms(125);
		gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_7);
		rcc_delay_ms(125);
	}
}

