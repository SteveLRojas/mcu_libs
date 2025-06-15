#include "ch32v20x.h"
#include "ch32v203_afio.h"
#include "ch32v203_core.h"
#include "debug.h"
#include "fifo.h"
#include "ch32v203_uart.h"
#include "ch32v203_gpio.h"
#include "ch32v203_rcc.h"
#include "ch32v203_spi.h"

//Pins:
// LED3 = PA8
// TXD = PA9
// RXD = PA10
// UDM = PA11
// UDP = PA12
// SPI1_NCS = PA15
// SPI1_SCK = PB3
// SPI1_MISO = PB4
// SPI1_MOSI = PB5
// LED2 = PC13
// LED1 = PC14
// LED0 = PC15

int main(void)
{
	rcc_apb2_clk_enable(RCC_AFIOEN | RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_ADC1EN | RCC_ADC2EN | RCC_TIM1EN | RCC_SPI1EN | RCC_USART1EN);
	afio_pcfr1_remap(AFIO_PCFR1_SWJ_CFG_DISABLE);

	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_AFIO_PP, GPIO_PIN_9);
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_IN | GPIO_MODE_FLOAT_IN, GPIO_PIN_10);
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_8 | GPIO_PIN_15);
	gpio_set_mode(GPIOB, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_AFIO_PP, GPIO_PIN_3 | GPIO_PIN_5);
	gpio_set_mode(GPIOB, GPIO_DIR_SPD_IN | GPIO_MODE_PULL_IN, GPIO_PIN_4);
	gpio_set_mode(GPIOC, GPIO_DIR_SPD_OUT_2MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
	gpio_set_pin(GPIOB, GPIO_PIN_4);

	delay_init();
	uart_init(USART1, 115200);
	core_enable_irq(USART1_IRQn);

	printf("SYSCLK: %u\n", rcc_compute_sysclk_freq());
	printf("HCLK: %u\n", rcc_compute_hclk_freq());
	printf("PCLK1: %u\n", rcc_compute_pclk1_freq());
	printf("PCLK2: %u\n", rcc_compute_pclk2_freq());
	printf("ADCCLK: %u\n", rcc_compute_adcclk());

	spi_init(SPI1, SPI_8_BIT | SPI_CLK_DIV_16 | SPI_MODE_0);
	afio_pcfr1_remap(AFIO_PCFR1_SPI1_REMAP);
	gpio_set_pin(GPIOA, GPIO_PIN_15);

	// blink the led once
	gpio_set_pin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
	gpio_set_pin(GPIOA, GPIO_PIN_8);
	delay_ms(100);
	gpio_clear_pin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
	gpio_set_pin(GPIOA, GPIO_PIN_8);
	delay_ms(100);
	gpio_write_pin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, 1);
	delay_ms(100);
	printf("Unicorn\n");

	uint8_t time = 0;
	uint8_t temp = 0;
	while(1)
	{
		++time;
		gpio_write_pin(GPIOA, GPIO_PIN_8, time & 0x80);

		if(uart_bytes_available(uart1_rx_fifo))
		{
			temp = uart_read_byte(USART1, uart1_rx_fifo);
			gpio_clear_pin(GPIOA, GPIO_PIN_15);
			temp = (uint8_t)spi_transfer(SPI1, temp);
			gpio_set_pin(GPIOA, GPIO_PIN_15);
			uart_write_byte(USART1, uart1_tx_fifo, temp);
			gpio_toggle_pin(GPIOC, GPIO_PIN_13);
		}

		delay_ms(1);
	}
}
