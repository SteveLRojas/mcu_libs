/*
 * uart_fifo.h
 *
 *  Created on: Feb 18, 2024
 *      Author: Steve
 */
//TODO: have defines to enable each buffer
#ifndef USER_UART_FIFO_H_
#define USER_UART_FIFO_H_

//buffer sizes must be powers of 2
#define UART_TX_BUF_SIZE 32
#define UART_RX_BUF_SIZE 64

uint16_t uart_rx_num_used(uint8_t uart_idx);
uint16_t uart_rx_num_free(uint8_t uart_idx);
uint8_t uart_rx_empty(uint8_t uart_idx);
uint8_t uart_rx_full(uint8_t uart_idx);
uint8_t uart_rx_push(uint8_t uart_idx, uint8_t data);
uint8_t uart_rx_pop(uint8_t uart_idx);
uint8_t uart_rx_peek(uint8_t uart_idx);
uint8_t uart_rx_read(uint8_t uart_idx, uint8_t* dest, uint16_t num_bytes);
uint8_t uart_rx_write(uint8_t uart_idx, uint8_t* src, uint16_t num_bytes);

uint16_t uart_tx_num_used(uint8_t uart_idx);
uint16_t uart_tx_num_free(uint8_t uart_idx);
uint8_t uart_tx_empty(uint8_t uart_idx);
uint8_t uart_tx_full(uint8_t uart_idx);
uint8_t uart_tx_push(uint8_t uart_idx, uint8_t data);
uint8_t uart_tx_pop(uint8_t uart_idx);
uint8_t uart_tx_peek(uint8_t uart_idx);
uint8_t uart_tx_read(uint8_t uart_idx, uint8_t* dest, uint16_t num_bytes);
uint8_t uart_tx_write(uint8_t uart_idx, uint8_t* src, uint16_t num_bytes);

#endif /* USER_UART_FIFO_H_ */
