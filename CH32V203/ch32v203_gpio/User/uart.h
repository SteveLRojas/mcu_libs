/*
 * uart.h
 *
 *  Created on: Feb 18, 2024
 *      Author: Steve
 */

#ifndef USER_UART_H_
#define USER_UART_H_

void uart_init(uint32_t baud1, uint32_t baud2, uint32_t baud3);
uint16_t uart_bytes_available(USART_TypeDef* uart);
int16_t uart_peek(USART_TypeDef* uart);
uint8_t uart_read_byte(USART_TypeDef* uart);
void uart_read_bytes(USART_TypeDef* uart, uint8_t* dest, uint16_t num_bytes);
uint16_t uart_read_bytes_until(USART_TypeDef* uart, uint8_t terminator, uint8_t* dest, uint16_t num_bytes);
uint16_t uart_get_string(USART_TypeDef* uart, char* buf, uint16_t size);

uint16_t uart_bytes_available_for_write(USART_TypeDef* uart);
void uart_write_byte(USART_TypeDef* uart, uint8_t val);
uint16_t uart_write_string(USART_TypeDef* uart, char* str);
void uart_write_bytes(USART_TypeDef* uart, uint8_t* src, uint16_t num_bytes);


#endif /* USER_UART_H_ */
