#include "ch32v20x.h"
#include "ch32v203_core.h"
#include "debug.h"
#include "fifo.h"
#include "ch32v203_uart.h"
#include "ch32v203_gpio.h"
#include "ch32v203_spi.h"
#include "ch32v203_rcc.h"
#include "ch32v203_afio.h"
#include "ch32v203_timer.h"

//Pins:
// LED3 = PA8
// TXD = PA9
// RXD = PA10
// UDM = PA11
// UDP = PA12
// LED2 = PA13
// LED1 = PA14
// LED0 = PA15

void on_timer1(void)
{
	gpio_toggle_pin(GPIOA, GPIO_PIN_8);
}

void on_timer2(void)
{
	gpio_toggle_pin(GPIOA, GPIO_PIN_15);
	gpio_toggle_pin(GPIOA, GPIO_PIN_14);
}

int main(void)
{
	rcc_apb2_clk_enable(RCC_AFIOEN | RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_TIM1EN | RCC_SPI1EN | RCC_USART1EN);
	rcc_apb1_clk_enable(RCC_TIM2EN);
	afio_pcfr1_remap(AFIO_PCFR1_SWJ_CFG_DISABLE);

	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_8 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);	//LED0, LED1, LED2, LED3
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_AFIO_PP, GPIO_PIN_9);	//TXD
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_IN | GPIO_MODE_FLOAT_IN, GPIO_PIN_10);		//RXD

	delay_init();
	delay_ms(1);
	uart_init(USART1, 115200);
	core_enable_irq(USART1_IRQn);

	printf("SYSCLK: %u\n", rcc_compute_sysclk_freq());
	printf("HCLK: %u\n", rcc_compute_hclk_freq());
	printf("PCLK1: %u\n", rcc_compute_pclk1_freq());
	printf("PCLK2: %u\n", rcc_compute_pclk2_freq());
	printf("ADCCLK: %u\n", rcc_compute_adcclk());

	// blink the led once
	gpio_set_pin(GPIOA, GPIO_PIN_8 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
	delay_ms(100);
	gpio_clear_pin(GPIOA, GPIO_PIN_8 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
	delay_ms(100);
	gpio_write_pin(GPIOA, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, 1);
	delay_ms(100);

	timer_init(TIM1, 96000000 / 10000 - 1, 1000 - 1);	//100us time scale, 100ms period
	timer_init(TIM2, 96000000 / 1000000 - 1, 1000 - 1);	//1us time scale, 1ms period
	timer1_callback = on_timer1;
	timer2_callback = on_timer2;
	timer_enable_interrupt(TIM1);
	timer_enable_interrupt(TIM2);
	core_enable_irq(TIM1_UP_IRQn);
	core_enable_irq(TIM2_IRQn);
	timer_start(TIM1);

	printf("Unicorn\n");

	uint32_t iteration_count;

	while(1)
	{
		//test long delay
		iteration_count = 1000;
		timer_set_period(TIM2, 1000 - 1);	//1ms period
		do
		{
			timer_long_delay(TIM2, 2);	//2ms delay
			gpio_toggle_pin(GPIOA, GPIO_PIN_13);
		} while(--iteration_count);

		//test short delay
		iteration_count = 10000;
		do
		{
			timer_short_delay(TIM2, 200);	//200us delay
			gpio_toggle_pin(GPIOA, GPIO_PIN_13);
		} while(--iteration_count);
	}
}
