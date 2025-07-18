#include "CH547.H"
#include "CH547_RCC.h"
#include "CH547_GPIO.h"

//Pins:
// LED1 = P11
// LED2 = P31
// SW1  = P57
// UDM  = P50
// UDP  = P51

UINT8 delay;

void on_gpio_int(void)
{
	gpio_toggle_pin(GPIO_PORT_3, GPIO_PIN_1);
	if(delay == 125)
		delay = 250;
	else
		delay = 125;
}

int main()
{
	rcc_set_clk_freq(RCC_CLK_FREQ_24M);
	
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_1);
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_3, GPIO_PIN_1);
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_5, GPIO_PIN_7);
	
	gpio_callback = on_gpio_int;
	gpio_set_interrupt_mode(GPIO_INT_MODE_EDGE);
	gpio_enable_interrupts(GPIO_INT_P5_7_RISE);
	gpio_global_interrupt_enable();
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	delay = 125;

	while (TRUE)
	{
		gpio_set_pin(GPIO_PORT_1, GPIO_PIN_1);
		rcc_delay_ms(delay);
		gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_1);
		rcc_delay_ms(delay);
	}
}
