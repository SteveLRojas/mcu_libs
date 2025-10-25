#include "ch32v20x.h"
#include "debug.h"
#include "ch32v203_afio.h"
#include "ch32v203_core.h"
#include "ch32v203_gpio.h"
#include "ch32v203_rcc.h"
#include "ch32v203_timer.h"
#include "ch32v203_usbd_cdc.h"
#include "ch32v203_sd_card.h"
#include "pseudo_random.h"
#include "support.h"

//Pins:
// SD_CS = PA4
// SD_SCK = PA5
// SD_MISO = PA6
// SD_MOSI = PA7
// LED0 = PA8
// TXD = PA9
// RXD = PA10
// UDM = PA11
// UDP = PA12
// LED1 = PA13
// LED2 = PA14
// LED3 = PA15

int main(void)
{
	rcc_apb2_clk_enable(RCC_AFIOEN | RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_ADC1EN | RCC_ADC2EN | RCC_TIM1EN | RCC_SPI1EN | RCC_USART1EN);
	rcc_apb1_clk_enable(RCC_TIM2EN | RCC_USBEN);
	rcc_ahb_clk_enable(RCC_DMA1EN);
	afio_pcfr1_remap(AFIO_PCFR1_SWJ_CFG_DISABLE);

	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_4 | GPIO_PIN_8 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);	//SD_CS, LED0, LED1, LED2, LED3
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_AFIO_PP, GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_9);	//SD_SCK, SD_MOSI, TXD
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_IN | GPIO_MODE_PULL_IN, GPIO_PIN_6 | GPIO_PIN_10);	//RXD
	gpio_set_pin(GPIOA, GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_10);

	//HINT: This timer provides the time counter for the sd card library (see ch32v203_sd_card.h)
	timer_init(TIM1, 96000000 / 1000000 - 1, 1000 - 1);	//1us time scale, 1ms period
	timer_enable_interrupt(TIM1);
	core_enable_irq(TIM1_UP_IRQn);
	timer_start(TIM1);

	core_delay_init();
	// blink the led once
	gpio_set_pin(GPIOA, GPIO_PIN_8 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
	core_delay_ms(100);
	gpio_clear_pin(GPIOA, GPIO_PIN_8 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
	core_delay_ms(100);
	gpio_set_pin(GPIOA, GPIO_PIN_8 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
	core_delay_ms(100);

	cdc_init();
	cdc_set_serial_state(0x03);
	uint8_t prev_control_line_state = cdc_control_line_state;
	while(!cdc_config);	//Wait for host to configure the CDC interface
	printf("Unicorn\n");

	printf("SYSCLK: %u\n", rcc_compute_sysclk_freq());
	printf("HCLK: %u\n", rcc_compute_hclk_freq());
	printf("PCLK1: %u\n", rcc_compute_pclk1_freq());
	printf("PCLK1_TIM: %u\n", rcc_compute_pclk1_tim_freq());
	printf("PCLK2: %u\n", rcc_compute_pclk2_freq());
	printf("PCLK2_TIM: %u\n", rcc_compute_pclk2_tim_freq());
	printf("ADCCLK: %u\n", rcc_compute_adcclk());

	uint8_t datagram[2];
	uint8_t datagram_val;
	uint8_t temp;
	uint8_t card_buf[512];
	uint32_t num_blocks;
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
					sd_card_init();
					printf("Card type: %02X\n", sd_card_type);
					printf("Card status: %04X\n", sd_card_status);
					break;
				case 0x01:
					printf("Card type: %02X\n", sd_card_type);
					printf("Card status: %04X\n", sd_card_status);
					break;
				case 0x02:
					pseudo_random_seed(0x1337BEEF);
					break;
				case 0x03:
					for(uint16_t d = 0; d < 512; ++d)
					{
						pseudo_random_generate(8);
						card_buf[d] = pseudo_random_get_byte();
					}
					break;
				case 0x04:
					for(uint16_t d = 0; d < 512; ++d)
					{
						card_buf[d] = 0;
					}
					break;
				case 0x05:
					cdc_print_bytes(card_buf, 512);
					break;
				case 0x06:
					cdc_write_bytes(card_buf, 512);
					break;
				case 0x07:
					sd_card_read_block(0, card_buf);
					break;
				case 0x08:
					sd_card_read_block(1, card_buf);
					break;
				case 0x09:
					sd_card_read_block(2, card_buf);
					break;
				case 0x0A:
					sd_card_read_block(3, card_buf);
					break;
				case 0x0B:
					sd_card_write_block(0, card_buf);
					break;
				case 0x0C:
					sd_card_write_block(1, card_buf);
					break;
				case 0x0D:
					sd_card_write_block(2, card_buf);
					break;
				case 0x0E:
					sd_card_write_block(3, card_buf);
					break;
				case 0x0F:
					printf(sd_card_is_busy() ? "Card busy\n" : "Not busy\n");
					break;
				case 0x10:
					num_blocks = sd_card_get_num_blocks();
					printf("Num blocks: %u\n", num_blocks);
					printf("Card size (KB): %u\n", num_blocks >> 1);
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
