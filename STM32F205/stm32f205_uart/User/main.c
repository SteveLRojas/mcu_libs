#include "stm32f205xx.h"
#include "stm32f205_core.h"
#include "stm32f205_gpio.h"
#include "stm32f205_rcc.h"
#include "debug.h"
#include "fifo.h"
#include "stm32f205_uart.h"

//Pins:
// TXD2 = PD5
// RXD2 = PD6
// LED0 = PE0
// LED1 = PE1


int main(void)
{
	rcc_ahb1_clk_enable(RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_GPIOHEN);
	apb1_clk_enable(RCC_APB1ENR_USART2EN | RCC_APB1ENR_PWREN);
	apb2_clk_enable(RCC_APB2ENR_SYSCFGEN);
	core_enable_global_irq();

	gpio_set_mode(GPIOD, GPIO_MODE_AFIO, GPIO_TYPE_PP, GPIO_SPEED_HIGHEST, GPIO_PULL_UP, GPIO_PIN_5);	//TXD2
	gpio_set_mode(GPIOD, GPIO_MODE_AFIO, GPIO_TYPE_PP, GPIO_SPEED_HIGHEST, GPIO_PULL_UP, GPIO_PIN_6);		//RXD2
	gpio_set_mode(GPIOE, GPIO_MODE_OUTPUT, GPIO_TYPE_PP, GPIO_SPEED_HIGH, GPIO_PULL_NONE, GPIO_PIN_0 | GPIO_PIN_1);	//LED0, LED1
	gpio_set_pin(GPIOD, GPIO_PIN_5);
	gpio_set_alternate_function(GPIOD, GPIO_FUNCTION_AF7, GPIO_PIN_5 | GPIO_PIN_6);

	core_delay_init();
	uart_init(USART2, 115200);
	core_nvic_enable_irq(USART2_IRQn);

	printf("SYSCLK: %u\n", rcc_compute_sysclk_freq());
	printf("HCLK: %u\n", rcc_compute_hclk_freq());
	printf("PCLK1: %u\n", rcc_compute_pclk1_freq());
	printf("PCLK2: %u\n", rcc_compute_pclk2_freq());

	uint32_t pll_prediv = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;
	uint32_t pll_mul = (RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> RCC_PLLCFGR_PLLN_Pos;
	uint32_t pll_div = 2 << ((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >> RCC_PLLCFGR_PLLP_Pos);

	printf("pll_prediv: %u\n", pll_prediv);
	printf("pll_mul: %u\n", pll_mul);
	printf("pll_div: %u\n", pll_div);

	gpio_set_pin(GPIOE, GPIO_PIN_0 | GPIO_PIN_1);
	core_delay_ms(100);
	gpio_clear_pin(GPIOE, GPIO_PIN_0 | GPIO_PIN_1);
	core_delay_ms(100);
	gpio_set_pin(GPIOE, GPIO_PIN_0 | GPIO_PIN_1);
	core_delay_ms(100);
	printf("Unicorn\n");

	uint16_t count = 0;
	uint8_t temp = 0;
	while(1)
	{
		if(uart_bytes_available(uart2_rx_fifo))
		{
			temp = uart_read_byte(USART2, uart2_rx_fifo);
			uart_write_byte(USART2, uart2_tx_fifo, temp);
		}

		gpio_write_port(GPIOE, count >> 7);
		++count;
		core_delay_ms(1);
	}
}
