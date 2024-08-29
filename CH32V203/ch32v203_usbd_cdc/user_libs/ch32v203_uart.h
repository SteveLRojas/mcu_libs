/*
 * ch32v203_uart.h
 *
 *  Created on: Jun 29, 2024
 *      Author: Steve
 */

#ifndef _CH32V203_UART_H_
#define _CH32V203_UART_H_

// HINT: user options
#define USE_UART_1 1
#define USE_UART_2 1
#define USE_UART_3 1
#define USE_UART_4 1

//buffer sizes must be powers of 2
#define UART1_RX_BUF_SIZE 64
#define UART1_TX_BUF_SIZE 64
#define UART2_RX_BUF_SIZE 64
#define UART2_TX_BUF_SIZE 64
#define UART3_RX_BUF_SIZE 64
#define UART3_TX_BUF_SIZE 64
#define UART4_RX_BUF_SIZE 64
#define UART4_TX_BUF_SIZE 64

// HINT: do not change any defines below this line
extern volatile fifo_t uart1_rx_fifo_i;
extern volatile fifo_t uart1_tx_fifo_i;
extern volatile fifo_t uart2_rx_fifo_i;
extern volatile fifo_t uart2_tx_fifo_i;
extern volatile fifo_t uart3_rx_fifo_i;
extern volatile fifo_t uart3_tx_fifo_i;
extern volatile fifo_t uart4_rx_fifo_i;
extern volatile fifo_t uart4_tx_fifo_i;

#if USE_UART_1
#define uart1_rx_fifo (&uart1_rx_fifo_i)
#define uart1_tx_fifo (&uart1_tx_fifo_i)
#endif

#if USE_UART_2
#define uart2_rx_fifo (&uart2_rx_fifo_i)
#define uart2_tx_fifo (&uart2_tx_fifo_i)
#endif

#if USE_UART_3
#define uart3_rx_fifo (&uart3_rx_fifo_i)
#define uart3_tx_fifo (&uart3_tx_fifo_i)
#endif

#if USE_UART_4
#define uart4_rx_fifo (&uart4_rx_fifo_i)
#define uart4_tx_fifo (&uart4_tx_fifo_i)
#endif

void uart_init(USART_TypeDef* uart, uint32_t baud);

// Receive
uint16_t uart_bytes_available(volatile fifo_t* rx_fifo);
uint16_t uart_peek(volatile fifo_t* rx_fifo);
uint8_t uart_read_byte(USART_TypeDef* uart, volatile fifo_t* rx_fifo);
void uart_read_bytes(USART_TypeDef* uart, volatile fifo_t* rx_fifo, uint8_t* dest, uint16_t num_bytes);
uint16_t uart_read_bytes_until(USART_TypeDef* uart, volatile fifo_t* rx_fifo, uint8_t terminator, uint8_t* dest, uint16_t num_bytes);
uint16_t uart_get_string(USART_TypeDef* uart, volatile fifo_t* rx_fifo, char* buf, uint16_t buf_size);

// Send
uint16_t uart_bytes_available_for_write(volatile fifo_t* tx_fifo);
void uart_write_byte(USART_TypeDef* uart, volatile fifo_t* tx_fifo, uint8_t val);
uint16_t uart_write_string(USART_TypeDef* uart, volatile fifo_t* tx_fifo, char* str);
void uart_write_bytes(USART_TypeDef* uart, volatile fifo_t* tx_fifo, uint8_t* src, uint16_t num_bytes);

#endif /* _CH32V203_UART_H_ */
