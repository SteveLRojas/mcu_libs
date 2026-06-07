#include "ch32v20x.h"
#include "ch32v203_afio.h"
#include "ch32v203_core.h"
#include "ch32v203_dma.h"
#include "ch32v203_gpio.h"
#include "ch32v203_rcc.h"
#include "ch32v203_uart_dma.h"
#include "ch32v203_usbd.h"	//for usbd_cdc_scrutinize
#include "ch32v203_usbd_cdc.h"
#include "debug.h"

//Pins:
// LED3 = PA8
// TXD = PA9
// RXD = PA10
// UDM = PA11
// UDP = PA12
// LED2 = PC13
// LED1 = PC14
// LED0 = PC15

static const char hex_table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
void cdc_print_bytes(uint8_t* src, uint16_t num_bytes)
{
	uint8_t count = 0;
	uint8_t value;
	while(num_bytes)
	{
		value = *src;
		while(cdc_bytes_available_for_write() < 3);
		cdc_write_byte(hex_table[(value >> 4) & 0x0f]);
		cdc_write_byte(hex_table[(value) & 0x0f]);
		--num_bytes;
		++count;
		++src;
		if(!(count & 0x0F))
		{
			cdc_write_byte('\n');
		}
	}
}

void usbd_cdc_scrutinize(void)
{
	//CDC variables
	printf("ep2_t0_zlp: %02X\n", ep2_t0_zlp);
	printf("ep2_t1_zlp: %02X\n", ep2_t1_zlp);
	printf("ep2_read_select: %02X\n", ep2_read_select);
	printf("ep2_t0_num_bytes: %02X\n", ep2_t0_num_bytes);
	printf("ep2_t0_read_offset: %02X\n", ep2_t0_read_offset);
	printf("ep2_t1_num_bytes: %02X\n", ep2_t1_num_bytes);
	printf("ep2_t1_read_offset: %02X\n", ep2_t1_read_offset);
	printf("ep3_wip: %02X\n", ep3_wip);
	printf("ep3_write_select: %02X\n", ep3_write_select);
	printf("ep3_t0_num_bytes: %02X\n", ep3_t0_num_bytes);
	printf("ep3_t1_num_bytes: %02X\n", ep3_t1_num_bytes);
	printf("cdc_address: %02X\n", cdc_address);
	printf("cdc_config: %02X\n", cdc_config);
	printf("cdc_control_line_state: %02X\n", cdc_control_line_state);

	//USBD registers
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
	rcc_apb2_clk_enable(RCC_AFIOEN | RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_ADC1EN | RCC_ADC2EN | RCC_TIM1EN | RCC_SPI1EN | RCC_USART1EN);
	rcc_apb1_clk_enable(RCC_TIM2EN | RCC_USBEN);
	rcc_ahb_clk_enable(RCC_DMA1EN);
	afio_pcfr1_remap(AFIO_PCFR1_SWJ_CFG_DISABLE);

	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_AFIO_PP, GPIO_PIN_9);
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_IN | GPIO_MODE_FLOAT_IN, GPIO_PIN_10);
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_8);
	gpio_set_mode(GPIOC, GPIO_DIR_SPD_OUT_2MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

	core_delay_init();
	dma_init();
	uart_dma_init(USART1, 125000);

	// blink the led once
	gpio_set_pin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
	gpio_set_pin(GPIOA, GPIO_PIN_8);
	core_delay_ms(100);
	gpio_clear_pin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
	gpio_clear_pin(GPIOA, GPIO_PIN_8);
	core_delay_ms(100);
	gpio_write_pin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, 1);
	core_delay_ms(100);

	cdc_init();
	cdc_set_serial_state(CDC_SS_TXCARRIER | CDC_SS_RXCARRIER);
	uint8_t prev_control_line_state = cdc_control_line_state;
	//while(!cdc_config);	//Wait for host to configure the CDC interface
	printf("Unicorn\n");

	printf("SYSCLK: %lu\n", rcc_compute_sysclk_freq());
	printf("HCLK: %lu\n", rcc_compute_hclk_freq());
	printf("PCLK1: %lu\n", rcc_compute_pclk1_freq());
	printf("PCLK1_TIM: %lu\n", rcc_compute_pclk1_tim_freq());
	printf("PCLK2: %lu\n", rcc_compute_pclk2_freq());
	printf("PCLK2_TIM: %lu\n", rcc_compute_pclk2_tim_freq());
	printf("ADCCLK: %lu\n", rcc_compute_adcclk());

	uint8_t datagram[2];
	uint16_t bytes_available;
	uint8_t temp;
    while(1)
	{
    	bytes_available = uart_dma_bytes_available(uart_dma_1);
		temp = uart_dma_peek(uart_dma_1);
		if((bytes_available >= 2) && (temp & 0x80))	//Handle write datagram
		{
			uart_dma_read_bytes(uart_dma_1, datagram, 2);

			switch(datagram[0] & 0x7F)
			{

			}
		}
		else if(bytes_available && !(temp & 0x80))	//handle read datagram
		{
			datagram[0] = uart_dma_read_byte(uart_dma_1);

			switch(datagram[0])
			{

			}
		}

    	if(prev_control_line_state != cdc_control_line_state)
		{
			cdc_set_serial_state(cdc_control_line_state & 3);
			prev_control_line_state = cdc_control_line_state;
		}
	}
}
