#include "ch32v30x.h"
#include "ch32v307_core.h"
#include "ch32v307_dma.h"
#include "ch32v307_gpio.h"
#include "ch32v307_rcc.h"
#include "debug.h"
#include "fifo.h"
#include "ch32v307_uart.h"
#include "pseudo_random.h"

//Pins:
// TXD = PA9
// RXD = PA10
// OTG_DM = PA11
// OTG_DP = PA12
// LED0 = PC0
// LED1 = PC1
// LED2 = PC2
// LED3 = PC3

volatile uint32_t test_buf_1[64];
volatile uint32_t test_buf_2[64];

void on_dma_transfer_complete(void)
{
	printf("DMA transfer completed.\n");
}

static const char hex_table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};

void uart_print_bytes(uint8_t* src, uint16_t num_bytes)
{
	uint8_t count = 0;
	uint8_t value;
	while(num_bytes)
	{
		value = *src;
		while(uart_bytes_available_for_write(uart1_tx_fifo) < 3);
		uart_write_byte(USART1, uart1_tx_fifo, hex_table[(value >> 4) & 0x0f]);
		uart_write_byte(USART1, uart1_tx_fifo, hex_table[(value) & 0x0f]);
		--num_bytes;
		++count;
		++src;
		if(!(count & 0x0F))
		{
			uart_write_byte(USART1, uart1_tx_fifo, '\n');
		}
	}
}

int main(void)
{
	rcc_apb2_clk_enable(RCC_AFIOEN | RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_TIM1EN | RCC_SPI1EN | RCC_USART1EN);
	rcc_ahb_clk_enable(RCC_DMA1EN | RCC_DMA2EN);

	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_AFIO_PP, GPIO_PIN_9);	//TXD
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_IN | GPIO_MODE_FLOAT_IN, GPIO_PIN_10);		//RXD
	gpio_set_mode(GPIOC, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

	core_delay_init();
	uart_init(USART1, 115200);
	core_enable_irq(USART1_IRQn);

	printf("SYSCLK: %u\n", rcc_compute_sysclk_freq());
	printf("HCLK: %u\n", rcc_compute_hclk_freq());
	printf("PCLK1: %u\n", rcc_compute_pclk1_freq());
	printf("PCLK2: %u\n", rcc_compute_pclk2_freq());
	printf("ADCCLK: %u\n", rcc_compute_adcclk());

	gpio_set_pin(GPIOC, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
	core_delay_ms(100);
	gpio_clear_pin(GPIOC, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
	core_delay_ms(100);
	gpio_write_pin(GPIOC, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 1);
	core_delay_ms(100);
	printf("Unicorn\n");

	dma_init();
	dma2_channel11_callback = on_dma_transfer_complete;

	uint8_t datagram[2];
	uint8_t datagram_val;
	uint8_t temp;
	while(1)
	{
		if(uart_bytes_available(uart1_rx_fifo) >= 2)
		{
			temp = uart_peek(uart1_rx_fifo);
			if((temp == '\r') || (temp == '\n'))
			{
				(void)uart_read_byte(USART1, uart1_rx_fifo);
				continue;
			}

			uart_read_bytes(USART1, uart1_rx_fifo, datagram, 2);
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

			gpio_toggle_pin(GPIOC, GPIO_PIN_0);
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
					uart_print_bytes((uint8_t*)test_buf_1, 256);
					break;
				case 0x03:
					uart_print_bytes((uint8_t*)test_buf_2, 256);
					break;
				case 0x04:
					dma_channel_config(DMA2_Channel11, DMA_CFG_MEM2MEM_ON | DMA_CFG_PRI_MEDIUM | DMA_CFG_MSIZE_32 | DMA_CFG_PSIZE_32 | DMA_CFG_MINC_ON | DMA_CFG_PINC_ON | DMA_CFG_MEM_TO_PERIPH | DMA_CFG_TCIE_ON);
					break;
				case 0x05:
					dma_channel_config(DMA2_Channel11, DMA_CFG_MEM2MEM_ON | DMA_CFG_PRI_MEDIUM | DMA_CFG_MSIZE_32 | DMA_CFG_PSIZE_32 | DMA_CFG_MINC_ON | DMA_CFG_PINC_ON | DMA_CFG_PERIPH_TO_MEM | DMA_CFG_TCIE_ON);
					break;
				case 0x06:
					dma_transfer(DMA2_Channel11, (uint32_t)test_buf_1, (uint32_t)test_buf_2, 64);
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
			gpio_write_pin(GPIOC, GPIO_PIN_1, gpio_read_pin(GPIOC, GPIO_PIN_0));
		}
	}
}
