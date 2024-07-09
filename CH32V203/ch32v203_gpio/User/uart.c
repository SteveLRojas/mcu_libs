/*
 * uart.c
 *
 *  Created on: Feb 18, 2024
 *      Author: Steve
 */

#include "ch32v20x.h"
#include "uart.h"
#include "uart_fifo.h"

#define MASK_TXE 0x0080
#define MASK_TC 0x0040
#define MASK_RXNE 0x0020

void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

static const IRQn_Type irq_tab[3] = {USART1_IRQn, USART2_IRQn, USART3_IRQn};

void USART1_IRQHandler(void)
{
	if(USART1->STATR & MASK_RXNE)	//receive buffer not empty
	{
		USART1->STATR = USART1->STATR & ~MASK_RXNE;
		uart_rx_push(0, (uint8_t)(USART1->DATAR));
	}

	if(USART1->STATR & MASK_TXE)	//transmit buffer empty
	{
		USART1->STATR &= ~MASK_TC;
		if(uart_tx_num_used(0))
		{
			USART1->DATAR = uart_tx_pop(0);
		}
	}
}

void USART2_IRQHandler(void)
{
	if(USART2->STATR & MASK_RXNE)	//receive buffer not empty
	{
		USART2->STATR = USART2->STATR & ~MASK_RXNE;
		uart_rx_push(1, (uint8_t)(USART2->DATAR));
	}

	if(USART2->STATR & MASK_TXE)	//transmit buffer empty
	{
		USART2->STATR &= ~MASK_TC;
		if(uart_tx_num_used(1))
		{
			USART2->DATAR = uart_tx_pop(1);
		}
	}
}

void USART3_IRQHandler(void)
{
	if(USART3->STATR & MASK_RXNE)	//receive buffer not empty
	{
		USART3->STATR = USART3->STATR & ~MASK_RXNE;
		uart_rx_push(2, (uint8_t)(USART3->DATAR));
	}

	if(USART3->STATR & MASK_TXE)	//transmit buffer empty
	{
		USART3->STATR &= ~MASK_TC;
		if(uart_tx_num_used(2))
		{
			USART3->DATAR = uart_tx_pop(2);
		}
	}
}

//disable a UART by setting its baud rate to 0
void uart_init(uint32_t baud1, uint32_t baud2, uint32_t baud3)
{
	uint32_t apbclock = 0x00;
	uint32_t usart_div = 0x00;
	RCC_ClocksTypeDef RCC_ClocksStatus;

	RCC_GetClocksFreq(&RCC_ClocksStatus);

	//HINT: USART1 is on APB2, others are on APB1
	if(baud1)
	{
		apbclock = RCC_ClocksStatus.PCLK2_Frequency;
		usart_div = ((apbclock << 1) / baud1);
		usart_div = usart_div + 1;
		usart_div = usart_div >> 1;

		USART1->BRR = (uint16_t)usart_div;
		USART1->CTLR3 = 0x0000;	//CTS disabled, RTS disabled, smartcard disabled, infrared disabled, DMA disabled
		USART1->CTLR2 = 0x0000;	//LIN mode disabled, 1 stop bit
		USART1->CTLR1 = 0x206C;	//word length = 8, no parity, receive and transmit enabled, TCIE enabled, RXNEIE enabled, UART enabled
		NVIC_EnableIRQ(USART1_IRQn);
	}
	else
	{
		USART1->CTLR1 = 0x0000;
		NVIC_DisableIRQ(USART1_IRQn);
	}

	if(baud2)
	{
		apbclock = RCC_ClocksStatus.PCLK1_Frequency;
		usart_div = ((apbclock << 1) / baud2);
		usart_div = usart_div + 1;
		usart_div = usart_div >> 1;

		USART2->BRR = (uint16_t)usart_div;
		USART2->CTLR3 = 0x0000;	//CTS disabled, RTS disabled, smartcard disabled, infrared disabled, DMA disabled
		USART2->CTLR2 = 0x0000;	//LIN mode disabled, 1 stop bit
		USART2->CTLR1 = 0x206C;	//word length = 8, no parity, receive and transmit enabled, TCIE enabled, RXNEIE enabled, UART enabled
		NVIC_EnableIRQ(USART2_IRQn);
	}
	else
	{
		USART2->CTLR1 = 0x0000;
		NVIC_DisableIRQ(USART2_IRQn);
	}

	if(baud3)
	{
		apbclock = RCC_ClocksStatus.PCLK1_Frequency;
		usart_div = ((apbclock << 1) / baud3);
		usart_div = usart_div + 1;
		usart_div = usart_div >> 1;

		USART3->BRR = (uint16_t)usart_div;
		USART3->CTLR3 = 0x0000;	//CTS disabled, RTS disabled, smartcard disabled, infrared disabled, DMA disabled
		USART3->CTLR2 = 0x0000;	//LIN mode disabled, 1 stop bit
		USART3->CTLR1 = 0x206C;	//word length = 8, no parity, receive and transmit enabled, TCIE enabled, RXNEIE enabled, UART enabled
		NVIC_EnableIRQ(USART3_IRQn);
	}
	else
	{
		USART3->CTLR1 = 0x0000;
		NVIC_DisableIRQ(USART3_IRQn);
	}
}

uint8_t get_uart_index(USART_TypeDef* uart)
{
	switch((uint32_t)uart)
	{
		case (uint32_t)USART1: return 0;
		case (uint32_t)USART2: return 1;
		case (uint32_t)USART3: return 2;
		default: return 0;
	}
}

// Receive
uint16_t uart_bytes_available(USART_TypeDef* uart)
{
	uint8_t uart_index = get_uart_index(uart);
	return uart_rx_num_used(uart_index);
}

int16_t uart_peek(USART_TypeDef* uart)
{
	uint8_t uart_index = get_uart_index(uart);

	if(uart_rx_empty(uart_index))
		return -1;

	return uart_rx_peek(uart_index);
}

uint8_t uart_read_byte(USART_TypeDef* uart)
{
	uint8_t uart_index = get_uart_index(uart);
	uint8_t popped;

	while(uart_rx_empty(uart_index));

	NVIC_DisableIRQ(irq_tab[uart_index]);
	popped = uart_rx_pop(uart_index);
	NVIC_EnableIRQ(irq_tab[uart_index]);
	return popped;
}

void uart_read_bytes(USART_TypeDef* uart, uint8_t* dest, uint16_t num_bytes)
{
	uint8_t uart_index = get_uart_index(uart);
	uint16_t num_remaining = num_bytes;
	uint16_t num_to_read = 0;

	while(num_remaining)
	{
		num_to_read = uart_rx_num_used(uart_index);
		if (num_to_read > num_remaining)
		{
			num_to_read = num_remaining;
		}

		NVIC_DisableIRQ(irq_tab[uart_index]);
		uart_rx_read(uart_index, dest, num_to_read);
		NVIC_EnableIRQ(irq_tab[uart_index]);

		num_remaining -= num_to_read;
		dest += num_to_read;
	}

	return;
}

uint16_t uart_read_bytes_until(USART_TypeDef* uart, uint8_t terminator, uint8_t* dest, uint16_t num_bytes)
{
	uint8_t uart_index = get_uart_index(uart);
	uint16_t num_remaining = num_bytes;
	uint8_t popped;

	while(num_remaining)
	{
		if(uart_rx_empty(uart_index))
			continue;

		NVIC_DisableIRQ(irq_tab[uart_index]);
		popped = uart_rx_pop(uart_index);
		NVIC_EnableIRQ(irq_tab[uart_index]);

		if(popped == terminator)
			break;

		--num_remaining;
		*dest++ = popped;
	}

	return (num_bytes - num_remaining);
}

uint16_t uart_get_string(USART_TypeDef* uart, char* buf, uint16_t size)
{
	uint8_t uart_index = get_uart_index(uart);
	uint16_t bytes_read = 0;
	char received_data;

	--size;	//leave space for the null byte
	while(bytes_read < size)
	{
		while(uart_rx_empty(uart_index)) {};
		NVIC_DisableIRQ(irq_tab[uart_index]);
		received_data = (char)uart_rx_pop(uart_index);
		NVIC_EnableIRQ(irq_tab[uart_index]);
		buf[bytes_read++] = received_data;
		if(received_data == '\r' || received_data == '\n')
			break;
	}
	buf[bytes_read] = '\0';
	return bytes_read;
}

// Send
uint16_t uart_bytes_available_for_write(USART_TypeDef* uart)
{
	uint8_t uart_index = get_uart_index(uart);
	return uart_tx_num_free(uart_index);
}

void uart_write_byte(USART_TypeDef* uart, uint8_t val)
{
	uint8_t uart_index;

	if(uart->STATR & MASK_TXE)	//transmit buffer empty
	{
		uart->DATAR = val;	//send data directly to UART
	}
	else
	{
		uart_index = get_uart_index(uart);
		NVIC_DisableIRQ(irq_tab[uart_index]);
		uart_tx_push(uart_index, val);
		NVIC_EnableIRQ(irq_tab[uart_index]);
	}
}

uint16_t uart_write_string(USART_TypeDef* uart, char* str)
{
	uint8_t uart_index = get_uart_index(uart);
	uint16_t len = 0;

	while(*str)
	{
		if(uart->STATR & MASK_TXE)	//transmit buffer empty
		{
			uart->DATAR = *str++;	//send data directly to UART
		}
		else
		{
			NVIC_DisableIRQ(irq_tab[uart_index]);
			uart_tx_push(uart_index, *str++);
			NVIC_EnableIRQ(irq_tab[uart_index]);
		}
		len++;
	}
	return len;
}

void uart_write_bytes(USART_TypeDef* uart, uint8_t* src, uint16_t num_bytes)
{
	uint8_t uart_index = get_uart_index(uart);

	uint16_t num_remaining = num_bytes;
	uint16_t num_to_write = 0;

	if(uart->STATR & MASK_TXE)	//transmit buffer empty
	{
		uart->DATAR = *src++;
		--num_remaining;
	}

	while(num_remaining)
	{
		num_to_write = uart_tx_num_free(uart_index);
		if(num_to_write > num_remaining)
		{
			num_to_write = num_remaining;
		}

		NVIC_DisableIRQ(irq_tab[uart_index]);
		uart_tx_write(uart_index, src, num_to_write);
		NVIC_EnableIRQ(irq_tab[uart_index]);
		num_remaining -= num_to_write;
		src += num_to_write;
	}
}



