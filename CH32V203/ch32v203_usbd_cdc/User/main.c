#include "ch32v20x.h"
#include "ch32v203_core.h"
#include "debug.h"
#include "fifo.h"
#include "ch32v203_uart.h"
#include "ch32v203_gpio.h"
#include "ch32v203_rcc.h"
#include "ch32v203_usbd.h"
#include "ch32v203_usbd_cdc.h"

//Pins:
// LED3 = PA8
// TXD = PA9
// RXD = PA10
// UDM = PA11
// UDP = PA12
// LED2 = PC13
// LED1 = PC14
// LED0 = PC15

void usbd_scrutinize(void)
{
	printf("USBD_CNTR: %04X\n", USBD_COMMON->CNTR);
	printf("USBD_ISTR: %04X\n", USBD_COMMON->ISTR);
	printf("USBD_DADDR: %04X\n", USBD_COMMON->DADDR);
	printf("USBD_BTABLE: %04X\n", USBD_COMMON->BTABLE);

	printf("USBD_EPR0: %04X\n", USBD_ENDPOINT[0].EPR);
	printf("USBD_EPR1: %04X\n", USBD_ENDPOINT[1].EPR);
	printf("USBD_EPR2: %04X\n", USBD_ENDPOINT[2].EPR);
	printf("USBD_EPR3: %04X\n", USBD_ENDPOINT[3].EPR);
	printf("USBD_EPR4: %04X\n", USBD_ENDPOINT[4].EPR);
	printf("USBD_EPR5: %04X\n", USBD_ENDPOINT[5].EPR);
	printf("USBD_EPR6: %04X\n", USBD_ENDPOINT[6].EPR);
	printf("USBD_EPR7: %04X\n", USBD_ENDPOINT[7].EPR);

	printf("USBD_ADDR0_TX: %04X\n", USBD_BTABLE[0].ADDR_TX);
	printf("USBD_COUNT0_TX: %04X\n", USBD_BTABLE[0].COUNT_TX);
	printf("USBD_ADDR0_RX: %04X\n", USBD_BTABLE[0].ADDR_RX);
	printf("USBD_COUNT0_RX: %04X\n", USBD_BTABLE[0].COUNT_RX);

	printf("USBD_ADDR1_TX: %04X\n", USBD_BTABLE[1].ADDR_TX);
	printf("USBD_COUNT1_TX: %04X\n", USBD_BTABLE[1].COUNT_TX);
	printf("USBD_ADDR1_RX: %04X\n", USBD_BTABLE[1].ADDR_RX);
	printf("USBD_COUNT1_RX: %04X\n", USBD_BTABLE[1].COUNT_RX);

	printf("USBD_ADDR2_TX: %04X\n", USBD_BTABLE[2].ADDR_TX);
	printf("USBD_COUNT2_TX: %04X\n", USBD_BTABLE[2].COUNT_TX);
	printf("USBD_ADDR2_RX: %04X\n", USBD_BTABLE[2].ADDR_RX);
	printf("USBD_COUNT2_RX: %04X\n", USBD_BTABLE[2].COUNT_RX);

	printf("USBD_ADDR3_TX: %04X\n", USBD_BTABLE[3].ADDR_TX);
	printf("USBD_COUNT3_TX: %04X\n", USBD_BTABLE[3].COUNT_TX);
	printf("USBD_ADDR3_RX: %04X\n", USBD_BTABLE[3].ADDR_RX);
	printf("USBD_COUNT3_RX: %04X\n", USBD_BTABLE[3].COUNT_RX);
}

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

    cdc_init();
    usbd_scrutinize();
    cdc_set_serial_state(0x03);
    uint8_t prev_control_line_state = cdc_control_line_state;

    //extern volatile uint8_t ep2_t0_num_bytes;
    //extern uint8_t ep2_t0_read_offset;
    //extern volatile uint8_t ep2_t1_num_bytes;
    //extern uint8_t ep2_t1_read_offset;
	while(1)
	{
		while(cdc_bytes_available())
		{
//			printf("bytes available: %d\n", cdc_bytes_available());
//			printf("t0 num_bytes: %d\n", ep2_t0_num_bytes);
//			printf("t0 read offset: %d\n", ep2_t0_read_offset);
//			printf("t1 num_bytes: %d\n", ep2_t1_num_bytes);
//			printf("t1 read_offset: %d\n", ep2_t1_read_offset);
//
//			while(uart_bytes_available_for_write(uart1_tx_fifo) == 0);
			gpio_toggle_pin(GPIOA, GPIO_PIN_8);
			uart_write_byte(USART1, uart1_tx_fifo, cdc_read_byte());
			gpio_write_pin(GPIOC, GPIO_PIN_13, gpio_read_pin(GPIOA, GPIO_PIN_8));
		}

		if(uart_bytes_available(uart1_rx_fifo))
		{
			if(uart_peek(uart1_rx_fifo) == 'S')
			{
				usbd_scrutinize();
			}

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

//    extern volatile uint16_t sof_count;
//    extern uint16_t cdc_last_status_time;
//    while(1)
//    {
//    	printf("SOF count: %d\n", sof_count);
//    	printf("Status time: %d\n", cdc_last_status_time);
//    	Delay_Ms(750);
//    }
}

