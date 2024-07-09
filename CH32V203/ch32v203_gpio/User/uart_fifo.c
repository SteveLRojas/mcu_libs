/*
 * uart_fifo.c
 *
 *  Created on: Feb 18, 2024
 *      Author: Steve
 */
#include "ch32v20x.h"
#include "uart_fifo.h"

volatile uint8_t uart1_rx_buf[UART_RX_BUF_SIZE];
volatile uint8_t uart2_rx_buf[UART_RX_BUF_SIZE];
volatile uint8_t uart3_rx_buf[UART_RX_BUF_SIZE];

volatile uint8_t uart1_tx_buf[UART_TX_BUF_SIZE];
volatile uint8_t uart2_tx_buf[UART_TX_BUF_SIZE];
volatile uint8_t uart3_tx_buf[UART_TX_BUF_SIZE];


volatile uint8_t* uart_rx_bufs[3] = {uart1_rx_buf, uart2_rx_buf, uart3_rx_buf};
volatile uint16_t uart_rx_counts[3] = {0};
volatile uint16_t uart_rx_fronts[3] = {0};
volatile uint16_t uart_rx_backs[3] = {0};

volatile uint8_t* uart_tx_bufs[3] = {uart1_tx_buf, uart2_tx_buf, uart3_tx_buf};
volatile uint16_t uart_tx_counts[3] = {0};
volatile uint16_t uart_tx_fronts[3] = {0};
volatile uint16_t uart_tx_backs[3] = {0};


// #############################
// ########## RECEIVE ##########
// #############################
//no need to disable interrupts before calling
uint16_t uart_rx_num_used(uint8_t uart_idx)
{
	return uart_rx_counts[uart_idx];
}

//disable UART interrupts before calling (from user code..)
uint16_t uart_rx_num_free(uint8_t uart_idx)
{
	return (UART_RX_BUF_SIZE - uart_rx_counts[uart_idx]);
}

//no need to disable interrupts before calling
uint8_t uart_rx_empty(uint8_t uart_idx)
{
	return uart_rx_counts[uart_idx] == 0;
}

//disable UART interrupts before calling (from user code..)
uint8_t uart_rx_full(uint8_t uart_idx)
{
	return uart_rx_counts[uart_idx] >= UART_RX_BUF_SIZE;
}

//disable UART interrupts before calling (from user code..)
uint8_t uart_rx_push(uint8_t uart_idx, uint8_t data)
{
	if(uart_rx_full(uart_idx))
	{
		return 0; // FIFO full
	}

	uart_rx_bufs[uart_idx][uart_rx_backs[uart_idx]] = data;
	++uart_rx_backs[uart_idx];
	uart_rx_backs[uart_idx] &= (UART_RX_BUF_SIZE - 1); // Handle wrap-around
	++uart_rx_counts[uart_idx];

	return 1;
}

//disable UART interrupts before calling (from user code..)
uint8_t uart_rx_pop(uint8_t uart_idx)
{
	if(uart_rx_empty(uart_idx))
	{
		return 0; // Nothing in FIFO
	}

	uint8_t data = uart_rx_bufs[uart_idx][uart_rx_fronts[uart_idx]];
	++uart_rx_fronts[uart_idx];
	uart_rx_fronts[uart_idx] &= (UART_RX_BUF_SIZE - 1); // Handle wrap-around
	--uart_rx_counts[uart_idx];

	return data;
}

//no need to disable interrupts before calling
uint8_t uart_rx_peek(uint8_t uart_idx)
{
	if(uart_rx_empty(uart_idx))
	{
		return 0; // Nothing in FIFO
	}

	return uart_rx_bufs[uart_idx][uart_rx_fronts[uart_idx]];
}

//disable UART interrupts before calling (from user code..)
uint8_t uart_rx_read(uint8_t uart_idx, uint8_t* dest, uint16_t num_bytes)
{
	if(num_bytes > uart_rx_counts[uart_idx])
	{
		return 0; // Not enough data to read
	}

	// Read from FIFO to dest
	uart_rx_counts[uart_idx] -= num_bytes;
	while(num_bytes)
	{
		*dest = uart_rx_bufs[uart_idx][uart_rx_fronts[uart_idx]];
		++uart_rx_fronts[uart_idx];
		uart_rx_fronts[uart_idx] &= (UART_RX_BUF_SIZE - 1); // Handle wrap-around
		++dest;
		--num_bytes;
	}

	return 1;
}

//disable UART interrupts before calling (from user code..)
uint8_t uart_rx_write(uint8_t uart_idx, uint8_t* src, uint16_t num_bytes)
{
	if(num_bytes > (UART_RX_BUF_SIZE - uart_rx_counts[uart_idx]))
	{
		return 0; // Not enough space to write
	}

	// Write to FIFO from src
	uart_rx_counts[uart_idx] += num_bytes;
	while(num_bytes)
	{
		uart_rx_bufs[uart_idx][uart_rx_backs[uart_idx]] = *src;
		++uart_rx_backs[uart_idx];
		uart_rx_backs[uart_idx] &= (UART_RX_BUF_SIZE - 1); // Handle wrap-around
		++src;
		--num_bytes;
	}

	return 1;
}

// ##############################
// ########## TRANSMIT ##########
// ##############################
//no need to disable interrupts before calling
uint16_t uart_tx_num_used(uint8_t uart_idx)
{
	return uart_tx_counts[uart_idx];
}

//no need to disable interrupts before calling
uint16_t uart_tx_num_free(uint8_t uart_idx)
{
	return (UART_TX_BUF_SIZE - uart_tx_counts[uart_idx]);
}

//no need to disable interrupts before calling
uint8_t uart_tx_empty(uint8_t uart_idx)
{
	return uart_tx_counts[uart_idx] == 0;
}

//no need to disable interrupts before calling
uint8_t uart_tx_full(uint8_t uart_idx)
{
	return uart_tx_counts[uart_idx] >= UART_TX_BUF_SIZE;
}

//disable UART interrupts before calling (from user code..)
uint8_t uart_tx_push(uint8_t uart_idx, uint8_t data)
{
	if(uart_tx_full(uart_idx))
	{
		return 0; // FIFO full
	}

	uart_tx_bufs[uart_idx][uart_tx_backs[uart_idx]] = data;
	++uart_tx_backs[uart_idx];
	uart_tx_backs[uart_idx] &= (UART_TX_BUF_SIZE - 1); // Handle wrap-around
	++uart_tx_counts[uart_idx];

	return 1;
}

//disable UART interrupts before calling (from user code..)
uint8_t uart_tx_pop(uint8_t uart_idx)
{
	if(uart_tx_empty(uart_idx))
	{
		return 0; // Nothing in FIFO
	}

	uint8_t data = uart_tx_bufs[uart_idx][uart_tx_fronts[uart_idx]];
	++uart_tx_fronts[uart_idx];
	uart_tx_fronts[uart_idx] &= (UART_TX_BUF_SIZE - 1); // Handle wrap-around
	--uart_tx_counts[uart_idx];

	return data;
}

//no need to disable interrupts before calling
uint8_t uart_tx_peek(uint8_t uart_idx)
{
	if(uart_tx_empty(uart_idx))
	{
		return 0; // Nothing in FIFO
	}

	return uart_tx_bufs[uart_idx][uart_tx_fronts[uart_idx]];
}

//disable UART interrupts before calling (from user code..)
uint8_t uart_tx_read(uint8_t uart_idx, uint8_t* dest, uint16_t num_bytes)
{
	if(num_bytes > uart_tx_counts[uart_idx])
	{
		return 0; // Not enough data to read
	}

	// Read from FIFO to dest
	uart_tx_counts[uart_idx] -= num_bytes;
	while(num_bytes)
	{
		*dest = uart_tx_bufs[uart_idx][uart_tx_fronts[uart_idx]];
		++uart_tx_fronts[uart_idx];
		uart_tx_fronts[uart_idx] &= (UART_TX_BUF_SIZE - 1); // Handle wrap-around
		++dest;
		--num_bytes;
	}

	return 1;
}

//disable UART interrupts before calling (from user code..)
uint8_t uart_tx_write(uint8_t uart_idx, uint8_t* src, uint16_t num_bytes)
{
	if(num_bytes > (UART_TX_BUF_SIZE - uart_tx_counts[uart_idx]))
	{
		return 0; // Not enough space to write
	}

	// Write to FIFO from src
	uart_tx_counts[uart_idx] += num_bytes;
	while(num_bytes)
	{
		uart_tx_bufs[uart_idx][uart_tx_backs[uart_idx]] = *src;
		++uart_tx_backs[uart_idx];
		uart_tx_backs[uart_idx] &= (UART_TX_BUF_SIZE - 1); // Handle wrap-around
		++src;
		--num_bytes;
	}

	return 1;
}
