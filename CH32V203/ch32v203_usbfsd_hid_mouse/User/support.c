/*
 * support.c
 *
 *  Created on: Dec 9, 2023
 *      Author: Steve
 */
#include "debug.h"
#include "ch32v20x.h"
#include "fifo.h"
#include "ch32v203_uart.h"
#include "pseudo_random.h"
#include "support.h"

void byte_to_hex(uint8_t value, char* buff)
{
	static const char table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	buff[0] = table[(value >> 4) & 0x0f];
	buff[1] = table[(value) & 0x0f];
	buff[2] = '\0';
}

void word_to_hex(uint16_t value, char* buff)
{
	byte_to_hex(value >> 8, buff);
	byte_to_hex(value & 0xFF, buff + 2);
}

void print_hex_byte(uint8_t value)
{
	char buff[3];
	byte_to_hex(value, buff);
	uart_write_string(USART1, uart1_tx_fifo, buff);
	uart_write_string(USART1, uart1_tx_fifo, "\n");
}

void print_hex_word(uint16_t value)
{
	char buff[5];
	word_to_hex(value, buff);
	uart_write_string(USART1, uart1_tx_fifo, buff);
	uart_write_string(USART1, uart1_tx_fifo, "\n");
}

void print_test_data()
{
	char buff[5];
	uint16_t counter = 0;
	uint16_t current_word;
	pseudo_random_seed(0);
	do
	{
		pseudo_random_generate(16);
		current_word = pseudo_random_get_word();
		word_to_hex(current_word, buff);
		uart_write_string(USART1, uart1_tx_fifo, buff);
		if((counter & 0x0f) == 0x0f)
			uart_write_string(USART1, uart1_tx_fifo, "\n");
		++counter;
	} while(counter != 0);
}

uint16_t get_hex()
{
	uint8_t hexbuf[6];  //leave space for pesky CR LF
	uint8_t temp;
	uint16_t value = 0;

	for(uint8_t d = 0; d < 6; ++d)
	{
		hexbuf[d] = 0;
	}

	uart_get_string(USART1, uart1_rx_fifo, hexbuf, 6);

	for(uint8_t d = 0; d < 6; ++d)
	{
		temp = hexbuf[d];
		if(temp >= '0' && temp <= '9')  //convert numbers
			temp = temp - '0';
		else if(temp >= 'A' && temp <= 'F')   //convert uppercase letters
			temp = temp - 'A' + 10;
		else if(temp >= 'a' && temp <= 'f')   //convert the annoying lowercase letters
			temp = temp - 'a' + 10;
		else
			continue;

		value = value << 4;
		value = value | temp;
	}

	return value;
}

void to_caps(char* str)
{
	uint8_t d = 0;
	while(str[d])
	{
		if((str[d] >= 'a') & (str[d] <= 'z'))
			str[d] = str[d] - 0x20;
		++d;
	}
	return;
}

