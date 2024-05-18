#include "CH552.H"
#include "CH552_UART_FIFO.h"

volatile UINT8 uart0_rx_buf[UART_RX_BUF_SIZE];
volatile UINT8 uart1_rx_buf[UART_RX_BUF_SIZE];

volatile UINT8 uart0_tx_buf[UART_TX_BUF_SIZE];
volatile UINT8 uart1_tx_buf[UART_TX_BUF_SIZE];


volatile UINT8* uart_rx_bufs[2] = {uart0_rx_buf, uart1_rx_buf};
volatile UINT16 uart_rx_counts[2] = {0};
volatile UINT16 uart_rx_fronts[2] = {0};
volatile UINT16 uart_rx_backs[2] = {0};

volatile UINT8* uart_tx_bufs[2] = {uart0_tx_buf, uart1_tx_buf};
volatile UINT16 uart_tx_counts[2] = {0};
volatile UINT16 uart_tx_fronts[2] = {0};
volatile UINT16 uart_tx_backs[2] = {0};


// #############################
// ########## RECEIVE ##########
// #############################
//no need to disable interrupts before calling
UINT16 uart_rx_num_used(UINT8 uart_idx)
{
	return uart_rx_counts[uart_idx];
}

//disable UART interrupts before calling (from user code..)
UINT16 uart_rx_num_free(UINT8 uart_idx)
{
	return (UART_RX_BUF_SIZE - uart_rx_counts[uart_idx]);
}

//no need to disable interrupts before calling
UINT8 uart_rx_empty(UINT8 uart_idx)
{
	return uart_rx_counts[uart_idx] == 0;
}

//disable UART interrupts before calling (from user code..)
UINT8 uart_rx_full(UINT8 uart_idx)
{
	return uart_rx_counts[uart_idx] >= UART_RX_BUF_SIZE;
}

//disable UART interrupts before calling (from user code..)
UINT8 uart_rx_push(UINT8 uart_idx, UINT8 val) reentrant
{
	if(uart_rx_full(uart_idx))
	{
		return 0; // FIFO full
	}

	uart_rx_bufs[uart_idx][uart_rx_backs[uart_idx]] = val;
	++uart_rx_backs[uart_idx];
	uart_rx_backs[uart_idx] &= (UART_RX_BUF_SIZE - 1); // Handle wrap-around
	++uart_rx_counts[uart_idx];

	return 1;
}

//disable UART interrupts before calling (from user code..)
UINT8 uart_rx_pop(UINT8 uart_idx)
{
	UINT8 val;
	if(uart_rx_empty(uart_idx))
	{
		return 0; // Nothing in FIFO
	}

	val = uart_rx_bufs[uart_idx][uart_rx_fronts[uart_idx]];
	++uart_rx_fronts[uart_idx];
	uart_rx_fronts[uart_idx] &= (UART_RX_BUF_SIZE - 1); // Handle wrap-around
	--uart_rx_counts[uart_idx];

	return val;
}

//no need to disable interrupts before calling
UINT8 uart_rx_peek(UINT8 uart_idx)
{
	if(uart_rx_empty(uart_idx))
	{
		return 0; // Nothing in FIFO
	}

	return uart_rx_bufs[uart_idx][uart_rx_fronts[uart_idx]];
}

//disable UART interrupts before calling (from user code..)
UINT8 uart_rx_read(UINT8 uart_idx, UINT8* dest, UINT16 num_bytes)
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
UINT8 uart_rx_write(UINT8 uart_idx, UINT8* src, UINT16 num_bytes)
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
UINT16 uart_tx_num_used(UINT8 uart_idx)
{
	return uart_tx_counts[uart_idx];
}

//no need to disable interrupts before calling
UINT16 uart_tx_num_free(UINT8 uart_idx)
{
	return (UART_TX_BUF_SIZE - uart_tx_counts[uart_idx]);
}

//no need to disable interrupts before calling
UINT8 uart_tx_empty(UINT8 uart_idx)
{
	return uart_tx_counts[uart_idx] == 0;
}

//no need to disable interrupts before calling
UINT8 uart_tx_full(UINT8 uart_idx)
{
	return uart_tx_counts[uart_idx] >= UART_TX_BUF_SIZE;
}

//disable UART interrupts before calling (from user code..)
UINT8 uart_tx_push(UINT8 uart_idx, UINT8 val)
{
	if(uart_tx_full(uart_idx))
	{
		return 0; // FIFO full
	}

	uart_tx_bufs[uart_idx][uart_tx_backs[uart_idx]] = val;
	++uart_tx_backs[uart_idx];
	uart_tx_backs[uart_idx] &= (UART_TX_BUF_SIZE - 1); // Handle wrap-around
	++uart_tx_counts[uart_idx];

	return 1;
}

//disable UART interrupts before calling (from user code..)
UINT8 uart_tx_pop(UINT8 uart_idx) reentrant
{
	UINT8 val;
	if(uart_tx_empty(uart_idx))
	{
		return 0; // Nothing in FIFO
	}

	val = uart_tx_bufs[uart_idx][uart_tx_fronts[uart_idx]];
	++uart_tx_fronts[uart_idx];
	uart_tx_fronts[uart_idx] &= (UART_TX_BUF_SIZE - 1); // Handle wrap-around
	--uart_tx_counts[uart_idx];

	return val;
}

//no need to disable interrupts before calling
UINT8 uart_tx_peek(UINT8 uart_idx)
{
	if(uart_tx_empty(uart_idx))
	{
		return 0; // Nothing in FIFO
	}

	return uart_tx_bufs[uart_idx][uart_tx_fronts[uart_idx]];
}

//disable UART interrupts before calling (from user code..)
UINT8 uart_tx_read(UINT8 uart_idx, UINT8* dest, UINT16 num_bytes)
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
UINT8 uart_tx_write(UINT8 uart_idx, UINT8* src, UINT16 num_bytes)
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
