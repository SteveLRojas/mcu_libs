#include "ch32v20x.h"
#include "ch32v203_afio.h"
#include "ch32v203_core.h"
#include "debug.h"
#include "fifo.h"
#include "ch32v203_uart.h"
#include "ch32v203_gpio.h"
#include "ch32v203_rcc.h"
#include "ch32v203_adc.h"

//Pins:
// ADC0 = PA0
// ADC1 = PA1
// LED0 = PA8
// TXD = PA9
// RXD = PA10
// UDM = PA11
// UDP = PA12
// LED1 = PA13
// LED2 = PA14
// LED3 = PA15

const uint8_t test_schedule[] = {ADC_IN0, ADC_IN1};
volatile uint16_t test_results[18];

int main(void)
{
	rcc_apb2_clk_enable(RCC_AFIOEN | RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_ADC1EN | RCC_ADC2EN | RCC_TIM1EN | RCC_SPI1EN | RCC_USART1EN);
	rcc_apb1_clk_enable(RCC_TIM2EN | RCC_USBEN);
	afio_pcfr1_remap(AFIO_PCFR1_SWJ_CFG_DISABLE);

	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_8 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);	//LED0, LED1, LED2, LED3
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_AFIO_PP, GPIO_PIN_9);	//TXD
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_IN | GPIO_MODE_FLOAT_IN, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_10);		//ADC0, ADC1, RXD

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

	adc_init(ADC1);
	adc1_results = (uint16_t*)test_results;
	adc1_schedule = (uint8_t*)test_schedule;
	adc1_schedule_len = 2;
	adc_init_schedule(ADC1);
	core_enable_irq(ADC_IRQn);

	printf("Unicorn\n");

	while(1)
	{
		adc_run_schedule(ADC1, 2);
		delay_ms(50);
		printf("Channel 0: %04X, Channel 1: %04X\n", test_results[0], test_results[1]);
		gpio_toggle_pin(GPIOA, GPIO_PIN_8);
	}
}

