#include "ch32v20x.h"
#include "ch32v203_core.h"
#include "debug.h"
#include "fifo.h"
#include "ch32v203_uart.h"
#include "ch32v203_gpio.h"
#include "ch32v203_rcc.h"
#include "usbd_hid_mouse.h"

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

	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_AFIO_PP, GPIO_PIN_9);
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_IN | GPIO_MODE_FLOAT_IN, GPIO_PIN_10);
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_8);
	gpio_set_mode(GPIOC, GPIO_DIR_SPD_OUT_2MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
	gpio_set_pin(GPIOB, GPIO_PIN_4);

    Delay_Init();
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
    Delay_Ms(100);
    gpio_clear_pin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_set_pin(GPIOA, GPIO_PIN_8);
    Delay_Ms(100);
    gpio_write_pin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, 1);
    Delay_Ms(100);
    printf("Unicorn\n");

    hid_init();
//    printf("USBD_CNTR: %04X\n", USBD_COMMON->CNTR);
//    printf("USBD_ISTR: %04X\n", USBD_COMMON->ISTR);
//    printf("USBD_DADDR: %04X\n", USBD_COMMON->DADDR);
//    printf("USBD_BTABLE: %04X\n", USBD_COMMON->BTABLE);
//
//    printf("USBD_ADDR0_TX: %04X\n", USBD_BTABLE[0].ADDR_TX);
//    printf("USBD_COUNT0_TX: %04X\n", USBD_BTABLE[0].COUNT_TX);
//    printf("USBD_ADDR0_RX: %04X\n", USBD_BTABLE[0].ADDR_RX);
//    printf("USBD_COUNT0_RX: %04X\n", USBD_BTABLE[0].COUNT_RX);
//
//    printf("USBD_ADDR1_TX: %04X\n", USBD_BTABLE[1].ADDR_TX);
//	printf("USBD_COUNT1_TX: %04X\n", USBD_BTABLE[1].COUNT_TX);
//	printf("USBD_ADDR1_RX: %04X\n", USBD_BTABLE[1].ADDR_RX);
//	printf("USBD_COUNT1_RX: %04X\n", USBD_BTABLE[1].COUNT_RX);

/*
    //Begin PMA access test
    uint16_t test_data[256];

    //byte write test
    pseudo_random_seed(0xDEADBEEF);
    for(uint16_t d = 0; d < 256; ++d)
    {
    	pseudo_random_generate(16);
    	test_data[d] = pseudo_random_get_word();
    }
    printf("Generated random test data\n");

    for(uint16_t d = 0; d < 256; ++d)
    {
    	usbd_write_pma_word(d << 1, test_data[d]);
    }
    printf("Wrote words to PMA\n");

    for(uint16_t d = 0; d < 256; ++d)
    {
    	if(usbd_read_pma_word(d << 1) != test_data[d])
    	{
    		printf("read wrong data at address %d\n", d);
    		printf("Expected %04X, read %04X\n", test_data[d], usbd_read_pma_word(d << 1));
    		break;
    	}
    }
    printf("Done reading and checking words\n");

    usbd_read_from_pma(0, test_data, 256);
	printf("Finished bulk read\n");

	pseudo_random_seed(0xDEADBEEF);
	for(uint16_t d = 0; d < 256; ++d)
	{
		pseudo_random_generate(16);
		if(pseudo_random_get_word() != test_data[d])
		{
			printf("read wrong data at address %d\n", d);
			printf("Expected %04X, read %04X\n", pseudo_random_get_word(), test_data[d]);
			break;
		}
	}
	printf("Done checking words in array\n");

	//word write test
	pseudo_random_seed(0xDEADBEEF);
	for(uint16_t d = 0; d < 256; ++d)
	{
		pseudo_random_generate(16);
		test_data[d] = pseudo_random_get_word();
	}
	printf("Generated random test data\n");

    usbd_write_to_pma(0, test_data, 256);
    printf("Finished bulk write\n");

    for(uint16_t d = 0; d < 256; ++d)
	{
		if(usbd_read_pma_word(d << 1) != test_data[d])
		{
			printf("read wrong data at address %d\n", d);
			break;
		}
	}
	printf("Done reading and checking words\n");

	usbd_read_from_pma(0, test_data, 256);
	printf("Finished bulk read\n");

	pseudo_random_seed(0xDEADBEEF);
	for(uint16_t d = 0; d < 256; ++d)
	{
		pseudo_random_generate(16);
		if(pseudo_random_get_word() != test_data[d])
		{
			printf("read wrong data at address %d\n", d);
			break;
		}
	}
	printf("Done checking words in array\n");
*/

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
				Delay_Ms(50);
				hid_mouse_release(HID_MOUSE_BTN_LEFT);
				break;
			case 0x02:
				hid_mouse_press(HID_MOUSE_BTN_RIGHT);
				Delay_Ms(50);
				hid_mouse_release(HID_MOUSE_BTN_RIGHT);
				break;
			case 0x03:
				hid_mouse_press(HID_MOUSE_BTN_WHEEL);
				Delay_Ms(50);
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

		Delay_Ms(1);
		++time;

		if(hid_idle_rate && ((time >> 2) >= hid_idle_rate))	//idle rate is in units of 4 ms
		{
			hid_mouse_send_report();
			time = 0;
		}
	}
}
