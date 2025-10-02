#include "ch32v20x.h"
#include "debug.h"
#include "ch32v203_afio.h"
#include "ch32v203_core.h"
#include "ch32v203_gpio.h"
#include "ch32v203_rcc.h"
#include "ch32v203_usbd_cdc.h"
#include "ch32v203_st7735s.h"

//Pins:
// ST_RST = PA0
// ST_DC = PA1
// ST_CS = PA4
// ST_SCK = PA5
// ST_MOSI = PA7
// LED0 = PA8
// TXD = PA9
// RXD = PA10
// UDM = PA11
// UDP = PA12
// LED1 = PA13
// LED2 = PA14
// LED3 = PA15

extern const uint8_t gmctrp1_tab[16];
extern const uint8_t gmctrn1_tab[16];
const uint8_t gmctrp1_alt_tab[16] = {0x12, 0x1C, 0x10, 0x18, 0x33, 0x2C, 0x25, 0x28, 0x28, 0x27, 0x2F, 0x3C, 0x00, 0x03, 0x03, 0x10};
const uint8_t gmctrn1_alt_tab[16] = {0x12, 0x1C, 0x10, 0x18, 0x2D, 0x28, 0x23, 0x28, 0x28, 0x26, 0x2F, 0x3B, 0x00, 0x03, 0x03, 0x10};

int main(void)
{
	rcc_apb2_clk_enable(RCC_AFIOEN | RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_ADC1EN | RCC_ADC2EN | RCC_TIM1EN | RCC_SPI1EN | RCC_USART1EN);
	rcc_apb1_clk_enable(RCC_TIM2EN | RCC_USBEN);
	rcc_ahb_clk_enable(RCC_DMA1EN);
	afio_pcfr1_remap(AFIO_PCFR1_SWJ_CFG_DISABLE);

	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_8 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);	//ST_RST, ST_DC, LED0, LED1, LED2, LED3
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_AFIO_PP, GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_9);	//ST_SCK, ST_MOSI, TXD
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_IN | GPIO_MODE_PULL_IN, GPIO_PIN_10);	//RXD
	gpio_set_pin(GPIOA, GPIO_PIN_0 | GPIO_PIN_10);

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
	printf("PCLK1_TIM: %u\n", rcc_compute_pclk1_tim_freq());
	printf("PCLK2: %u\n", rcc_compute_pclk2_freq());
	printf("PCLK2_TIM: %u\n", rcc_compute_pclk2_tim_freq());
	printf("ADCCLK: %u\n", rcc_compute_adcclk());

	st7735s_init();

	uint8_t datagram[2];
	uint8_t datagram_val;
	uint8_t temp;
	uint8_t image_buf[64];
	uint16_t bytes_received;
	uint16_t bytes_available;
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
					fill_color = 0x00FF0000;
					st7735s_fill_display();
					break;
				case 0x01:
					fill_color = 0x0000FF00;
					st7735s_fill_display();
					break;
				case 0x02:
					fill_color = 0x000000FF;
					st7735s_fill_display();
					break;
				case 0x03:
					fill_color = 0x00FFFFFF;
					st7735s_fill_display();
					break;
				case 0x04:
					fill_color = 0x00000000;
					st7735s_fill_display();
					break;
				case 0x05:
					st7735s_set_pixel(20, 20, 0x00FF00FF);
					break;
				case 0x06:
					st7735s_set_pixel(20, 20, 0x0000FF00);
					break;
				case 0x07:
					text_bg_color = 0x00000000;
					text_fg_color = 0x0020FF20;
					st7735s_draw_text(ST7735S_DA_ROW_START, ST7735S_DA_COL_START, "Unicorn\n");
					break;
				case 0x08:
					text_bg_color = 0x00000000;
					text_fg_color = 0x0020FF20;
					st7735s_draw_text(ST7735S_DA_ROW_START + 10, ST7735S_DA_COL_START, "Dragon\n");
					break;
				case 0x09:
					text_bg_color = 0x00000000;
					text_fg_color = 0x0020FF20;
					st7735s_draw_text(ST7735S_DA_ROW_START + 20, ST7735S_DA_COL_START, "Wolf\n");
					break;
				case 0x0A:
					text_bg_color = 0x00000000;
					text_fg_color = 0x0020FF20;
					st7735s_draw_text(ST7735S_DA_ROW_START + 30, ST7735S_DA_COL_START, "Horse\n");
					break;
				case 0x0B:
					text_bg_color = 0x00000000;
					text_fg_color = 0x0020FF20;
					st7735s_draw_text(ST7735S_DA_ROW_START + 40, ST7735S_DA_COL_START, "Lion\n");
					break;
				case 0x0C:
					text_bg_color = 0x00000000;
					text_fg_color = 0x0020FF20;
					st7735s_draw_text(ST7735S_DA_ROW_START + 50, ST7735S_DA_COL_START, "Bear\n");
					break;
				case 0x0D:
					text_bg_color = 0x00000000;
					text_fg_color = 0x0020FF20;
					st7735s_draw_text(ST7735S_DA_ROW_START + 60, ST7735S_DA_COL_START, "Lizard\n");
					break;
				case 0x0E:
					text_bg_color = 0x00000000;
					text_fg_color = 0x0020FF20;
					st7735s_draw_text(ST7735S_DA_ROW_START + 70, ST7735S_DA_COL_START + 20, "Cat\n");
					break;
				case 0x0F:
					fill_color = 0x00202020;
					st7735s_fill_rectangle(ST7735S_DA_ROW_START + 0, ST7735S_DA_COL_START, ST7735S_DA_ROW_START + 9, ST7735S_DA_COL_END);
					break;
				case 0x10:
					fill_color = 0x00202020;
					st7735s_fill_rectangle(ST7735S_DA_ROW_START + 10, ST7735S_DA_COL_START, ST7735S_DA_ROW_START + 19, ST7735S_DA_COL_END);
					break;
				case 0x11:
					fill_color = 0x00202020;
					st7735s_fill_rectangle(ST7735S_DA_ROW_START + 20, ST7735S_DA_COL_START, ST7735S_DA_ROW_START + 29, ST7735S_DA_COL_END);
					break;
				case 0x12:
					fill_color = 0x00202020;
					st7735s_fill_rectangle(ST7735S_DA_ROW_START + 30, ST7735S_DA_COL_START, ST7735S_DA_ROW_START + 39, ST7735S_DA_COL_END);
					break;
				case 0x13:
					fill_color = 0x00202020;
					st7735s_fill_rectangle(ST7735S_DA_ROW_START + 40, ST7735S_DA_COL_START, ST7735S_DA_ROW_START + 49, ST7735S_DA_COL_END);
					break;
				case 0x14:
					fill_color = 0x00202020;
					st7735s_fill_rectangle(ST7735S_DA_ROW_START + 50, ST7735S_DA_COL_START, ST7735S_DA_ROW_START + 59, ST7735S_DA_COL_END);
					break;
				case 0x15:
					fill_color = 0x00202020;
					st7735s_fill_rectangle(ST7735S_DA_ROW_START + 60, ST7735S_DA_COL_START, ST7735S_DA_ROW_START + 69, ST7735S_DA_COL_END);
					break;
				case 0x16:
					fill_color = 0x00202020;
					st7735s_fill_rectangle(ST7735S_DA_ROW_START + 70, ST7735S_DA_COL_START, ST7735S_DA_ROW_START + 79, ST7735S_DA_COL_END);
					break;
				case 0x17:
					// Set VCOM to 0x05
					st7735s_send_command(ST7735S_COM_VMCTR1);
					st7735s_send_data(0x05);
					break;
				case 0x18:
					// Set VCOM to 0x0A
					st7735s_send_command(ST7735S_COM_VMCTR1);
					st7735s_send_data(0x0A);
					break;
				case 0x19:
					// Set VCOM to 0x0C
					st7735s_send_command(ST7735S_COM_VMCTR1);
					st7735s_send_data(0x0C);
					break;
				case 0x1A:
					// Set VCOM to 0x0E
					st7735s_send_command(ST7735S_COM_VMCTR1);
					st7735s_send_data(0x0E);
					break;
				case 0x1B:
					// Set VCOM to 0x12
					st7735s_send_command(ST7735S_COM_VMCTR1);
					st7735s_send_data(0x12);
					break;
				case 0x1C:
					// Set VCOM to 0x18
					st7735s_send_command(ST7735S_COM_VMCTR1);
					st7735s_send_data(0x18);
					break;
				case 0x1D:
					// Set VCOM to 0x20
					st7735s_send_command(ST7735S_COM_VMCTR1);
					st7735s_send_data(0x20);
					break;
				case 0x1E:
					// set scroll area
					st7735s_send_command(ST7735S_COM_SCRLAR);
					st7735s_send_data(0x00);
					st7735s_send_data(0x00);
					st7735s_send_data(0);
					st7735s_send_data(162);
					st7735s_send_data(0x00);
					st7735s_send_data(0x00);
					break;
				case 0x1F:
					// scroll to 50
					st7735s_send_command(ST7735S_COM_VSCSAD);
					st7735s_send_data(0x00);
					st7735s_send_data(50);
					break;
				case 0x20:
					// scroll to 0
					st7735s_send_command(ST7735S_COM_VSCSAD);
					st7735s_send_data(0x00);
					st7735s_send_data(0x00);
					break;
				case 0x21:
					// select gamma curve 1
					st7735s_send_command(ST7735S_COM_GAMSET);
					st7735s_send_data(0x01);
					break;
				case 0x22:
					// select gamma curve 2
					st7735s_send_command(ST7735S_COM_GAMSET);
					st7735s_send_data(0x02);
					break;
				case 0x23:
					// select gamma curve 3
					st7735s_send_command(ST7735S_COM_GAMSET);
					st7735s_send_data(0x04);
					break;
				case 0x24:
					// select gamma curve 4
					st7735s_send_command(ST7735S_COM_GAMSET);
					st7735s_send_data(0x08);
					break;
				case 0x25:
					// set default gamma correction table
					st7735s_send_command(ST7735S_COM_GMCTRP1);
					for(uint8_t d = 0; d < 16; ++d)
					{
						st7735s_send_data(gmctrp1_tab[d]);
					}
					st7735s_send_command(ST7735S_COM_GMCTRN1);
					for(uint8_t d = 0; d < 16; ++d)
					{
						st7735s_send_data(gmctrn1_tab[d]);
					}
					break;
				case 0x26:
					// set alternate gamma correction table
					st7735s_send_command(ST7735S_COM_GMCTRP1);
					for(uint8_t d = 0; d < 16; ++d)
					{
						st7735s_send_data(gmctrp1_alt_tab[d]);
					}
					st7735s_send_command(ST7735S_COM_GMCTRN1);
					for(uint8_t d = 0; d < 16; ++d)
					{
						st7735s_send_data(gmctrn1_alt_tab[d]);
					}
					break;
				case 0x27:
					// display off
					st7735s_send_command(ST7735S_COM_DISPOFF);
					break;
				case 0x28:
					// display on
					st7735s_send_command(ST7735S_COM_DISPON);
					break;
				case 0x29:
					// inversion off
					st7735s_send_command(ST7735S_COM_INVOFF);
					break;
				case 0x30:
					// inversion on
					st7735s_send_command(ST7735S_COM_INVON);
					break;
				case 0x31:
					// receive raw image data
					bytes_received = 0;

					st7735s_send_command(ST7735S_COM_CASET);
					st7735s_send_data(0x00);
					st7735s_send_data(ST7735S_DA_COL_START);
					st7735s_send_data(0x00);
					st7735s_send_data(ST7735S_DA_COL_END);
					st7735s_send_command(ST7735S_COM_RASET);
					st7735s_send_data(0x00);
					st7735s_send_data(ST7735S_DA_ROW_START);
					st7735s_send_data(0x00);
					st7735s_send_data(ST7735S_DA_ROW_END);

					st7735s_send_command(ST7735S_COM_RAMWR);
					while(bytes_received < (ST7735S_NUM_PIXELS * 3))
					{
						while(!(bytes_available = cdc_bytes_available()));
						if(bytes_available > 64)
							bytes_available = 64;
						cdc_read_bytes(image_buf, bytes_available);
						bytes_received += bytes_available;
						for(uint16_t d = 0; d < bytes_available; ++d)
						{
							st7735s_send_data(image_buf[d]);
						}
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
