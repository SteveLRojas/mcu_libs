#include "ch32v20x.h"
#include "ch32v203_dma.h"
#include "ch32v203_rcc.h"
#include "ch32v203_uart_dma.h"

#if USE_UART_DMA_1
uint8_t uart_dma_1_rx_buf[UART_DMA_1_RX_BUF_SIZE];
uint8_t uart_dma_1_tx_buf[UART_DMA_1_TX_BUF_SIZE];
uart_dma_t uart_dma_1_i;
#endif

#if USE_UART_DMA_2
uint8_t uart_dma_2_rx_buf[UART_DMA_2_RX_BUF_SIZE];
uint8_t uart_dma_2_tx_buf[UART_DMA_2_TX_BUF_SIZE];
uart_dma_t uart_dma_2_i;
#endif

#if USE_UART_DMA_3
uint8_t uart_dma_3_rx_buf[UART_DMA_3_RX_BUF_SIZE];
uint8_t uart_dma_3_tx_buf[UART_DMA_3_TX_BUF_SIZE];
uart_dma_t uart_dma_3_i;
#endif

#if USE_UART_DMA_4
uint8_t uart_dma_4_rx_buf[UART_DMA_4_RX_BUF_SIZE];
uint8_t uart_dma_4_tx_buf[UART_DMA_4_TX_BUF_SIZE];
uart_dma_t uart_dma_4_i;
#endif

#if USE_UART_DMA_1
void on_uart_dma_1_tx(void)
{
	uint16_t num_bytes_to_send;
	uint16_t read_idx;
	uint16_t to_wrap;

	dma_disable(uart_dma_1_i.tx_dma);
	//Update read address
	read_idx = (uint16_t)(uart_dma_1_i.tx_dma->MADDR + uart_dma_1_i.tx_transfer_size - (uint32_t)(uart_dma_1_i.tx_buf));
	read_idx &= (uart_dma_1_i.tx_buf_size - 1);
	uart_dma_1_i.tx_dma->MADDR = (uint32_t)(uart_dma_1_i.tx_buf) + read_idx;	//update MADDR and make sure it wraps around when needed.
	//Compute number of bytes available to send
	num_bytes_to_send = uart_dma_1_i.tx_write_idx - read_idx;
	num_bytes_to_send &= (uart_dma_1_i.tx_buf_size - 1);	//expression is valid even if tx_write_idx or read_idx are not yet wrapped around

	if(num_bytes_to_send)
	{
		to_wrap = uart_dma_1_i.tx_buf_size - read_idx;
		if(num_bytes_to_send > to_wrap)
			num_bytes_to_send = to_wrap;
		uart_dma_1_i.tx_transfer_size = num_bytes_to_send;
		dma_set_num_words(uart_dma_1_i.tx_dma, num_bytes_to_send);
		dma_enable(uart_dma_1_i.tx_dma);
	}
}
#endif

#if USE_UART_DMA_2
void on_uart_dma_2_tx(void)
{
	uint16_t num_bytes_to_send;
	uint16_t read_idx;
	uint16_t to_wrap;

	dma_disable(uart_dma_2_i.tx_dma);
	//Update read address
	read_idx = (uint16_t)(uart_dma_2_i.tx_dma->MADDR + uart_dma_2_i.tx_transfer_size - (uint32_t)(uart_dma_2_i.tx_buf));
	read_idx &= (uart_dma_2_i.tx_buf_size - 1);
	uart_dma_2_i.tx_dma->MADDR = (uint32_t)(uart_dma_2_i.tx_buf) + read_idx;	//update MADDR and make sure it wraps around when needed.
	//Compute number of bytes available to send
	num_bytes_to_send = uart_dma_2_i.tx_write_idx - read_idx;
	num_bytes_to_send &= (uart_dma_2_i.tx_buf_size - 1);	//expression is valid even if tx_write_idx or read_idx are not yet wrapped around

	if(num_bytes_to_send)
	{
		to_wrap = uart_dma_2_i.tx_buf_size - read_idx;
		if(num_bytes_to_send > to_wrap)
			num_bytes_to_send = to_wrap;
		dma_set_num_words(uart_dma_2_i.tx_dma, num_bytes_to_send);
		dma_enable(uart_dma_2_i.tx_dma);
	}
}
#endif

#if USE_UART_DMA_3
void on_uart_dma_3_tx(void)
{
	uint16_t num_bytes_to_send;
	uint16_t read_idx;
	uint16_t to_wrap;

	dma_disable(uart_dma_3_i.tx_dma);
	//Update read address
	read_idx = (uint16_t)(uart_dma_3_i.tx_dma->MADDR + uart_dma_3_i.tx_transfer_size - (uint32_t)(uart_dma_3_i.tx_buf));
	read_idx &= (uart_dma_3_i.tx_buf_size - 1);
	uart_dma_3_i.tx_dma->MADDR = (uint32_t)(uart_dma_3_i.tx_buf) + read_idx;	//update MADDR and make sure it wraps around when needed.
	//Compute number of bytes available to send
	num_bytes_to_send = uart_dma_3_i.tx_write_idx - read_idx;
	num_bytes_to_send &= (uart_dma_3_i.tx_buf_size - 1);	//expression is valid even if tx_write_idx or read_idx are not yet wrapped around

	if(num_bytes_to_send)
	{
		to_wrap = uart_dma_3_i.tx_buf_size - read_idx;
		if(num_bytes_to_send > to_wrap)
			num_bytes_to_send = to_wrap;
		dma_set_num_words(uart_dma_3_i.tx_dma, num_bytes_to_send);
		dma_enable(uart_dma_3_i.tx_dma);
	}
}
#endif

#if USE_UART_DMA_4
void on_uart_dma_4_tx(void)
{
	uint16_t num_bytes_to_send;
	uint16_t read_idx;
	uint16_t to_wrap;

	dma_disable(uart_dma_4_i.tx_dma);
	//Update read address
	read_idx = (uint16_t)(uart_dma_4_i.tx_dma->MADDR + uart_dma_4_i.tx_transfer_size - (uint32_t)(uart_dma_4_i.tx_buf));
	read_idx &= (uart_dma_4_i.tx_buf_size - 1);
	uart_dma_4_i.tx_dma->MADDR = (uint32_t)(uart_dma_4_i.tx_buf) + read_idx;	//update MADDR and make sure it wraps around when needed.
	//Compute number of bytes available to send
	num_bytes_to_send = uart_dma_4_i.tx_write_idx - read_idx;
	num_bytes_to_send &= (uart_dma_4_i.tx_buf_size - 1);	//expression is valid even if tx_write_idx or read_idx are not yet wrapped around

	if(num_bytes_to_send)
	{
		to_wrap = uart_dma_4_i.tx_buf_size - read_idx;
		if(num_bytes_to_send > to_wrap)
			num_bytes_to_send = to_wrap;
		dma_set_num_words(uart_dma_4_i.tx_dma, num_bytes_to_send);
		dma_enable(uart_dma_4_i.tx_dma);
	}
}
#endif

// HINT: Be sure to call dma_init() first! No need to enable UART interrupts.
// This function configures the UART and initializes the corresponding uart_dma struct used in other functions.
/*
 * USART1_RX -> DMA1_CHAN5
 * USART1_TX -> DMA1_CHAN4
 * USART2_RX -> DMA1_CHAN6
 * USART2_TX -> DMA1_CHAN7
 * USART3_RX -> DMA1_CHAN3
 * USART3_TX -> DMA1_CHAN2
 * UART4_RX -> DMA1_CHAN8
 * UART4_TX -> DMA1_CHAN1
 */
void uart_dma_init(USART_TypeDef* uart, uint32_t baud)
{
	uint32_t apbclock;
	uint32_t usart_div;

	if(baud)
	{
		switch((uint32_t)uart)
		{
#if USE_UART_DMA_1
		case (uint32_t)USART1:
			uart_dma_1_i.rx_dma = DMA1_Channel5;
			uart_dma_1_i.rx_buf_size = UART_DMA_1_RX_BUF_SIZE;
			uart_dma_1_i.rx_buf = uart_dma_1_rx_buf;
			uart_dma_1_i.rx_read_idx = 0;
			uart_dma_1_i.tx_dma = DMA1_Channel4;
			uart_dma_1_i.tx_buf_size = UART_DMA_1_TX_BUF_SIZE;
			uart_dma_1_i.tx_buf = uart_dma_1_tx_buf;
			uart_dma_1_i.tx_write_idx = 0;
			dma_channel_config(DMA1_Channel5, DMA_CFG_MEM2MEM_OFF | DMA_CFG_PRI_MAX | DMA_CFG_MSIZE_8 | DMA_CFG_PSIZE_8 | DMA_CFG_MINC_ON | DMA_CFG_PINC_OFF | DMA_CFG_CIRC_ON | DMA_CFG_PERIPH_TO_MEM | DMA_CFG_TCIE_OFF);
			dma_transfer(DMA1_Channel5, (uint32_t)uart_dma_1_rx_buf, (uint32_t)(&USART1->DATAR), UART_DMA_1_RX_BUF_SIZE);
			dma_channel_config(DMA1_Channel4, DMA_CFG_MEM2MEM_OFF | DMA_CFG_PRI_MAX | DMA_CFG_MSIZE_8 | DMA_CFG_PSIZE_8 | DMA_CFG_MINC_ON | DMA_CFG_PINC_OFF | DMA_CFG_CIRC_OFF | DMA_CFG_MEM_TO_PERIPH | DMA_CFG_TCIE_ON);
			dma_set_mem_addr(DMA1_Channel4, uart_dma_1_tx_buf);
			dma_set_periph_addr(DMA1_Channel4, (uint32_t)(&USART1->DATAR));
			dma1_channel4_callback = on_uart_dma_1_tx;
			break;
#endif
#if USE_UART_DMA_2
		case (uint32_t)USART2:
			uart_dma_2_i.rx_dma = DMA1_Channel6;
			uart_dma_2_i.rx_buf_size = UART_DMA_2_RX_BUF_SIZE;
			uart_dma_2_i.rx_buf = uart_dma_2_rx_buf;
			uart_dma_2_i.rx_read_idx = 0;
			uart_dma_2_i.tx_dma = DMA1_Channel7;
			uart_dma_2_i.tx_buf_size = UART_DMA_2_TX_BUF_SIZE;
			uart_dma_2_i.tx_buf = uart_dma_2_tx_buf;
			uart_dma_2_i.tx_write_idx = 0;
			dma_channel_config(DMA1_Channel6, DMA_CFG_MEM2MEM_OFF | DMA_CFG_PRI_MAX | DMA_CFG_MSIZE_8 | DMA_CFG_PSIZE_8 | DMA_CFG_MINC_ON | DMA_CFG_PINC_OFF | DMA_CFG_CIRC_ON | DMA_CFG_PERIPH_TO_MEM | DMA_CFG_TCIE_OFF);
			dma_transfer(DMA1_Channel6, (uint32_t)uart_dma_2_rx_buf, (uint32_t)(&USART2->DATAR), UART_DMA_2_RX_BUF_SIZE);
			dma_channel_config(DMA1_Channel7, DMA_CFG_MEM2MEM_OFF | DMA_CFG_PRI_MAX | DMA_CFG_MSIZE_8 | DMA_CFG_PSIZE_8 | DMA_CFG_MINC_ON | DMA_CFG_PINC_OFF | DMA_CFG_CIRC_OFF | DMA_CFG_MEM_TO_PERIPH | DMA_CFG_TCIE_ON);
			dma_set_mem_addr(DMA1_Channel7, uart_dma_2_tx_buf);
			dma_set_periph_addr(DMA1_Channel7, (uint32_t)(&USART2->DATAR));
			dma1_channel7_callback = on_uart_dma_2_tx;
			break;
#endif
#if USE_UART_DMA_3
		case (uint32_t)USART3:
			uart_dma_3_i.rx_dma = DMA1_Channel3;
			uart_dma_3_i.rx_buf_size = UART_DMA_3_RX_BUF_SIZE;
			uart_dma_3_i.rx_buf = uart_dma_3_rx_buf;
			uart_dma_3_i.rx_read_idx = 0;
			uart_dma_3_i.tx_dma = DMA1_Channel2;
			uart_dma_3_i.tx_buf_size = UART_DMA_3_TX_BUF_SIZE;
			uart_dma_3_i.tx_buf = uart_dma_3_tx_buf;
			uart_dma_3_i.tx_write_idx = 0;
			dma_channel_config(DMA1_Channel3, DMA_CFG_MEM2MEM_OFF | DMA_CFG_PRI_MAX | DMA_CFG_MSIZE_8 | DMA_CFG_PSIZE_8 | DMA_CFG_MINC_ON | DMA_CFG_PINC_OFF | DMA_CFG_CIRC_ON | DMA_CFG_PERIPH_TO_MEM | DMA_CFG_TCIE_OFF);
			dma_transfer(DMA1_Channel3, (uint32_t)uart_dma_3_rx_buf, (uint32_t)(&USART3->DATAR), UART_DMA_3_RX_BUF_SIZE);
			dma_channel_config(DMA1_Channel2, DMA_CFG_MEM2MEM_OFF | DMA_CFG_PRI_MAX | DMA_CFG_MSIZE_8 | DMA_CFG_PSIZE_8 | DMA_CFG_MINC_ON | DMA_CFG_PINC_OFF | DMA_CFG_CIRC_OFF | DMA_CFG_MEM_TO_PERIPH | DMA_CFG_TCIE_ON);
			dma_set_mem_addr(DMA1_Channel2, uart_dma_3_tx_buf);
			dma_set_periph_addr(DMA1_Channel2, (uint32_t)(&USART3->DATAR));
			dma1_channel2_callback = on_uart_dma_3_tx;
			break;
#endif
#if USE_UART_DMA_4
		case (uint32_t)UART4:
			uart_dma_4_i.rx_dma = DMA1_Channel8;
			uart_dma_4_i.rx_buf_size = UART_DMA_4_RX_BUF_SIZE;
			uart_dma_4_i.rx_buf = uart_dma_4_rx_buf;
			uart_dma_4_i.rx_read_idx = 0;
			uart_dma_4_i.tx_dma = DMA1_Channel1;
			uart_dma_4_i.tx_buf_size = UART_DMA_4_TX_BUF_SIZE;
			uart_dma_4_i.tx_buf = uart_dma_4_tx_buf;
			uart_dma_4_i.tx_write_idx = 0;
			dma_channel_config(DMA1_Channel8, DMA_CFG_MEM2MEM_OFF | DMA_CFG_PRI_MAX | DMA_CFG_MSIZE_8 | DMA_CFG_PSIZE_8 | DMA_CFG_MINC_ON | DMA_CFG_PINC_OFF | DMA_CFG_CIRC_ON | DMA_CFG_PERIPH_TO_MEM | DMA_CFG_TCIE_OFF);
			dma_transfer(DMA1_Channel8, (uint32_t)uart_dma_4_rx_buf, (uint32_t)(&UART4->DATAR), UART_DMA_4_RX_BUF_SIZE);
			dma_channel_config(DMA1_Channel1, DMA_CFG_MEM2MEM_OFF | DMA_CFG_PRI_MAX | DMA_CFG_MSIZE_8 | DMA_CFG_PSIZE_8 | DMA_CFG_MINC_ON | DMA_CFG_PINC_OFF | DMA_CFG_CIRC_OFF | DMA_CFG_MEM_TO_PERIPH | DMA_CFG_TCIE_ON);
			dma_set_mem_addr(DMA1_Channel1, uart_dma_4_tx_buf);
			dma_set_periph_addr(DMA1_Channel1, (uint32_t)(&UART4->DATAR));
			dma1_channel1_callback = on_uart_dma_4_tx;
			break;
#endif
		default: return;
		}

		apbclock = (uart == USART1) ? rcc_compute_pclk2_freq() : rcc_compute_pclk1_freq(); // USART1 is on APB2, others are on APB1
		usart_div = ((apbclock << 1) / baud);
		usart_div = usart_div + 1;
		usart_div = usart_div >> 1;

		uart->BRR = (uint16_t)usart_div;
		uart->CTLR3 = USART_CTLR3_DMAT | USART_CTLR3_DMAR;	//CTS disabled, RTS disabled, smartcard disabled, infrared disabled, DMA enabled
		uart->CTLR2 = 0x0000;	//LIN mode disabled, 1 stop bit
		uart->STATR = 0;
		uart->CTLR1 = USART_CTLR1_RE | USART_CTLR1_TE | USART_CTLR1_UE;
	}
	else
	{
		// Turn off UART interrupts, transmitter off, receiver off
		uart->CTLR1 = 0x0000;
	}
}

// Receive
//HINT: this returns 0 when the buffer is completely full... Do not let it become completely full!
/*uint16_t uart_dma_bytes_available(uart_dma_t* uart_dma)
{
	return (uart_dma->rx_buf_size - uart_dma->rx_dma->CNTR - uart_dma->rx_read_idx) & (uart_dma->rx_buf_size - 1);
}*/

uint8_t uart_dma_peek(uart_dma_t* uart_dma)
{
	return uart_dma->rx_buf[uart_dma->rx_read_idx];
}

//HINT: this function blocks if a byte is not available already
uint8_t uart_dma_read_byte(uart_dma_t* uart_dma)
{
	uint8_t temp;

	while(!uart_dma_bytes_available(uart_dma));

	temp = uart_dma->rx_buf[uart_dma->rx_read_idx];
	uart_dma->rx_read_idx += 1;
	uart_dma->rx_read_idx &= (uart_dma->rx_buf_size - 1);

	return temp;
}

//HINT: This function blocks if the requested bytes are not already available
void uart_dma_read_bytes(uart_dma_t* uart_dma, uint8_t* dest, uint16_t num_bytes)
{
	uint16_t num_to_read;
	uint8_t* read_ptr;
	uint16_t to_wrap;

	while(num_bytes)
	{
		//Determine the number of bytes that can be read
		num_to_read = uart_dma_bytes_available(uart_dma);
		if(num_to_read > num_bytes)
		{
			num_to_read = num_bytes;
		}

		num_bytes -= num_to_read;

		//Read the bytes
		to_wrap = uart_dma->rx_buf_size - uart_dma->rx_read_idx;
		if(num_to_read >= to_wrap)
		{
			//Read until wrap around
			read_ptr = uart_dma->rx_buf + uart_dma->rx_read_idx;
			for(uint16_t idx = 0; idx < to_wrap; idx += 1)
			{
				*dest = read_ptr[idx];
				++dest;
			}

			uart_dma->rx_read_idx = 0;
			num_to_read -= to_wrap;
		}

		//Read remaining, no wrap around
		read_ptr = uart_dma->rx_buf + uart_dma->rx_read_idx;
		for(uint16_t idx = 0; idx < num_to_read; idx += 1)
		{
			*dest = read_ptr[idx];
			++dest;
		}

		uart_dma->rx_read_idx += num_to_read;
	}
}

//HINT: This function blocks if the requested bytes are not already available
uint16_t uart_dma_read_bytes_until(uart_dma_t* uart_dma, uint8_t terminator, uint8_t* dest, uint16_t num_bytes)
{
	uint16_t num_remaining = num_bytes;
	uint8_t popped;
	uint16_t num_to_read;
	uint8_t* read_ptr;
	uint16_t to_wrap;

	while(num_remaining)
	{
		//Determine the number of bytes that can be read
		num_to_read = uart_dma_bytes_available(uart_dma);
		if(num_to_read > num_remaining)
		{
			num_to_read = num_remaining;
		}

		//Read the bytes
		to_wrap = uart_dma->rx_buf_size - uart_dma->rx_read_idx;
		if(num_to_read >= to_wrap)
		{
			//Read until wrap around
			read_ptr = uart_dma->rx_buf + uart_dma->rx_read_idx;
			for(uint16_t idx = 0; idx < to_wrap; idx += 1)
			{
				popped = read_ptr[idx];
				if(popped == terminator)
				{
					uart_dma->rx_read_idx += idx + 1;
					num_remaining -= idx;
					return num_bytes - num_remaining;
				}
				*dest = popped;
				dest += 1;
			}

			uart_dma->rx_read_idx = 0;
			num_to_read -= to_wrap;
			num_remaining -= to_wrap;
		}

		//Read remaining, no wrap around
		read_ptr = uart_dma->rx_buf + uart_dma->rx_read_idx;
		for(uint16_t idx = 0; idx < num_to_read; idx += 1)
		{
			popped = read_ptr[idx];
			if(popped == terminator)
			{
				uart_dma->rx_read_idx += idx + 1;
				num_remaining -= idx;
				return num_bytes - num_remaining;
			}
			*dest = popped;
			dest += 1;
		}

		uart_dma->rx_read_idx += num_to_read;
		num_remaining -= num_to_read;
	}

	return num_bytes;
}

uint16_t uart_dma_get_string(uart_dma_t* uart_dma, char* dest, uint16_t buf_size)
{
	uint16_t num_remaining;
	char received_data;
	uint16_t num_to_read;
	uint8_t* read_ptr;
	uint16_t to_wrap;
	uint16_t idx;

	buf_size -= 1;	//leave space for the null byte
	num_remaining = buf_size;

	while(num_remaining)
	{
		//Determine the number of bytes that can be read
		num_to_read = uart_dma_bytes_available(uart_dma);
		if(num_to_read > num_remaining)
		{
			num_to_read = num_remaining;
		}
		to_wrap = uart_dma->rx_buf_size - uart_dma->rx_read_idx;
		if(num_to_read > to_wrap)
		{
			num_to_read = to_wrap;
		}

		read_ptr = uart_dma->rx_buf + uart_dma->rx_read_idx;
		idx = 0;
		while(idx < num_to_read)
		{
			received_data = read_ptr[idx++];
			*dest = received_data;
			dest += 1;
			if(received_data == '\r' || received_data == '\n')
			{
				uart_dma->rx_read_idx += idx;
				num_remaining -= idx;
				*dest = '\0';
				return buf_size - num_remaining;
			}
		}

		uart_dma->rx_read_idx += num_to_read;
		uart_dma->rx_read_idx &= (uart_dma->rx_buf_size - 1);
		num_remaining -= num_to_read;
	}

	*dest = '\0';
	return buf_size;
}

// Send
//HINT: This function returns buf_size - 1 when the buffer is completely empty, that is the maximum amount that can be safely written.
/*uint16_t uart_dma_bytes_available_for_write(uart_dma_t* uart_dma)
{
	//return ~(write_idx - (buf_size - tx_dma->CNTR)) & (buf_size - 1);
	return ((uart_dma->tx_dma->MADDR - (uint32_t)(uart_dma->tx_buf)) + ~uart_dma->tx_write_idx) & (uart_dma->tx_buf_size - 1);
}*/

//HINT: This function has so much overhead it should probably never be used...
void uart_dma_write_byte(uart_dma_t* uart_dma, uint8_t val)
{
	uart_dma->tx_buf[uart_dma->tx_write_idx] = val;
	uart_dma->tx_write_idx += 1;
	uart_dma->tx_write_idx &= (uart_dma->tx_buf_size - 1);

	if(!dma_get_pending_transfers(uart_dma->tx_dma) && (uart_dma->tx_write_idx != (uint16_t)(uart_dma->tx_dma->MADDR - (uint32_t)(uart_dma->tx_buf))))
	{
		uart_dma->tx_transfer_size = 1;
		dma_set_num_words(uart_dma->tx_dma, 1);
		dma_enable(uart_dma->tx_dma);
	}
}

uint16_t uart_dma_write_string(uart_dma_t* uart_dma, char* str)
{
	uint16_t len = 0;

	while(str[len])
	{
		++len;
	}

	uart_dma_write_bytes(uart_dma, (uint8_t*)str, len);
	return len;
}

void uart_dma_write_bytes(uart_dma_t* uart_dma, uint8_t* src, uint16_t num_bytes)
{
	uint16_t num_to_write;
	uint8_t* write_ptr;
	uint16_t to_wrap;
	uint16_t read_idx;

	while(num_bytes)
	{
		//Determine the number of bytes that can be written
		num_to_write = uart_dma_bytes_available_for_write(uart_dma);
		if(num_to_write > num_bytes)
		{
			num_to_write = num_bytes;
		}

		num_bytes -= num_to_write;

		//Write the bytes
		to_wrap = uart_dma->tx_buf_size - uart_dma->tx_write_idx;
		if(num_to_write >= to_wrap)
		{
			//Write until wrap around
			write_ptr = uart_dma->tx_buf + uart_dma->tx_write_idx;
			for(uint16_t idx = 0; idx < to_wrap; idx += 1)
			{
				write_ptr[idx] = *src;
				++src;
			}

			uart_dma->tx_write_idx = 0;
			num_to_write -= to_wrap;
		}

		//write remaining - no wrap around
		write_ptr = uart_dma->tx_buf + uart_dma->tx_write_idx;
		for(uint16_t idx = 0; idx < num_to_write; idx += 1)
		{
			write_ptr[idx] = *src;
			++src;
		}

		uart_dma->tx_write_idx += num_to_write;

		if(!dma_get_pending_transfers(uart_dma->tx_dma))
		{
			//Compute number of bytes available to send
			read_idx = (uint16_t)(uart_dma->tx_dma->MADDR - (uint32_t)(uart_dma->tx_buf));
			num_to_write = uart_dma->tx_write_idx - read_idx;
			//num_to_write &= (uart_dma->tx_buf_size - 1);	//what if we don't wrap around? It will be clipped by to_wrap anyway...
			to_wrap = uart_dma->tx_buf_size - read_idx;
			if(num_to_write > to_wrap)
				num_to_write = to_wrap;
			uart_dma->tx_transfer_size = num_to_write;
			dma_disable(uart_dma->tx_dma);	//The DMA controller may be enabled with num_words set to 0, the documentation is not clear on what happens in this case... The transfer complete interrupt may not trigger, leaving the DMA enabled.
			dma_set_num_words(uart_dma->tx_dma, num_to_write);
			dma_enable(uart_dma->tx_dma);
		}
	}
}
