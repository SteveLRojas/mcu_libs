#include "ch32v30x.h"
#include "ch32v307_core.h"
#include "ch32v307_gpio.h"
#include "ch32v307_rcc.h"

//Pins:
// OTG_DM = PA11
// OTG_DP = PA12
// LED0 = PC0
// LED1 = PC1
// LED2 = PC2
// LED3 = PC3


int main(void)
{
	rcc_apb2_clk_enable(RCC_AFIOEN | RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_TIM1EN | RCC_SPI1EN | RCC_USART1EN);

	gpio_set_mode(GPIOC, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

	core_delay_init();

	gpio_set_pin(GPIOC, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
	core_delay_ms(100);
	gpio_clear_pin(GPIOC, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
	core_delay_ms(100);
	gpio_write_pin(GPIOC, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 1);
	core_delay_ms(100);

	/*if(((*(uint32_t *) 0x40022030) & 0x0F000000) == 0)
	{
    	printf("Something is fishy with port C\n");
	}*/

	uint8_t count = 0;
	while(1)
	{
    	gpio_write_port(GPIOC, count);
    	++count;
    	core_delay_ms(125);
	}
}
