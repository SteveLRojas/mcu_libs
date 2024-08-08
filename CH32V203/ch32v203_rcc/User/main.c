#include "debug.h"
#include "fifo.h"
#include "ch32v203_uart.h"
#include "ch32v203_gpio.h"
#include "ch32v203_spi.h"
#include "ch32v203_rcc.h"

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

#define UART_RX_INT_DISABLE(uart) ((uart)->CTLR1 &= (~USART_CTLR1_RXNEIE))
#define UART_RX_INT_ENABLE(uart) ((uart)->CTLR1 |= USART_CTLR1_RXNEIE)

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1 | RCC_APB2Periph_USART1, ENABLE);
	rcc_apb2_clk_enable(RCC_AFIOEN | RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_SPI1EN | RCC_USART1EN);

	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_AFIO_PP, GPIO_PIN_9);
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_IN | GPIO_MODE_FLOAT_IN, GPIO_PIN_10);
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_8 | GPIO_PIN_15);
	gpio_set_mode(GPIOB, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_AFIO_PP, GPIO_PIN_3 | GPIO_PIN_5);
	gpio_set_mode(GPIOB, GPIO_DIR_SPD_IN | GPIO_MODE_PULL_IN, GPIO_PIN_4);
	gpio_set_mode(GPIOC, GPIO_DIR_SPD_OUT_2MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
	gpio_set_pin(GPIOB, GPIO_PIN_4);

	Delay_Init();
	uart_init(USART1, 115200);
	NVIC_EnableIRQ(USART1_IRQn);

	printf("SystemClk:%d\n", rcc_compute_sysclk_freq());
	printf("Unicorn\n");

	spi_init(SPI1, SPI_8_BIT | SPI_CLK_DIV_16 | SPI_MODE_0);
	AFIO->PCFR1 |= AFIO_PCFR1_SPI1_REMAP;
	gpio_set_pin(GPIOA, GPIO_PIN_15);

	// blink the led once
	gpio_set_pin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
	gpio_set_pin(GPIOA, GPIO_PIN_8);
	Delay_Ms(100);
	gpio_clear_pin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
	gpio_set_pin(GPIOA, GPIO_PIN_8);
	Delay_Ms(100);
	gpio_write_pin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, 1);
	Delay_Ms(100);
	printf("Blink done\n");

	/*printf("From old RCC library:\n");
	RCC_ClocksTypeDef RCC_ClocksStatus;
	RCC_GetClocksFreq(&RCC_ClocksStatus);
	printf("SYSCLK: %u\n", RCC_ClocksStatus.SYSCLK_Frequency);
	printf("HCLK: %u\n", RCC_ClocksStatus.HCLK_Frequency);
	printf("PCLK1: %u\n", RCC_ClocksStatus.PCLK1_Frequency);
	printf("PCLK2: %u\n", RCC_ClocksStatus.PCLK2_Frequency);
	printf("ADCCLK: %u\n", RCC_ClocksStatus.ADCCLK_Frequency);*/

	printf("From new RCC library:\n");
	printf("SYSCLK: %u\n", rcc_compute_sysclk_freq());
	printf("HCLK: %u\n", rcc_compute_hclk_freq());
	printf("PCLK1: %u\n", rcc_compute_pclk1_freq());
	printf("PCLK2: %u\n", rcc_compute_pclk2_freq());
	printf("ADCCLK: %u\n", rcc_compute_adcclk());

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

		Delay_Ms(1);
	}
}
