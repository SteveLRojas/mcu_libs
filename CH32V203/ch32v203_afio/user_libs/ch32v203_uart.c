#include "ch32v20x.h"
#include "ch32v203_rcc.h"
#include "fifo.h"
#include "ch32v203_uart.h"

#define UART_RX_INT_DISABLE(uart) ((uart)->CTLR1 &= (~USART_CTLR1_RXNEIE))
#define UART_RX_INT_ENABLE(uart) ((uart)->CTLR1 |= USART_CTLR1_RXNEIE)
#define UART_TX_INT_DISABLE(uart) ((uart)->CTLR1 &= (~USART_CTLR1_TCIE))
#define UART_TX_INT_ENABLE(uart) ((uart)->CTLR1 |= USART_CTLR1_TCIE)

void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void UART4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

#if USE_UART_1
uint8_t uart1_rx_buf[UART1_RX_BUF_SIZE];
uint8_t uart1_tx_buf[UART1_TX_BUF_SIZE];
volatile fifo_t uart1_rx_fifo_i;
volatile fifo_t uart1_tx_fifo_i;
#else
#define uart1_rx_fifo (fifo_t*)NULL
#define uart1_tx_fifo (fifo_t*)NULL
#endif

#if USE_UART_2
uint8_t uart2_rx_buf[UART2_RX_BUF_SIZE];
uint8_t uart2_tx_buf[UART2_TX_BUF_SIZE];
volatile fifo_t uart2_rx_fifo_i;
volatile fifo_t uart2_tx_fifo_i;
#else
#define uart2_rx_fifo (fifo_t*)NULL
#define uart2_tx_fifo (fifo_t*)NULL
#endif

#if USE_UART_3
uint8_t uart3_rx_buf[UART3_RX_BUF_SIZE];
uint8_t uart3_tx_buf[UART3_TX_BUF_SIZE];
volatile fifo_t uart3_rx_fifo_i;
volatile fifo_t uart3_tx_fifo_i;
#else
#define uart3_rx_fifo (fifo_t*)NULL
#define uart3_tx_fifo (fifo_t*)NULL
#endif

#if USE_UART_4
uint8_t uart4_rx_buf[UART4_RX_BUF_SIZE];
uint8_t uart4_tx_buf[UART4_TX_BUF_SIZE];
volatile fifo_t uart4_rx_fifo_i;
volatile fifo_t uart4_tx_fifo_i;
#else
#define uart4_rx_fifo (fifo_t*)NULL
#define uart4_tx_fifo (fifo_t*)NULL
#endif


void USART1_IRQHandler(void)
{
	if(USART1->STATR & USART_STATR_RXNE)	//receive buffer not empty
	{
		USART1->STATR = USART1->STATR & ~USART_STATR_RXNE;
		fifo_push((fifo_t*)uart1_rx_fifo, (uint8_t)(USART1->DATAR));
	}

	if(USART1->STATR & USART_STATR_TC)	//transmit buffer empty
	{
		USART1->STATR &= ~USART_STATR_TC;
		if(fifo_num_used(uart1_tx_fifo))
		{
			USART1->DATAR = fifo_pop((fifo_t*)uart1_tx_fifo);
		}
	}
}

void USART2_IRQHandler(void)
{
	if(USART2->STATR & USART_STATR_RXNE)	//receive buffer not empty
	{
		USART2->STATR = USART2->STATR & ~USART_STATR_RXNE;
		fifo_push((fifo_t*)uart2_rx_fifo, (uint8_t)(USART2->DATAR));
	}

	if(USART2->STATR & USART_STATR_TC)	//transmit buffer empty
	{
		USART2->STATR &= ~USART_STATR_TC;
		if(fifo_num_used(uart2_tx_fifo))
		{
			USART2->DATAR = fifo_pop((fifo_t*)uart2_tx_fifo);
		}
	}
}

void USART3_IRQHandler(void)
{
	if(USART3->STATR & USART_STATR_RXNE)	//receive buffer not empty
	{
		USART3->STATR = USART3->STATR & ~USART_STATR_RXNE;
		fifo_push((fifo_t*)uart3_rx_fifo, (uint8_t)(USART3->DATAR));
	}

	if(USART3->STATR & USART_STATR_TC)	//transmit buffer empty
	{
		USART3->STATR &= ~USART_STATR_TC;
		if(fifo_num_used(uart3_tx_fifo))
		{
			USART3->DATAR = fifo_pop((fifo_t*)uart3_tx_fifo);
		}
	}
}

void UART4_IRQHandler()
{
	if(UART4->STATR & USART_STATR_RXNE)	//receive buffer not empty
	{
		UART4->STATR = UART4->STATR & ~USART_STATR_RXNE;
		fifo_push((fifo_t*)uart4_rx_fifo, (uint8_t)(UART4->DATAR));
	}

	if(UART4->STATR & USART_STATR_TC)	//transmit buffer empty
	{
		UART4->STATR &= ~USART_STATR_TC;
		if(fifo_num_used(uart4_tx_fifo))
		{
			UART4->DATAR = fifo_pop((fifo_t*)uart4_tx_fifo);
		}
	}
}

// HINT: Be sure to enable interrupts in PFIC
void uart_init(USART_TypeDef* uart, uint32_t baud)
{
	uint32_t apbclock = 0x00;
	uint32_t usart_div = 0x00;

	if(baud)
	{
		if(uart == USART1)
		{
			fifo_init((fifo_t*)uart1_rx_fifo, uart1_rx_buf, UART1_RX_BUF_SIZE);
			fifo_init((fifo_t*)uart1_tx_fifo, uart1_tx_buf, UART1_TX_BUF_SIZE);
		}
		if(uart == USART2)
		{
			fifo_init((fifo_t*)uart2_rx_fifo, uart2_rx_buf, UART2_RX_BUF_SIZE);
			fifo_init((fifo_t*)uart2_tx_fifo, uart2_tx_buf, UART2_TX_BUF_SIZE);
		}
		if(uart == USART3)
		{
			fifo_init((fifo_t*)uart3_rx_fifo, uart3_rx_buf, UART3_RX_BUF_SIZE);
			fifo_init((fifo_t*)uart3_tx_fifo, uart3_tx_buf, UART3_TX_BUF_SIZE);
		}
		if(uart == UART4)
		{
			fifo_init((fifo_t*)uart4_rx_fifo, uart4_rx_buf, UART4_RX_BUF_SIZE);
			fifo_init((fifo_t*)uart4_tx_fifo, uart4_tx_buf, UART4_TX_BUF_SIZE);
		}

		apbclock = (uart == USART1) ? rcc_compute_pclk2_freq() : rcc_compute_pclk1_freq(); // USART1 is on APB2, others are on APB1
		usart_div = ((apbclock << 1) / baud);
		usart_div = usart_div + 1;
		usart_div = usart_div >> 1;

		uart->BRR = (uint16_t)usart_div;
		uart->CTLR3 = 0x0000;	//CTS disabled, RTS disabled, smartcard disabled, infrared disabled, DMA disabled
		uart->CTLR2 = 0x0000;	//LIN mode disabled, 1 stop bit
		uart->STATR = 0;
		uart->CTLR1 = USART_CTLR1_RE | USART_CTLR1_TE | USART_CTLR1_RXNEIE | USART_CTLR1_TCIE | USART_CTLR1_UE;
	}
	else
	{
		// Turn off UART interrupts
		uart->CTLR1 = 0x0000;
	}
}

// Receive
uint16_t uart_bytes_available(volatile fifo_t* rx_fifo)
{
	return fifo_num_used(rx_fifo);
}

uint16_t uart_peek(volatile fifo_t* rx_fifo)
{
	return fifo_peek(rx_fifo);
}

uint8_t uart_read_byte(USART_TypeDef* uart, volatile fifo_t* rx_fifo)
{
	uint8_t popped;

	while(fifo_empty(rx_fifo));

	UART_RX_INT_DISABLE(uart);
	popped = fifo_pop((fifo_t*)rx_fifo);
	UART_RX_INT_ENABLE(uart);

	return popped;
}

void uart_read_bytes(USART_TypeDef* uart, volatile fifo_t* rx_fifo, uint8_t* dest, uint16_t num_bytes)
{
	uint16_t num_remaining = num_bytes;
	uint16_t num_to_read = 0;

	while(num_remaining)
	{
		num_to_read = fifo_num_used(rx_fifo);
		if(num_to_read > num_remaining)
		{
			num_to_read = num_remaining;
		}

		UART_RX_INT_DISABLE(uart);
		fifo_read((fifo_t*)rx_fifo, dest, num_to_read);
		UART_RX_INT_ENABLE(uart);

		num_remaining -= num_to_read;
		dest += num_to_read;
	}

	return;
}

uint16_t uart_read_bytes_until(USART_TypeDef* uart, volatile fifo_t* rx_fifo, uint8_t terminator, uint8_t* dest, uint16_t num_bytes)
{
	uint16_t num_remaining = num_bytes;
	uint8_t popped;

	while(num_remaining)
	{
		if(fifo_empty(rx_fifo))
			continue;

		UART_RX_INT_DISABLE(uart);
		popped = fifo_pop((fifo_t*)rx_fifo);
		UART_RX_INT_ENABLE(uart);

		if(popped == terminator)
			break;

		--num_remaining;
		*dest++ = popped;
	}

	return (num_bytes - num_remaining);
}

uint16_t uart_get_string(USART_TypeDef* uart, volatile fifo_t* rx_fifo, char* buf, uint16_t buf_size)
{
	uint16_t bytes_read = 0;
	char received_data;

	--buf_size;	//leave space for the null byte
	while(bytes_read < buf_size)
	{
		while(fifo_empty(rx_fifo));

		UART_RX_INT_DISABLE(uart);
		received_data = (char)fifo_pop((fifo_t*)rx_fifo);
		UART_RX_INT_ENABLE(uart);

		buf[bytes_read++] = received_data;
		if(received_data == '\r' || received_data == '\n')
			break;
	}
	buf[bytes_read] = '\0';
	return bytes_read;
}

// Send
uint16_t uart_bytes_available_for_write(volatile fifo_t* tx_fifo)
{
	return fifo_num_free(tx_fifo);
}

void uart_write_byte(USART_TypeDef* uart, volatile fifo_t* tx_fifo, uint8_t val)
{
	UART_TX_INT_DISABLE(uart);
	if(fifo_empty(tx_fifo) && (uart->STATR & USART_STATR_TXE))
	{
		uart->DATAR = val;	//send data directly to UART
	}
	else
	{
		fifo_push((fifo_t*)tx_fifo, val);
	}
	UART_TX_INT_ENABLE(uart);
}

uint16_t uart_write_string(USART_TypeDef* uart, volatile fifo_t* tx_fifo, char* str)
{
	uint16_t len = 0;

	while(str[len])
	{
		++len;
	}

	uart_write_bytes(uart, tx_fifo, (uint8_t*)str, len);
	return len;
}

void uart_write_bytes(USART_TypeDef* uart, volatile fifo_t* tx_fifo, uint8_t* src, uint16_t num_bytes)
{
	uint16_t num_to_write = 0;

	UART_TX_INT_DISABLE(uart);
	if(fifo_empty(tx_fifo) && (uart->STATR & USART_STATR_TXE))
	{
		uart->DATAR = *src++; 		//send data directly to UART
		--num_bytes;
	}
	UART_TX_INT_ENABLE(uart);

	while(num_bytes)
	{
		num_to_write = fifo_num_free(tx_fifo);
		if(!num_to_write)
			continue;
		if(num_to_write > num_bytes)
		{
			num_to_write = num_bytes;
		}

		UART_TX_INT_DISABLE(uart);
		fifo_write((fifo_t*)tx_fifo, src, num_to_write);
		UART_TX_INT_ENABLE(uart);
		num_bytes -= num_to_write;
		src += num_to_write;
	}
}
