#include "ch32v20x.h"
#include "ch32v203_afio.h"
#include "ch32v203_core.h"
#include "debug.h"
#include "fifo.h"
#include "ch32v203_uart.h"
#include "ch32v203_gpio.h"
#include "ch32v203_rcc.h"
#include "ch32v203_exti.h"

//Pins:
// LED0 = PA8
// TXD = PA9
// RXD = PA10
// UDM = PA11
// UDP = PA12
// LED1 = PA13
// LED2 = PA14
// LED3 = PA15
// BTN0 = PB0


void on_exti0(void)
{
	printf("EXTI0 triggered!\n");
	gpio_toggle_pin(GPIOA, GPIO_PIN_8);
}

void on_exti1(void)
{
	printf("EXTI1 triggered!\n");
	gpio_toggle_pin(GPIOA, GPIO_PIN_13);
}

void on_exti2(void)
{
	printf("EXTI2 triggered!\n");
	gpio_toggle_pin(GPIOA, GPIO_PIN_14);
}

void on_exti3(void)
{
	printf("EXTI3 triggered!\n");
	gpio_toggle_pin(GPIOA, GPIO_PIN_15);
}

int main(void)
{
	rcc_apb2_clk_enable(RCC_AFIOEN | RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_TIM1EN | RCC_SPI1EN | RCC_USART1EN);
	rcc_apb1_clk_enable(RCC_TIM2EN | RCC_USBEN);
	afio_pcfr1_remap(AFIO_PCFR1_SWJ_CFG_DISABLE);

	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_8 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);	//LED0, LED1, LED2, LED3
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_AFIO_PP, GPIO_PIN_9);	//TXD
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_IN | GPIO_MODE_FLOAT_IN, GPIO_PIN_10);		//RXD
	gpio_set_mode(GPIOB, GPIO_DIR_SPD_IN | GPIO_MODE_FLOAT_IN, GPIO_PIN_0);

    core_delay_init();
    core_delay_ms(1);
    uart_init(USART1, 115200);
    core_enable_irq(USART1_IRQn);

    printf("SYSCLK: %u\n", rcc_compute_sysclk_freq());
	printf("HCLK: %u\n", rcc_compute_hclk_freq());
	printf("PCLK1: %u\n", rcc_compute_pclk1_freq());
	printf("PCLK2: %u\n", rcc_compute_pclk2_freq());
	printf("ADCCLK: %u\n", rcc_compute_adcclk());

	// blink the led once
	gpio_set_pin(GPIOA, GPIO_PIN_8 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
	core_delay_ms(100);
	gpio_clear_pin(GPIOA, GPIO_PIN_8 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
	core_delay_ms(100);
	gpio_write_pin(GPIOA, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, 1);
	core_delay_ms(100);

	afio_exti_config(AFIO_PORT_B, AFIO_PIN_0);
	exti_enable_rising_edge(EXTI_CHAN_0);
	//no edge is enabled for channels 1 to 3, these will be triggered by software
	exti_enable_interrupt(EXTI_CHAN_0 | EXTI_CHAN_1 | EXTI_CHAN_2 | EXTI_CHAN_3);
	exti_clear_interrupt_flag(EXTI_CHAN_0 | EXTI_CHAN_1 | EXTI_CHAN_2 | EXTI_CHAN_3);
	exti0_callback = on_exti0;
	exti1_callback = on_exti1;
	exti2_callback = on_exti2;
	exti3_callback = on_exti3;
	core_enable_irq(EXTI0_IRQn);
	core_enable_irq(EXTI1_IRQn);
	core_enable_irq(EXTI2_IRQn);
	core_enable_irq(EXTI3_IRQn);

	printf("Unicorn\n");

	uint8_t temp;
	while(1)
	{
		if(uart_bytes_available(uart1_rx_fifo))
		{
			temp = uart_read_byte(USART1, uart1_rx_fifo);

			switch(temp)
			{
				case '0':
					exti_trigger_software_interrupt(EXTI_CHAN_0);
					break;
				case '1':
					exti_trigger_software_interrupt(EXTI_CHAN_1);
					break;
				case '2':
					exti_trigger_software_interrupt(EXTI_CHAN_2);
					break;
				case '3':
					exti_trigger_software_interrupt(EXTI_CHAN_3);
					break;
				default: ;
			}

			uart_write_byte(USART1, uart1_tx_fifo, temp);
		}
	}
}

