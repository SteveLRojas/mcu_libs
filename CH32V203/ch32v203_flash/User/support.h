/*
 * support.h
 *
 *  Created on: Dec 9, 2023
 *      Author: Steve
 */

#ifndef SUPPORT_H_
#define SUPPORT_H_

#define SUPPORT_USE_UART 0
#define SUPPORT_UART USART1
#define SUPPORT_UART_RX_FIFO uart1_rx_fifo
#define SUPPORT_UART_TX_FIFO uart1_tx_fifo
#define SUPPORT_USE_USBD_CDC 1
#define SUPPORT_USE_USBFSD_CDC 0
#define SUPPORT_USE_UART_DMA 0
#define SUPPORT_UART_DMA uart_dma_1
#define SUPPORT_USE_PSEUDO_RANDOM 1

void support_byte_to_hex(uint8_t value, char* buff);
void support_byte_to_hex_ln(uint8_t value, char* buff);
void support_word_to_hex(uint16_t value, char* buff);
void support_word_to_hex_ln(uint16_t value, char* buff);
void support_print_bytes(uint8_t* src, uint16_t num_bytes);
void support_print_hex_byte(uint8_t value);
void support_print_hex_word(uint16_t value);
#if SUPPORT_USE_PSEUDO_RANDOM
void support_print_test_data();
#endif
#if SUPPORT_USE_UART || SUPPORT_USE_UART_DMA
uint16_t support_get_hex();
#endif
void support_to_caps(char* str);

#endif /* SUPPORT_H_ */
