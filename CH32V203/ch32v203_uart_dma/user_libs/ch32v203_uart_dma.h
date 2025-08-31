/*
 * ch32v203_uart.h
 *
 *  Created on: Jun 29, 2024
 *      Author: Steve
 */

#ifndef _CH32V203_UART_H_
#define _CH32V203_UART_H_

// HINT: user options
#define USE_UART_DMA_1 1
#define USE_UART_DMA_2 1
#define USE_UART_DMA_3 1
#define USE_UART_DMA_4 1

//buffer sizes must be powers of 2
#define UART_DMA_1_RX_BUF_SIZE 512
#define UART_DMA_1_TX_BUF_SIZE 512
#define UART_DMA_2_RX_BUF_SIZE 64
#define UART_DMA_2_TX_BUF_SIZE 64
#define UART_DMA_3_RX_BUF_SIZE 64
#define UART_DMA_3_TX_BUF_SIZE 64
#define UART_DMA_4_RX_BUF_SIZE 64
#define UART_DMA_4_TX_BUF_SIZE 64

// HINT: do not change any defines below this line
typedef struct _UART_DMA
{
	DMA_Channel_TypeDef* rx_dma;
	uint16_t rx_buf_size;
	uint8_t* rx_buf;
	uint16_t rx_read_idx;
	DMA_Channel_TypeDef* tx_dma;
	uint16_t tx_buf_size;
	uint8_t* tx_buf;
	uint16_t tx_write_idx;
	uint16_t tx_transfer_size;
} uart_dma_t;

#if USE_UART_DMA_1
extern uart_dma_t uart_dma_1_i;
#define uart_dma_1 (&uart_dma_1_i)
#endif

#if USE_UART_DMA_2
extern uart_dma_t uart_dma_2_i;
#define uart_dma_2 (&uart_dma_2_i)
#endif

#if USE_UART_DMA_3
extern uart_dma_t uart_dma_3_i;
#define uart_dma_3 (&uart_dma_3_i)
#endif

#if USE_UART_DMA_4
extern uart_dma_t uart_dma_4_i;
#define uart_dma_4 (&uart_dma_4_i)
#endif

void uart_dma_init(USART_TypeDef* uart, uint32_t baud);

// Receive
#define uart_dma_bytes_available(uart_dma) ((uart_dma->rx_buf_size - (uint16_t)(uart_dma->rx_dma->CNTR) - uart_dma->rx_read_idx) & (uart_dma->rx_buf_size - 1))
//uint16_t uart_dma_bytes_available(uart_dma_t* uart_dma);
uint8_t uart_dma_peek(uart_dma_t* uart_dma);
uint8_t uart_dma_read_byte(uart_dma_t* uart_dma);
void uart_dma_read_bytes(uart_dma_t* uart_dma, uint8_t* dest, uint16_t num_bytes);
uint16_t uart_dma_read_bytes_until(uart_dma_t* uart_dma, uint8_t terminator, uint8_t* dest, uint16_t num_bytes);
uint16_t uart_dma_get_string(uart_dma_t* uart_dma, char* dest, uint16_t buf_size);

// Send
#define uart_dma_bytes_available_for_write(uart_dma) (((uint16_t)(uart_dma->tx_dma->MADDR - (uint32_t)(uart_dma->tx_buf)) + ~uart_dma->tx_write_idx) & (uart_dma->tx_buf_size - 1))
//uint16_t uart_dma_bytes_available_for_write(uart_dma_t* uart_dma);
void uart_dma_write_byte(uart_dma_t* uart_dma, uint8_t val);
uint16_t uart_dma_write_string(uart_dma_t* uart_dma, char* str);
void uart_dma_write_bytes(uart_dma_t* uart_dma, uint8_t* src, uint16_t num_bytes);

#endif /* _CH32V203_UART_H_ */
