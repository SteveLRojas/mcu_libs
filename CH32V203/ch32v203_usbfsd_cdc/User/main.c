#include "ch32v20x.h"
#include "ch32v203_core.h"
#include "debug.h"
#include "fifo.h"
#include "ch32v203_uart.h"
#include "ch32v203_gpio.h"
#include "ch32v203_rcc.h"
#include "ch32v203_usbfsd_cdc.h"

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
	rcc_apb1_clk_enable(RCC_TIM2EN | RCC_USBEN);
	rcc_ahb_clk_enable(RCC_USBFS);

	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_AFIO_PP, GPIO_PIN_9);
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_IN | GPIO_MODE_FLOAT_IN, GPIO_PIN_10);
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_8);
	gpio_set_mode(GPIOC, GPIO_DIR_SPD_OUT_2MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
	gpio_set_pin(GPIOB, GPIO_PIN_4);

    core_delay_init();
    uart_init(USART1, 115200);
    core_enable_irq(USART1_IRQn);

    printf("SYSCLK: %u\n", rcc_compute_sysclk_freq());
	printf("HCLK: %u\n", rcc_compute_hclk_freq());
	printf("PCLK1: %u\n", rcc_compute_pclk1_freq());
	printf("PCLK2: %u\n", rcc_compute_pclk2_freq());
	printf("ADCCLK: %u\n", rcc_compute_adcclk());

    // blink the led once
    gpio_set_pin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_set_pin(GPIOA, GPIO_PIN_8);
    core_delay_ms(100);
    gpio_clear_pin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_set_pin(GPIOA, GPIO_PIN_8);
    core_delay_ms(100);
    gpio_write_pin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, 1);
    core_delay_ms(100);
    printf("Unicorn\n");

    cdc_init();
    cdc_set_serial_state(0x03);
    uint8_t prev_control_line_state = cdc_control_line_state;

	while(1)
	{
		while(cdc_bytes_available())
		{
			while(uart_bytes_available_for_write(uart1_tx_fifo) == 0);
			gpio_toggle_pin(GPIOA, GPIO_PIN_8);
			uart_write_byte(USART1, uart1_tx_fifo, cdc_read_byte());
			gpio_write_pin(GPIOC, GPIO_PIN_13, gpio_read_pin(GPIOA, GPIO_PIN_8));
		}

		if(uart_bytes_available(uart1_rx_fifo))
		{
			gpio_toggle_pin(GPIOC, GPIO_PIN_14);
			cdc_write_byte(uart_read_byte(USART1, uart1_rx_fifo));
			gpio_write_pin(GPIOC, GPIO_PIN_15, gpio_read_pin(GPIOC, GPIO_PIN_14));
		}

		if(prev_control_line_state != cdc_control_line_state)
		{
			cdc_set_serial_state(cdc_control_line_state & 3);
			prev_control_line_state = cdc_control_line_state;
		}
	}
}

