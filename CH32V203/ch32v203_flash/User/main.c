#include "ch32v20x.h"
#include "debug.h"
#include "ch32v203_afio.h"
#include "ch32v203_core.h"
#include "ch32v203_gpio.h"
#include "ch32v203_rcc.h"
#include "ch32v203_timer.h"
#include "ch32v203_usbd_cdc.h"
#include "ch32v203_flash.h"
#include "pseudo_random.h"
#include "support.h"

//Pins:
// LED0 = PA8
// UDM = PA11
// UDP = PA12
// LED1 = PA13
// LED2 = PA14
// LED3 = PA15

//HINT: The linker script defines the section flash_buf and aligns it to a 32KB block (it will place it at 0x00008000).
//Very important little detail: The linker script uses aliased addresses starting at 0x00000000, add FLASH_BASE to get the actual flash address.
//Aliased addresses can only be used for reading, not writing or erasing.
const uint16_t flash_buf[512] __attribute__((section(".flash_buf")));

int main(void)
{
	rcc_apb2_clk_enable(RCC_AFIOEN | RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_ADC1EN | RCC_ADC2EN | RCC_TIM1EN | RCC_SPI1EN | RCC_USART1EN);
	rcc_apb1_clk_enable(RCC_TIM2EN | RCC_USBEN);
	rcc_ahb_clk_enable(RCC_DMA1EN);
	rcc_hsi_on();	//HSI must be enabled to program the flash
	afio_pcfr1_remap(AFIO_PCFR1_SWJ_CFG_DISABLE);

	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_8 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);	//LED0, LED1, LED2, LED3

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
	uint16_t test_buf[512] __attribute__((aligned(4)));
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
					for(uint16_t count = 0; count < 128; ++count)
					{
						pseudo_random_generate(16);
						test_buf[count] = pseudo_random_get_word();
					}
					break;
				case 0x02:
					for(uint16_t count = 0; count < 512; ++count)
					{
						pseudo_random_generate(16);
						test_buf[count] = pseudo_random_get_word();
					}
					break;
				case 0x03:
					support_print_bytes((uint8_t*)test_buf, 256);
					break;
				case 0x04:
					support_print_bytes((uint8_t*)test_buf, 1024);
					break;
				case 0x05:
					flash_unlock();
					break;
				case 0x06:
					flash_lock();
					break;
				case 0x07:
					flash_write_single16(FLASH_BASE + (uint32_t)flash_buf, test_buf[0]);
					break;
				case 0x08:
					flash_write16(FLASH_BASE + (uint32_t)flash_buf, test_buf, 512);
					break;
				case 0x09:
					flash_erase_page(FLASH_BASE + (uint32_t)flash_buf);
					break;
				case 0x0A:
					flash_fast_unlock();
					break;
				case 0x0B:
					flash_fast_lock();
					break;
				case 0x0C:
					flash_fast_write_page(FLASH_BASE + (uint32_t)flash_buf, (uint32_t*)test_buf);
					break;
				case 0x0D:
					flash_fast_erase_page(FLASH_BASE + (uint32_t)flash_buf);
					break;
				case 0x0E:
					support_print_bytes((uint8_t*)flash_buf, 256);
					break;
				case 0x0F:
					support_print_bytes((uint8_t*)flash_buf, 1024);
					break;
				case 0x10:
					printf("ACTLR: %08X\n", FLASH->ACTLR);
					printf("STATR: %08X\n", FLASH->STATR);
					printf("CTLR: %08X\n", FLASH->CTLR);
					printf("OBR: %08X\n", FLASH->OBR);
					printf("WPR: %08X\n", FLASH->WPR);
					break;
				case 0x11:
					FLASH->CTLR = (FLASH->CTLR & ~(FLASH_CTLR_PG | FLASH_CTLR_PER | FLASH_CTLR_MER | FLASH_CTLR_OPTPG | FLASH_CTLR_OPTER | FLASH_CTLR_PAGE_PG | FLASH_CTLR_PAGE_ER | FLASH_CTLR_PAGE_BER32 | FLASH_CTLR_PAGE_BER64)) | FLASH_CTLR_PG;
					printf("STATR: %08X\n", FLASH->STATR);
					printf("CTLR: %08X\n", FLASH->CTLR);
					*(volatile uint16_t*)(FLASH_BASE + (uint32_t)flash_buf) = test_buf[0];
					printf("STATR: %08X\n", FLASH->STATR);
					printf("CTLR: %08X\n", FLASH->CTLR);
					while(FLASH->STATR & FLASH_STATR_BSY);
					FLASH->CTLR &= ~(FLASH_CTLR_PG | FLASH_CTLR_PER | FLASH_CTLR_MER | FLASH_CTLR_OPTPG | FLASH_CTLR_OPTER | FLASH_CTLR_PAGE_PG | FLASH_CTLR_PAGE_ER | FLASH_CTLR_PAGE_BER32 | FLASH_CTLR_PAGE_BER64);
					printf("STATR: %08X\n", FLASH->STATR);
					printf("CTLR: %08X\n", FLASH->CTLR);
					break;
				case 0x12:
					FLASH->STATR |= FLASH_STATR_EOP;
					FLASH->CTLR = (FLASH->CTLR & ~(FLASH_CTLR_PG | FLASH_CTLR_PER | FLASH_CTLR_MER | FLASH_CTLR_OPTPG | FLASH_CTLR_OPTER | FLASH_CTLR_PAGE_PG | FLASH_CTLR_PAGE_ER | FLASH_CTLR_PAGE_BER32 | FLASH_CTLR_PAGE_BER64)) | FLASH_CTLR_PG;
					printf("STATR: %08X\n", FLASH->STATR);
					printf("CTLR: %08X\n", FLASH->CTLR);
					*(volatile uint16_t*)(FLASH_BASE + (uint32_t)flash_buf) = test_buf[0];
					printf("STATR: %08X\n", FLASH->STATR);
					printf("CTLR: %08X\n", FLASH->CTLR);
					while(!(FLASH->STATR & FLASH_STATR_EOP));
					FLASH->CTLR &= ~(FLASH_CTLR_PG | FLASH_CTLR_PER | FLASH_CTLR_MER | FLASH_CTLR_OPTPG | FLASH_CTLR_OPTER | FLASH_CTLR_PAGE_PG | FLASH_CTLR_PAGE_ER | FLASH_CTLR_PAGE_BER32 | FLASH_CTLR_PAGE_BER64);
					printf("STATR: %08X\n", FLASH->STATR);
					printf("CTLR: %08X\n", FLASH->CTLR);
					break;
				case 0x13:
					support_print_bytes(FLASH_BASE + (uint8_t*)flash_buf, 256);
					break;
				case 0x14:
					support_print_bytes(FLASH_BASE + (uint8_t*)flash_buf, 1024);
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
