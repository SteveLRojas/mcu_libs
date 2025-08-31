#include "ch32v20x.h"
#include "ch32v203_afio.h"
#include "ch32v203_core.h"
#include "ch32v203_dma.h"
#include "ch32v203_gpio.h"
#include "ch32v203_rcc.h"
#include "ch32v203_uart_dma.h"
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
	uart_dma_init(USART1, 115200);

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
	while(!cdc_config);	//Wait for host to configure the CDC interface
	printf("Unicorn\n");

	printf("SYSCLK: %lu\n", rcc_compute_sysclk_freq());
	printf("HCLK: %lu\n", rcc_compute_hclk_freq());
	printf("PCLK1: %lu\n", rcc_compute_pclk1_freq());
	printf("PCLK1_TIM: %lu\n", rcc_compute_pclk1_tim_freq());
	printf("PCLK2: %lu\n", rcc_compute_pclk2_freq());
	printf("PCLK2_TIM: %lu\n", rcc_compute_pclk2_tim_freq());
	printf("ADCCLK: %lu\n", rcc_compute_adcclk());

	uint8_t datagram[2];
	uint8_t datagram_val;
	uint8_t temp;
	uint8_t test_buf[1024];
	uint16_t num_bytes;
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
				case 0x00:	//read uart_bytes_available
					printf("uart_bytes_available: %u\n", uart_dma_bytes_available(uart_dma_1));
					break;
				case 0x01:	//uart_peek
					printf("uart_peek: %02X\n", uart_dma_peek(uart_dma_1));
					break;
				case 0x02:	//uart_read_byte
					printf("uart_read_byte: %02X\n", uart_dma_read_byte(uart_dma_1));
					break;
				case 0x03:	//uart_read_bytes (available)
					num_bytes = uart_dma_bytes_available(uart_dma_1);
					uart_dma_read_bytes(uart_dma_1, test_buf, num_bytes);
					cdc_print_bytes(test_buf, num_bytes);
					break;
				case 0x04:	//uart_read_bytes (10)
					uart_dma_read_bytes(uart_dma_1, test_buf, 10);
					cdc_print_bytes(test_buf, 10);
					break;
				case 0x05:	//uart_read_bytes_until
					num_bytes = uart_dma_read_bytes_until(uart_dma_1, '\n', test_buf, 1024);
					test_buf[num_bytes] = 0x00;
					printf("uart_read_bytes_until: %s\n", test_buf);
					break;
				case 0x06:	//uart_get_string
					uart_dma_get_string(uart_dma_1, (char*)test_buf, 1024);
					printf("uart_get_string: %s\n", test_buf);
					break;
				case 0x07:	//uart_bytes_available_for_write
					printf("uart_bytes_available_for_write: %u\n", uart_dma_bytes_available_for_write(uart_dma_1));
					break;
				case 0x08:	//uart_write_byte
					uart_dma_write_byte(uart_dma_1, test_buf[0]);
					break;
				case 0x09:	//uart_write_string (hard coded)
					uart_dma_write_string(uart_dma_1, "Magical pony\n");
					break;
				case 0x0A:	//uart_write_string (from buffer)
					test_buf[127] = 0x00;
					uart_dma_write_string(uart_dma_1, (char*)test_buf);
					break;
				case 0x0B:	//uart_write_bytes (10)
					uart_dma_write_bytes(uart_dma_1, test_buf, 10);
					break;
				case 0x0C:	//uart_write_bytes (512)
					uart_dma_write_bytes(uart_dma_1, test_buf, 512);
					break;
				case 0x0D:	//uart_write_bytes (1024)
					uart_dma_write_bytes(uart_dma_1, test_buf, 1024);
					break;
				case 0x0E:	//initialize test buffer
					for(uint16_t d = 0; d < 1024; d += 1)
					{
						test_buf[d] = (d >> 6) + (((d >> 6) < 10) ? '0' : 'A' - 10);
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
