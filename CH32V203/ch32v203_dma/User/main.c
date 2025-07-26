#include "ch32v20x.h"
#include "debug.h"
#include "ch32v203_afio.h"
#include "ch32v203_core.h"
#include "ch32v203_gpio.h"
#include "ch32v203_rcc.h"
#include "ch32v203_usbd_cdc.h"
#include "ch32v203_dma.h"
#include "pseudo_random.h"
#include "support.h"

//Pins:
// LED0 = PA8
// TXD = PA9
// RXD = PA10
// UDM = PA11
// UDP = PA12
// LED1 = PA13
// LED2 = PA14
// LED3 = PA15

volatile uint32_t test_buf_1[64];
volatile uint32_t test_buf_2[64];

void on_dma_transfer_complete(void)
{
	printf("DMA transfer completed.\n");
}

int main(void)
{
	rcc_apb2_clk_enable(RCC_AFIOEN | RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_ADC1EN | RCC_ADC2EN | RCC_TIM1EN | RCC_SPI1EN | RCC_USART1EN);
	rcc_apb1_clk_enable(RCC_TIM2EN | RCC_USBEN);
	rcc_ahb_clk_enable(RCC_DMA1EN);
	afio_pcfr1_remap(AFIO_PCFR1_SWJ_CFG_DISABLE);

	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_8 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);	//LED0, LED1, LED2, LED3
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_AFIO_PP, GPIO_PIN_9);	//TXD
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_IN | GPIO_MODE_FLOAT_IN, GPIO_PIN_10);		//RXD

    core_delay_init();
	// blink the led once
	gpio_set_pin(GPIOA, GPIO_PIN_8 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
	core_delay_ms(100);
	gpio_clear_pin(GPIOA, GPIO_PIN_8 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
	core_delay_ms(100);
	gpio_write_pin(GPIOA, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, 1);
	core_delay_ms(100);

	cdc_init();
	cdc_set_serial_state(0x03);
	uint8_t prev_control_line_state = cdc_control_line_state;
	while(!cdc_config);	//Wait for host to configure the CDC interface
	printf("Unicorn\n");

	printf("SYSCLK: %u\n", rcc_compute_sysclk_freq());
	printf("HCLK: %u\n", rcc_compute_hclk_freq());
	printf("PCLK1: %u\n", rcc_compute_pclk1_freq());
	printf("PCLK2: %u\n", rcc_compute_pclk2_freq());
	printf("ADCCLK: %u\n", rcc_compute_adcclk());

	dma_init();
	dma1_channel1_callback = on_dma_transfer_complete;

	uint8_t datagram[2];
	uint8_t datagram_val;
	uint8_t temp;
	while(1)
	{
		if(cdc_bytes_available() >= 2)
		{
			temp = cdc_peek();
			if((temp == '\r') || (temp == '\n'))
			{
				(void)cdc_read_byte();
				continue;
			}

			cdc_read_bytes(datagram, 2);
			datagram_val = 0;
			for(uint8_t count = 0; count < 2; ++count)
			{
				temp = datagram[count];
				if(temp >= '0' && temp <= '9')  //convert numbers
					temp = temp - '0';
				else if(temp >= 'A' && temp <= 'F')   //convert uppercase letters
					temp = temp - 'A' + 10;
				else if(temp >= 'a' && temp <= 'f')   //convert the annoying lowercase letters
					temp = temp - 'a' + 10;
				else
					continue;

				datagram_val = datagram_val << 4;
				datagram_val = datagram_val | temp;
			}

			gpio_toggle_pin(GPIOA, GPIO_PIN_8);
			switch(datagram_val)
			{
				case 0x00:
					pseudo_random_seed(0xDEADBEEF);
					break;
				case 0x01:
					for(uint8_t count = 0; count < 64; ++count)
					{
						pseudo_random_generate(32);
						test_buf_1[count] = pseudo_random_get_dword();
					}
					break;
				case 0x02:
					cdc_print_bytes((uint8_t*)test_buf_1, 256);
					break;
				case 0x03:
					cdc_print_bytes((uint8_t*)test_buf_2, 256);
					break;
				case 0x04:
					dma_channel_config(DMA1_Channel1, DMA_CFG_MEM2MEM_ON | DMA_CFG_PRI_MEDIUM | DMA_CFG_MSIZE_32 | DMA_CFG_PSIZE_32 | DMA_CFG_MINC_ON | DMA_CFG_PINC_ON | DMA_CFG_MEM_TO_PERIPH | DMA_CFG_TCIE_ON);
					break;
				case 0x05:
					dma_channel_config(DMA1_Channel1, DMA_CFG_MEM2MEM_ON | DMA_CFG_PRI_MEDIUM | DMA_CFG_MSIZE_32 | DMA_CFG_PSIZE_32 | DMA_CFG_MINC_ON | DMA_CFG_PINC_ON | DMA_CFG_PERIPH_TO_MEM | DMA_CFG_TCIE_ON);
					break;
				case 0x06:
					dma_transfer(DMA1_Channel1, (uint32_t)test_buf_1, (uint32_t)test_buf_2, 64);
					break;
				case 0x07:
					for(uint8_t count = 0; count < 64; ++count)
					{
						test_buf_1[count] = 0xFFFFFFFF;
					}
					break;
				case 0x08:
					for(uint8_t count = 0; count < 64; ++count)
					{
						test_buf_2[count] = 0xFFFFFFFF;
					}
					break;
				default:
					printf("Bad command!\n");
					break;
			}
			gpio_write_pin(GPIOA, GPIO_PIN_13, gpio_read_pin(GPIOA, GPIO_PIN_8));
		}

		if(prev_control_line_state != cdc_control_line_state)
		{
			cdc_set_serial_state(cdc_control_line_state & 3);
			prev_control_line_state = cdc_control_line_state;
		}
	}
}
