#include "ch32v20x.h"
#include "ch32v203_core.h"
#include "debug.h"
#include "fifo.h"
#include "ch32v203_uart.h"
#include "ch32v203_gpio.h"
#include "ch32v203_rcc.h"

//Pins:
// LED3 = PA8
// TXD = PA9
// RXD = PA10
// UDM = PA11
// UDP = PA12
// LED2 = PC13
// LED1 = PC14
// LED0 = PC15


int main(void)
{
	rcc_apb2_clk_enable(RCC_AFIOEN | RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_TIM1EN | RCC_SPI1EN | RCC_USART1EN);

	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_AFIO_PP, GPIO_PIN_9);
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_IN | GPIO_MODE_FLOAT_IN, GPIO_PIN_10);
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_8);
	gpio_set_mode(GPIOC, GPIO_DIR_SPD_OUT_2MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

	Delay_Init();
	uart_init(USART1, 115200);
	core_enable_irq(USART1_IRQn);

	printf("SYSCLK: %u\n", rcc_compute_sysclk_freq());
	printf("HCLK: %u\n", rcc_compute_hclk_freq());
	printf("PCLK1: %u\n", rcc_compute_pclk1_freq());
	printf("PCLK2: %u\n", rcc_compute_pclk2_freq());
	printf("ADCCLK: %u\n", rcc_compute_adcclk());

	gpio_set_pin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
	gpio_set_pin(GPIOA, GPIO_PIN_8);
	Delay_Ms(100);
	gpio_clear_pin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
	gpio_clear_pin(GPIOA, GPIO_PIN_8);
	Delay_Ms(100);
	gpio_write_pin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, 1);
	Delay_Ms(100);
	printf("Unicorn\n");

	if(((*(uint32_t *) 0x40022030) & 0x0F000000) == 0)
	{
    	printf("Something is fishy with port C\n");
	}

	while(1)
	{
    	gpio_toggle_pin(GPIOA, GPIO_PIN_8);
    	gpio_toggle_pin(GPIOC, GPIO_PIN_15);
    	Delay_Ms(50);
    	gpio_write_pin(GPIOC, GPIO_PIN_14, gpio_read_pin(GPIOC, GPIO_PIN_15));
    	Delay_Ms(50);
    	gpio_write_pin(GPIOC, GPIO_PIN_13, gpio_read_pin(GPIOC, GPIO_PIN_14));
    	Delay_Ms(50);
	}
}
