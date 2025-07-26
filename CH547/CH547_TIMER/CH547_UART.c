#include "CH547.H"
#include "CH547_RCC.h"
#include "CH547_FIFO.h"
#include "CH547_UART.h"

#if USE_UART_0
volatile UINT8 uart0_rx_buf[UART_RX_BUF_SIZE];
volatile UINT8 uart0_tx_buf[UART_TX_BUF_SIZE];
volatile fifo_t uart0_rx_fifo;
volatile fifo_t uart0_tx_fifo;
#define uart0_rx_fifo_ptr (&uart0_rx_fifo)
#define uart0_tx_fifo_ptr (&uart0_tx_fifo)
#else
#define uart0_rx_fifo_ptr (fifo_t*)NULL
#define uart0_tx_fifo_ptr (fifo_t*)NULL
#endif

#if USE_UART_1
volatile UINT8 uart1_rx_buf[UART_RX_BUF_SIZE];
volatile UINT8 uart1_tx_buf[UART_TX_BUF_SIZE];
volatile fifo_t uart1_rx_fifo;
volatile fifo_t uart1_tx_fifo;
#define uart1_rx_fifo_ptr (&uart1_rx_fifo)
#define uart1_tx_fifo_ptr (&uart1_tx_fifo)
#else
#define uart1_rx_fifo_ptr (fifo_t*)NULL
#define uart1_tx_fifo_ptr (fifo_t*)NULL
#endif

#if USE_UART_2
volatile UINT8 uart2_rx_buf[UART_RX_BUF_SIZE];
volatile UINT8 uart2_tx_buf[UART_TX_BUF_SIZE];
volatile fifo_t uart2_rx_fifo;
volatile fifo_t uart2_tx_fifo;
#define uart2_rx_fifo_ptr (&uart2_rx_fifo)
#define uart2_tx_fifo_ptr (&uart2_tx_fifo)
#else
#define uart2_rx_fifo_ptr (fifo_t*)NULL
#define uart2_tx_fifo_ptr (fifo_t*)NULL
#endif

#if USE_UART_3
volatile UINT8 uart3_rx_buf[UART_RX_BUF_SIZE];
volatile UINT8 uart3_tx_buf[UART_TX_BUF_SIZE];
volatile fifo_t uart3_rx_fifo;
volatile fifo_t uart3_tx_fifo;
#define uart3_rx_fifo_ptr (&uart3_rx_fifo)
#define uart3_tx_fifo_ptr (&uart3_tx_fifo)
#else
#define uart3_rx_fifo_ptr (fifo_t*)NULL
#define uart3_tx_fifo_ptr (fifo_t*)NULL
#endif

volatile fifo_t* code uart_rx_fifos[4] = {uart0_rx_fifo_ptr, uart1_rx_fifo_ptr, uart2_rx_fifo_ptr, uart3_rx_fifo_ptr};
volatile fifo_t* code uart_tx_fifos[4] = {uart0_tx_fifo_ptr, uart1_tx_fifo_ptr, uart2_tx_fifo_ptr, uart3_tx_fifo_ptr};
UINT8 uart_idle[4];

#if USE_UART_0
void uart0_isr(void) interrupt INT_NO_UART0
{
	if(RI)
    {
        RI = 0;
        fifo_push(uart_rx_fifos[UART_0], SBUF);
    }
    if(TI)
    {
        TI = 0;
        if(fifo_num_used(uart_tx_fifos[UART_0]))
            SBUF = fifo_pop(uart_tx_fifos[UART_0]);
        else
            uart_idle[UART_0] = 1;
    }
}
#endif

#if USE_UART_1
void uart1_isr(void) interrupt INT_NO_UART1
{
	if(SIF1 & bU1RI)
    {
        SIF1 = bU1RI;
        fifo_push(uart_rx_fifos[UART_1], SBUF1);
    }
    if(SIF1 & bU1TI)
    {
        SIF1 = bU1TI;
        if(fifo_num_used(uart_tx_fifos[UART_1]))
            SBUF1 = fifo_pop(uart_tx_fifos[UART_1]);
        else
            uart_idle[UART_1] = 1;
    }
}
#endif

#if USE_UART_2
void uart2_isr(void) interrupt INT_NO_UART2
{
	if(SIF2 & bU2RI)
	{
		SIF2 = bU2RI;
		fifo_push(uart_rx_fifos[UART_2], SBUF2);
	}
	if(SIF2 & bU2TI)
	{
		SIF2 = bU2TI;
		if(fifo_num_used(uart_tx_fifos[UART_2]))
            SBUF2 = fifo_pop(uart_tx_fifos[UART_2]);
        else
            uart_idle[UART_2] = 1;
	}
}
#endif

#if USE_UART_3
void uart3_isr(void) interrupt INT_NO_UART3
{
	if(SIF3 & bU3RI)
	{
		SIF3 = bU3RI;
		fifo_push(uart_rx_fifos[UART_3], SBUF3);
	}
	if(SIF3 & bU3TI)
	{
		SIF3 = bU3TI;
		if(fifo_num_used(uart_tx_fifos[UART_3]))
            SBUF3 = fifo_pop(uart_tx_fifos[UART_3]);
        else
            uart_idle[UART_3] = 1;
	}
}
#endif

#if USE_UART_0
void uart0_init(UINT8 uart0_tim, UINT32 baud, UINT8 pin_map)
{
	if(!baud)
	{
		REN = 0;
		ES = 0;
		return;
	}
	
	switch(uart0_tim)
	{
		case TIMER_0:
			//Timer 0 can not be used for the UART!
			return;
		case TIMER_1:
			uart_idle[UART_0] = 1;
			PCON |= SMOD;
			TMOD &= 0x0F;
			TMOD |= bT1_M1;
			TCLK = 0;
			RCLK = 0;
			T2MOD |= bTMR_CLK | bT1_CLK;
			TR1 = 1;
			TH1 = 256ul - FREQ_SYS / 16ul / baud;
			SCON = 0x50;	//SM0 = 0, SM1 = 1, SM2 = 0, REN = 1
			break;
		case TIMER_2:
			uart_idle[UART_0] = 1;
			T2CON = 0x34;	//RCLK = 1, TCLK = 1, TR2 = 1
			T2MOD |= bTMR_CLK | bT2_CLK;
			RCAP2 = 65536ul - FREQ_SYS / 16ul / baud;
			SCON = 0x50;	//SM0 = 0, SM1 = 1, SM2 = 0, REN = 1
			break;
		default:
			return;
	}
	
	PIN_FUNC &= ~bUART0_PIN_X;
	PIN_FUNC |= pin_map;
	
	fifo_init(&uart0_rx_fifo, uart0_rx_buf, UART_RX_BUF_SIZE);
	fifo_init(&uart0_tx_fifo, uart0_tx_buf, UART_TX_BUF_SIZE);
	ES = 1;	//enable interrupts
}
#endif

#if USE_UART_1
void uart1_init(UINT32 baud, UINT8 pin_map)
{
	if(!baud)
	{
		SCON1 &= ~bU1REN;
		IE_UART1 = 0;
		return;
	}
	
	uart_idle[UART_1] = 1;
	SCON1 = 0x30;	//U1SM0 = 0, U1SMOD = 1, U1REN = 1
	SBAUD1 = 256ul - FREQ_SYS / 16ul / baud;
	
	PIN_FUNC &= ~bUART1_PIN_X;
	PIN_FUNC |= pin_map;
	
	fifo_init(&uart1_rx_fifo, uart1_rx_buf, UART_RX_BUF_SIZE);
	fifo_init(&uart1_tx_fifo, uart1_tx_buf, UART_TX_BUF_SIZE);
	IE_UART1 = 1;	//enable interrupts
}
#endif

#if USE_UART_2
void uart2_init(UINT32 baud)
{
	if(!baud)
	{
		SCON2 &= ~bU2REN;
		IE_UART2 = 0;
		return;
	}
	
	uart_idle[UART_2] = 1;
	SCON2 = 0x30;	//U2SM0 = 0, U2SMOD = 1, U2REN = 1
	SBAUD2 = 256ul - FREQ_SYS / 16ul / baud;
	
	fifo_init(&uart2_rx_fifo, uart2_rx_buf, UART_RX_BUF_SIZE);
	fifo_init(&uart2_tx_fifo, uart2_tx_buf, UART_TX_BUF_SIZE);
	IE_UART2 = 1;	//enable interrupts
}
#endif

#if USE_UART_3
void uart3_init(UINT32 baud)
{
	if(!baud)
	{
		SCON3 &= ~bU3REN;
		IE_UART3 = 0;
		return;
	}
	
	uart_idle[UART_3] = 1;
	SCON3 = 0x30;	//U3SM0 = 0, U3SMOD = 1, U3REN = 1
	SBAUD3 = 256ul - FREQ_SYS / 16ul / baud;
	
	fifo_init(&uart3_rx_fifo, uart3_rx_buf, UART_RX_BUF_SIZE);
	fifo_init(&uart3_tx_fifo, uart3_tx_buf, UART_TX_BUF_SIZE);
	IE_UART3 = 1;	//enable interrupts
}
#endif

// Receive
UINT16 uart_bytes_available(UINT8 uart)
{
	return fifo_num_used(uart_rx_fifos[uart]);
}

UINT8 uart_peek(UINT8 uart)
{
	return fifo_peek(uart_rx_fifos[uart]);
}

UINT8 uart_read_byte(UINT8 uart)
{
	UINT8 popped;

	while(fifo_empty(uart_rx_fifos[uart]));
	
	uart_interrupt_disable(uart);
	popped = fifo_pop(uart_rx_fifos[uart]);
	uart_interrupt_enable(uart);
	
	return popped;
}

void uart_read_bytes(UINT8 uart, UINT8* dest, UINT16 num_bytes)
{
	UINT16 num_remaining = num_bytes;
	UINT16 num_to_read = 0;

	while(num_remaining)
	{
		num_to_read = fifo_num_used(uart_rx_fifos[uart]);
		if (num_to_read > num_remaining)
		{
			num_to_read = num_remaining;
		}

		uart_interrupt_disable(uart);
		fifo_read(uart_rx_fifos[uart], dest, num_to_read);
		uart_interrupt_enable(uart);

		num_remaining -= num_to_read;
		dest += num_to_read;
	}

	return;
}

UINT16 uart_read_bytes_until(UINT8 uart, UINT8 terminator, UINT8* dest, UINT16 num_bytes)
{
	UINT16 num_remaining = num_bytes;
	UINT8 popped;

	while(num_remaining)
	{
		if(fifo_empty(uart_rx_fifos[uart]))
			continue;

		uart_interrupt_disable(uart);
		popped = fifo_pop(uart_rx_fifos[uart]);
		uart_interrupt_enable(uart);

		if(popped == terminator)
			break;

		--num_remaining;
		*dest++ = popped;
	}

	return (num_bytes - num_remaining);
}

UINT16 uart_get_string(UINT8 uart, char* buf, UINT16 buf_size)
{
	UINT16 bytes_read = 0;
	char received_data;

	--buf_size;	//leave space for the null byte
	while(bytes_read < buf_size)
	{
		while(fifo_empty(uart_rx_fifos[uart])) {};
		
		uart_interrupt_disable(uart);
		received_data = (char)fifo_pop(uart_rx_fifos[uart]);
		uart_interrupt_enable(uart);
		
		buf[bytes_read++] = received_data;
		if(received_data == '\r' || received_data == '\n')
			break;
	}
	buf[bytes_read] = '\0';
	return bytes_read;
}

// Send
UINT16 uart_bytes_available_for_write(UINT8 uart)
{
	return fifo_num_free(uart_tx_fifos[uart]);
}

void uart_write_byte(UINT8 uart, UINT8 val)
{
	uart_interrupt_disable(uart);
	if(uart_idle[uart])	//transmit buffer empty
	{
		//send data directly to UART
		switch(uart)
		{
			case UART_0:
				SBUF = val;
				break;
			case UART_1:
				SBUF1 = val;
				break;
			case UART_2:
				SBUF2 = val;
				break;
			case UART_3:
				SBUF3 = val;
				break;
			default: return;
		}
		uart_idle[uart] = 0;
	}
	else
	{
		fifo_push(uart_tx_fifos[uart], val);
	}
	uart_interrupt_enable(uart);
}

UINT16 uart_write_string(UINT8 uart, char* str)
{
	UINT16 len = 0;
	
	while(str[len])
	{
		++len;
	}
	
	uart_write_bytes(uart, (UINT8*)str, len);
	return len;
}

void uart_write_bytes(UINT8 uart, UINT8* src, UINT16 num_bytes)
{
	UINT16 num_to_write = 0;

	uart_interrupt_disable(uart);
	if(uart_idle[uart])	//transmit buffer empty
	{
		//send data directly to UART
		switch(uart)
		{
			case UART_0:
				SBUF = *src++;
				break;
			case UART_1:
				SBUF1 = *src++;
				break;
			case UART_2:
				SBUF2 = *src++;
				break;
			case UART_3:
				SBUF3 = *src++;
				break;
			default: return;
		}
		uart_idle[uart] = 0;
		--num_bytes;
	}
	uart_interrupt_enable(uart);

	while(num_bytes)
	{
		num_to_write = fifo_num_free(uart_tx_fifos[uart]);
		if(!num_to_write)
			continue;
		if(num_to_write > num_bytes)
		{
			num_to_write = num_bytes;
		}

		uart_interrupt_disable(uart);
		fifo_write(uart_tx_fifos[uart], src, num_to_write);
		uart_interrupt_enable(uart);
		
		num_bytes -= num_to_write;
		src += num_to_write;
	}
}

void uart_interrupt_disable(UINT8 uart)
{
	switch(uart)
	{
		case UART_0:
			ES = 0;
			break;
		case UART_1:
			IE_UART1 = 0;
			break;
		case UART_2:
			IE_UART2 = 0;
			break;
		case UART_3:
			IE_UART3 = 0;
			break;
		default: return;
	}
}

void uart_interrupt_enable(UINT8 uart)
{
	switch(uart)
	{
		case UART_0:
			ES = 1;
			break;
		case UART_1:
			IE_UART1 = 1;
			break;
		case UART_2:
			IE_UART2 = 1;
			break;
		case UART_3:
			IE_UART3 = 1;
			break;
		default: return;
	}
}
