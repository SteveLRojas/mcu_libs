#include "stm32f205xx.h"
#include "stm32f205_core.h"
#include "stm32f205_gpio.h"
#include "stm32f205_rcc.h"

//Pins:
// LED0 = PE1


int main(void)
{
	rcc_system_init();
	core_delay_init();

	rcc_ahb1_clk_enable(RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_GPIOHEN);

	gpio_set_mode(GPIOE, GPIO_MODE_OUTPUT, GPIO_TYPE_PP, GPIO_SPEED_HIGH, GPIO_PULL_NONE, GPIO_PIN_0 | GPIO_PIN_1);
	gpio_clear_pin(GPIOE, GPIO_PIN_0 | GPIO_PIN_1);
	gpio_set_pin(GPIOE, GPIO_PIN_1);

	while(1)
	{
		core_delay_ms(50);
		gpio_toggle_pin(GPIOE, GPIO_PIN_1);
	}
}
