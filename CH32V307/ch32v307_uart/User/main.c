#include "ch32v30x.h"
#include "ch32v307_core.h"
#include "ch32v307_gpio.h"
#include "ch32v307_rcc.h"
#include "debug.h"
#include "fifo.h"
#include "ch32v307_uart.h"

//Pins:
// TXD = PA9
// RXD = PA10
// OTG_DM = PA11
// OTG_DP = PA12
// LED0 = PC0
// LED1 = PC1
// LED2 = PC2
// LED3 = PC3


int main(void)
{
	rcc_apb2_clk_enable(RCC_AFIOEN | RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_TIM1EN | RCC_SPI1EN | RCC_USART1EN);

	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_AFIO_PP, GPIO_PIN_9);	//TXD
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_IN | GPIO_MODE_FLOAT_IN, GPIO_PIN_10);		//RXD
	gpio_set_mode(GPIOC, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

	core_delay_init();
	uart_init(USART1, 115200);
	core_enable_irq(USART1_IRQn);

	printf("SYSCLK: %u\n", rcc_compute_sysclk_freq());
	printf("HCLK: %u\n", rcc_compute_hclk_freq());
	printf("PCLK1: %u\n", rcc_compute_pclk1_freq());
	printf("PCLK2: %u\n", rcc_compute_pclk2_freq());
	printf("ADCCLK: %u\n", rcc_compute_adcclk());

	gpio_set_pin(GPIOC, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
	core_delay_ms(100);
	gpio_clear_pin(GPIOC, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
	core_delay_ms(100);
	gpio_write_pin(GPIOC, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 1);
	core_delay_ms(100);
	printf("Unicorn\n");

	if(((*(uint32_t *) 0x40022030) & 0x0F000000) == 0)
	{
    	printf("Something is fishy with port C\n");
	}

	uint16_t count = 0;
	uint8_t temp = 0;
	while(1)
	{
		if(uart_bytes_available(uart1_rx_fifo))
		{
			temp = uart_read_byte(USART1, uart1_rx_fifo);
			uart_write_byte(USART1, uart1_tx_fifo, temp);
			gpio_toggle_pin(GPIOC, GPIO_PIN_13);
		}

		gpio_write_port(GPIOC, count >> 7);
		++count;
		core_delay_ms(1);
	}
}
