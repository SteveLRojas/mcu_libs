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
// TXD2 = PA2
// RXD2 = PA3
// LED = PA8
// TXD1 = PA9
// RXD1 = PA10
// UDM = PA11
// UDP = PA12

extern volatile uint8_t ep2_t0_zlp;
extern volatile uint8_t ep2_t1_zlp;
extern volatile uint8_t ep2_read_select;
extern volatile uint8_t ep2_t0_num_bytes;
extern uint8_t ep2_t0_read_offset;
extern volatile uint8_t ep2_t1_num_bytes;
extern uint8_t ep2_t1_read_offset;
extern volatile uint8_t ep3_wip;
extern volatile uint8_t ep3_write_select;
extern volatile uint8_t ep3_t0_num_bytes;
extern volatile uint8_t ep3_t1_num_bytes;
extern uint8_t cdc_address;

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

	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_8);	//LED
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_AFIO_PP, GPIO_PIN_2 | GPIO_PIN_9);	//TXD2, TXD1
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_IN | GPIO_MODE_PULL_IN, GPIO_PIN_3 | GPIO_PIN_10);		//RXD2, RXD1

	core_delay_init();
	dma_init();
	uart_dma_init(USART1, 125000);
	uart_dma_init(USART2, 125000);

	// blink the led once
	gpio_set_pin(GPIOA, GPIO_PIN_8);
	core_delay_ms(100);
	gpio_clear_pin(GPIOA, GPIO_PIN_8);
	core_delay_ms(100);
	gpio_set_pin(GPIOA, GPIO_PIN_8);
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

	uint8_t cdc_buf[256] __attribute__ ((aligned(2)));
	uint8_t cdc_idx = 0;
	uint8_t line_coding_idx = 0;
	uint8_t cdc_byte_read = 0;

	uint8_t usbd_config[64] __attribute__ ((aligned(2)));	//only 50 bytes are needed, power of 2 to simplify address limiting
	uint8_t config_idx = 0;
	uint8_t usbd_packet[64] __attribute__ ((aligned(2)));
	uint8_t packet_idx = 0;
	uint8_t ep_sel = 0;
	uint16_t arg16 = 0;
    while(1)
	{
    	bytes_available = uart_dma_bytes_available(uart_dma_1);
		temp = uart_dma_peek(uart_dma_1);
		if((bytes_available >= 2) && (temp & 0x80))	//Handle write datagram
		{
			uart_dma_read_bytes(uart_dma_1, datagram, 2);

			switch(datagram[0] & 0x7F)
			{
				case 0x08:
					cdc_idx = datagram[1];
					break;
				case 0x09:
					cdc_buf[cdc_idx] = datagram[1];
					cdc_idx += 1;
					break;
				case 0x0B:
					line_coding_idx = datagram[1];
					break;
				case 0x0C:
					((uint8_t*)&cdc_line_coding)[line_coding_idx & 0x07] = datagram[1];
					line_coding_idx += 1;
					break;
				case 0x0E:
					cdc_init();
					break;
				case 0x0F:
					cdc_set_serial_state(datagram[1]);
					break;
				case 0x12:
					cdc_byte_read = cdc_read_byte();
					break;
				case 0x13:
					cdc_read_bytes(cdc_buf, datagram[1]);
					break;
				case 0x15:
					cdc_write_byte(datagram[1]);
					break;
				case 0x16:
					cdc_write_bytes(cdc_buf, datagram[1]);
					break;
				case 0x17:
					cdc_write_string((char*)cdc_buf);
					break;

				case 0x18:
					config_idx = datagram[1];
					break;
				case 0x19:
					usbd_config[config_idx & 0x3F] = datagram[1];
					++config_idx;
					break;
				case 0x1A:
					packet_idx = datagram[1];
					break;
				case 0x1B:
					usbd_packet[packet_idx & 0x3F] = datagram[1];
					++packet_idx;
					break;
				case 0x1C:
					ep_sel = datagram[1] & 0x07;
					break;
				case 0x1F:
					usbd_init((usbd_config_t*)usbd_config);
					break;
				case 0x20:
					usbd_disable();
					break;
				case 0x21:
					usbd_write_to_pma(usbd_get_tx_0_buf_offset(ep_sel), (uint16_t*)usbd_packet, (uint16_t)datagram[1]);
					break;
				case 0x22:
					usbd_write_to_pma(usbd_get_tx_1_buf_offset(ep_sel), (uint16_t*)usbd_packet, (uint16_t)datagram[1]);
					break;
				case 0x23:
					usbd_read_from_pma(usbd_get_rx_0_buf_offset(ep_sel), (uint16_t*)usbd_packet, (uint16_t)datagram[1]);
					break;
				case 0x24:
					usbd_read_from_pma(usbd_get_rx_1_buf_offset(ep_sel), (uint16_t*)usbd_packet, (uint16_t)datagram[1]);
					break;
				case 0x25:
					usbd_write_bytes_to_pma(usbd_get_tx_0_buf_offset(ep_sel), usbd_packet, (uint16_t)datagram[1]);
					break;
				case 0x26:
					usbd_write_bytes_to_pma(usbd_get_tx_1_buf_offset(ep_sel), usbd_packet, (uint16_t)datagram[1]);
					break;
				case 0x27:
					arg16 &= 0xFF00;
					arg16 |= datagram[1];
					break;
				case 0x28:
					arg16 &= 0x00FF;
					arg16 |= ((uint16_t)datagram[1]) << 8;
					break;
				case 0x29:
					usbd_enable_interrupts(arg16);
					break;
				case 0x2A:
					usbd_disable_interrupts(arg16);
					break;
				case 0x2B:
					usbd_set_addr(datagram[1]);
					break;
				case 0x2C:
					usbd_set_tx_0_len(ep_sel, (uint16_t)datagram[1]);
					break;
				case 0x2D:
					usbd_set_tx_1_len(ep_sel, (uint16_t)datagram[1]);
					break;
				case 0x2E:
					usbd_set_out_toggle(ep_sel, datagram[1] ? USBD_OUT_TOG_1 : USBD_OUT_TOG_0);
					break;
				case 0x2F:
					usbd_set_in_toggle(ep_sel, datagram[1] ? USBD_IN_TOG_1 : USBD_IN_TOG_0);
					break;
				case 0x30:
					usbd_set_out_res(ep_sel, (uint16_t)(datagram[1] & 0x30) << 8);
					break;
				case 0x31:
					usbd_set_in_res(ep_sel, (uint16_t)(datagram[1] & 0x30));
					break;
			}
		}
		else if(bytes_available && !(temp & 0x80))	//handle read datagram
		{
			datagram[0] = uart_dma_read_byte(uart_dma_1);

			switch(datagram[0])
			{
				case 0x00: //device_id[0]
					datagram[0] = 'C';
					break;
				case 0x01:	//device_id[1]
					datagram[0] = 'D';
					break;
				case 0x02:	//device_id[2]
					datagram[0] = 'C';
					break;
				case 0x03:	//device_id[3]
					datagram[0] = '1';
					break;
				case 0x04:	//unique_id[0]
					datagram[0] = *(uint8_t*)0x1FFFF7E8;
					break;
				case 0x05:	//unique_id[1]
					datagram[0] = *(uint8_t*)0x1FFFF7E9;
					break;
				case 0x06:	//unique_id[2]
					datagram[0] = *(uint8_t*)0x1FFFF7EA;
					break;
				case 0x07:	//unique_id[3]
					datagram[0] = *(uint8_t*)0x1FFFF7EB;
					break;

				case 0x08:
					datagram[0] = cdc_idx;
					break;
				case 0x09:
					datagram[0] = cdc_buf[cdc_idx];
					cdc_idx += 1;
					break;
				case 0x0A:
					datagram[0] = cdc_config;
					break;
				case 0x0B:
					datagram[0] = line_coding_idx;
					break;
				case 0x0C:
					datagram[0] = ((uint8_t*)&cdc_line_coding)[line_coding_idx];
					line_coding_idx += 1;
					break;
				case 0x0D:
					datagram[0] = cdc_control_line_state;
					break;
				case 0x10:
					datagram[0] = (uint8_t)cdc_bytes_available();
					break;
				case 0x11:
					datagram[0] = cdc_peek();
					break;
				case 0x12:
					datagram[0] = cdc_byte_read;
					break;
				case 0x14:
					datagram[0] = cdc_bytes_available_for_write();
					break;

				case 0x18:
					datagram[0] = config_idx;
					break;
				case 0x19:
					datagram[0] = usbd_config[config_idx];
					++config_idx;
					break;
				case 0x1A:
					datagram[0] = packet_idx;
					break;
				case 0x1B:
					datagram[0] = usbd_packet[packet_idx];
					++packet_idx;
					break;
				case 0x1C:
					datagram[0] = ep_sel;
					break;
				case 0x1D:
					datagram[0] = (uint8_t)sof_count;
					break;
				case 0x1E:
					datagram[0] = (uint8_t)(sof_count >> 8);
					break;
				case 0x27:
					datagram[0] = (uint8_t)arg16;
					break;
				case 0x28:
					datagram[0] = (uint8_t)(arg16 >> 8);
					break;
				case 0x2C:
					datagram[0] = (uint8_t)usbd_get_rx_0_len(ep_sel);
					break;
				case 0x2D:
					datagram[0] = (uint8_t)usbd_get_rx_1_len(ep_sel);
					break;
				case 0x2E:
					datagram[0] = (usbd_get_out_toggle(ep_sel) != 0);
					break;
				case 0x2F:
					datagram[0] = (usbd_get_in_toggle(ep_sel) != 0);
					break;
				case 0x30:
					datagram[0] = (uint8_t)(usbd_get_out_res(ep_sel) >> 8);
					break;
				case 0x31:
					datagram[0] = (uint8_t)usbd_get_in_res(ep_sel);
					break;
				default:
					datagram[0] = 0x00;
					break;
			}
			uart_dma_write_byte(uart_dma_1, datagram[0]);
		}

    	if(prev_control_line_state != cdc_control_line_state)
		{
			cdc_set_serial_state(cdc_control_line_state & 3);
			prev_control_line_state = cdc_control_line_state;
		}
	}
}
