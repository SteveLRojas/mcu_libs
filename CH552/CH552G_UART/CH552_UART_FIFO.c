#include "CH552.H"
#include "CH552_UART_FIFO.h"

#if USE_UART_0
volatile UINT8 uart0_rx_buf[UART_RX_BUF_SIZE];
volatile UINT8 uart0_tx_buf[UART_TX_BUF_SIZE];
#else
#define uart0_rx_buf (UINT8*)0
#define uart0_tx_buf (UINT8*)0
#endif

#if USE_UART_1
volatile UINT8 uart1_rx_buf[UART_RX_BUF_SIZE];
volatile UINT8 uart1_tx_buf[UART_TX_BUF_SIZE];
#else
#define uart1_rx_buf (UINT8*)0
#define uart1_tx_buf (UINT8)0
#endif

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
/*UINT16 uart_rx_num_used(UINT8 uart_idx)
{
	return uart_rx_counts[uart_idx];
}*/

//disable UART interrupts before calling (from user code..)
/*UINT16 uart_rx_num_free(UINT8 uart_idx)
{
	return (UART_RX_BUF_SIZE - uart_rx_counts[uart_idx]);
}*/

//no need to disable interrupts before calling
/*UINT8 uart_rx_empty(UINT8 uart_idx)
{
	return uart_rx_counts[uart_idx] == 0;
}*/

//disable UART interrupts before calling (from user code..)
/*UINT8 uart_rx_full(UINT8 uart_idx)
{
	return uart_rx_counts[uart_idx] >= UART_RX_BUF_SIZE;
}*/

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
/*UINT8 uart_rx_peek(UINT8 uart_idx)
{
	if(uart_rx_empty(uart_idx))
	{
		return 0; // Nothing in FIFO
	}

	return uart_rx_bufs[uart_idx][uart_rx_fronts[uart_idx]];
}*/

//disable UART interrupts before calling (from user code..)
UINT8 uart_rx_read(UINT8 uart_idx, UINT8* dest, UINT16 num_bytes)
{
	UINT16 to_wrap;
	UINT8* read_ptr;
	UINT16 idx;
	
	if(num_bytes > uart_rx_counts[uart_idx])
	{
		return 0; // Not enough data to read
	}
	
	to_wrap = UART_RX_BUF_SIZE - uart_rx_fronts[uart_idx];
	if(num_bytes >= to_wrap)
	{
		// read until wrap around
		read_ptr = uart_rx_bufs[uart_idx] + uart_rx_fronts[uart_idx];
		for(idx = 0; idx < to_wrap; ++idx)
		{
			*dest = read_ptr[idx];
			++dest;
		}
		
		uart_rx_fronts[uart_idx] = 0;
		uart_rx_counts[uart_idx] -= to_wrap;
		num_bytes -= to_wrap;
	}
	
	// read remaining - no wrap around
	read_ptr = uart_rx_bufs[uart_idx] + uart_rx_fronts[uart_idx];
	for(idx = 0; idx < num_bytes; ++idx)
	{
		*dest = read_ptr[idx];
		++dest;
	}
	
	uart_rx_fronts[uart_idx] += num_bytes;
	uart_rx_counts[uart_idx] -= num_bytes;

	// Read from FIFO to dest
	/*uart_rx_counts[uart_idx] -= num_bytes;
	while(num_bytes)
	{
		*dest = uart_rx_bufs[uart_idx][uart_rx_fronts[uart_idx]];
		++uart_rx_fronts[uart_idx];
		uart_rx_fronts[uart_idx] &= (UART_RX_BUF_SIZE - 1); // Handle wrap-around
		++dest;
		--num_bytes;
	}*/

	return 1;
}

//disable UART interrupts before calling (from user code..)
UINT8 uart_rx_write(UINT8 uart_idx, UINT8* src, UINT16 num_bytes)
{
	UINT16 to_wrap;
	UINT8* write_ptr;
	UINT16 idx;
	
	if(num_bytes > (UART_RX_BUF_SIZE - uart_rx_counts[uart_idx]))
	{
		return 0; // Not enough space to write
	}
	
	to_wrap = UART_RX_BUF_SIZE - uart_rx_backs[uart_idx];
	if(num_bytes >= to_wrap)
	{
		// write until wrap around
		write_ptr = uart_rx_bufs[uart_idx] + uart_rx_backs[uart_idx];
		for(idx = 0; idx < to_wrap; ++idx)
		{
			write_ptr[idx] = *src;
			++src;
		}
		
		uart_rx_backs[uart_idx] = 0;
		uart_rx_counts[uart_idx] += to_wrap;
		num_bytes -= to_wrap;
	}
	
	// write remaining - no wrap around
	write_ptr = uart_rx_bufs[uart_idx] + uart_rx_backs[uart_idx];
	for(idx = 0; idx < num_bytes; ++idx)
	{
		write_ptr[idx] = *src;
		++src;
	}
	
	uart_rx_backs[uart_idx] += num_bytes;
	uart_rx_counts[uart_idx] += num_bytes;

	// Write to FIFO from src
	/*uart_rx_counts[uart_idx] += num_bytes;
	while(num_bytes)
	{
		uart_rx_bufs[uart_idx][uart_rx_backs[uart_idx]] = *src;
		++uart_rx_backs[uart_idx];
		uart_rx_backs[uart_idx] &= (UART_RX_BUF_SIZE - 1); // Handle wrap-around
		++src;
		--num_bytes;
	}*/

	return 1;
}

// ##############################
// ########## TRANSMIT ##########
// ##############################
//no need to disable interrupts before calling
/*UINT16 uart_tx_num_used(UINT8 uart_idx)
{
	return uart_tx_counts[uart_idx];
}*/

//no need to disable interrupts before calling
/*UINT16 uart_tx_num_free(UINT8 uart_idx)
{
	return (UART_TX_BUF_SIZE - uart_tx_counts[uart_idx]);
}*/

//no need to disable interrupts before calling
/*UINT8 uart_tx_empty(UINT8 uart_idx)
{
	return uart_tx_counts[uart_idx] == 0;
}*/

//no need to disable interrupts before calling
/*UINT8 uart_tx_full(UINT8 uart_idx)
{
	return uart_tx_counts[uart_idx] >= UART_TX_BUF_SIZE;
}*/

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
/*UINT8 uart_tx_peek(UINT8 uart_idx)
{
	if(uart_tx_empty(uart_idx))
	{
		return 0; // Nothing in FIFO
	}

	return uart_tx_bufs[uart_idx][uart_tx_fronts[uart_idx]];
}*/

//disable UART interrupts before calling (from user code..)
UINT8 uart_tx_read(UINT8 uart_idx, UINT8* dest, UINT16 num_bytes)
{
	UINT16 to_wrap;
	UINT8* read_ptr;
	UINT16 idx;
	
	if(num_bytes > uart_tx_counts[uart_idx])
	{
		return 0; // Not enough data to read
	}
	
	to_wrap = UART_TX_BUF_SIZE - uart_tx_fronts[uart_idx];
	if(num_bytes >= to_wrap)
	{
		// read until wrap around
		read_ptr = uart_tx_bufs[uart_idx] + uart_tx_fronts[uart_idx];
		for(idx = 0; idx < to_wrap; ++idx)
		{
			*dest = read_ptr[idx];
			++dest;
		}
		
		uart_tx_fronts[uart_idx] = 0;
		uart_tx_counts[uart_idx] -= to_wrap;
		num_bytes -= to_wrap;
	}
	
	// read remaining - no wrap around
	read_ptr = uart_tx_bufs[uart_idx] + uart_tx_fronts[uart_idx];
	for(idx = 0; idx < num_bytes; ++idx)
	{
		*dest = read_ptr[idx];
		++dest;
	}
	
	uart_tx_fronts[uart_idx] += num_bytes;
	uart_tx_counts[uart_idx] -= num_bytes;

	// Read from FIFO to dest
	/*uart_tx_counts[uart_idx] -= num_bytes;
	while(num_bytes)
	{
		*dest = uart_tx_bufs[uart_idx][uart_tx_fronts[uart_idx]];
		++uart_tx_fronts[uart_idx];
		uart_tx_fronts[uart_idx] &= (UART_TX_BUF_SIZE - 1); // Handle wrap-around
		++dest;
		--num_bytes;
	}*/

	return 1;
}

//disable UART interrupts before calling (from user code..)
UINT8 uart_tx_write(UINT8 uart_idx, UINT8* src, UINT16 num_bytes)
{
	UINT16 to_wrap;
	UINT8* write_ptr;
	UINT16 idx;
	
	if(num_bytes > (UART_TX_BUF_SIZE - uart_tx_counts[uart_idx]))
	{
		return 0; // Not enough space to write
	}
	
	to_wrap = UART_TX_BUF_SIZE - uart_tx_fronts[uart_idx];
	if(num_bytes >= to_wrap)
	{
		// write until wrap around
		write_ptr = uart_tx_bufs[uart_idx] + uart_tx_backs[uart_idx];
		for(idx = 0; idx < to_wrap; ++idx)
		{
			write_ptr[idx] = *src;
			++src;
		}
		
		uart_tx_backs[uart_idx] = 0;
		uart_tx_counts[uart_idx] += to_wrap;
		num_bytes -= to_wrap;
	}
	
	// write remaining - no wrap around
	write_ptr = uart_tx_bufs[uart_idx] + uart_tx_backs[uart_idx];
	for(idx = 0; idx < num_bytes; ++idx)
	{
		write_ptr[idx] = *src;
		++src;
	}
	
	uart_tx_backs[uart_idx] += num_bytes;
	uart_tx_counts[uart_idx] += num_bytes;

	// Write to FIFO from src
	/*uart_tx_counts[uart_idx] += num_bytes;
	while(num_bytes)
	{
		uart_tx_bufs[uart_idx][uart_tx_backs[uart_idx]] = *src;
		++uart_tx_backs[uart_idx];
		uart_tx_backs[uart_idx] &= (UART_TX_BUF_SIZE - 1); // Handle wrap-around
		++src;
		--num_bytes;
	}*/

	return 1;
}
