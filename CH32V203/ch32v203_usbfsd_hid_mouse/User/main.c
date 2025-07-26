#include "ch32v20x.h"
#include "ch32v203_core.h"
#include "debug.h"
#include "fifo.h"
#include "ch32v203_uart.h"
#include "ch32v203_gpio.h"
#include "ch32v203_rcc.h"
#include "usbfsd_hid_mouse.h"

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

//    USBOTG_FS->USB_INT_EN = 0x80;
//    if(USBOTG_FS->USB_INT_EN & 0x80)
//    	printf("The bit exists\n");
//    else
//    	printf("The bit does not exists\n");

    hid_init();

	uint8_t time = 0;
	uint8_t temp = 0;
	while(1)
	{
		if(uart_bytes_available(uart1_rx_fifo))
		{
			temp = uart_read_byte(USART1, uart1_rx_fifo);

			switch(temp)
			{
			case 0x01:
				hid_mouse_press(HID_MOUSE_BTN_LEFT);
				core_delay_ms(50);
				hid_mouse_release(HID_MOUSE_BTN_LEFT);
				break;
			case 0x02:
				hid_mouse_press(HID_MOUSE_BTN_RIGHT);
				core_delay_ms(50);
				hid_mouse_release(HID_MOUSE_BTN_RIGHT);
				break;
			case 0x03:
				hid_mouse_press(HID_MOUSE_BTN_WHEEL);
				core_delay_ms(50);
				hid_mouse_release(HID_MOUSE_BTN_WHEEL);
				break;
			case 0x04:
				temp = uart_read_byte(USART1, uart1_rx_fifo);
				hid_mouse_move(temp, 0x00);
				break;
			case 0x05:
				temp = uart_read_byte(USART1, uart1_rx_fifo);
				hid_mouse_move(0x00, temp);
				break;
			case 0x06:
				temp = uart_read_byte(USART1, uart1_rx_fifo);
				hid_mouse_scroll(temp);
				break;
			default:
				uart_write_byte(USART1, uart1_tx_fifo, 0xFF);
				break;
			}

			gpio_toggle_pin(GPIOA, GPIO_PIN_8);
		}

		core_delay_ms(1);
		++time;

		if(hid_idle_rate && ((time >> 2) >= hid_idle_rate))	//idle rate is in units of 4 ms
		{
			hid_mouse_send_report();
			time = 0;
		}
	}
}

