/*
 * support.c
 *
 *  Created on: Dec 9, 2023
 *      Author: Steve
 */
#include "debug.h"
#include "ch32v20x.h"
#include "support.h"

#if SUPPORT_USE_UART
#include "fifo.h"
#include "ch32v203_uart.h"
#define SUPPORT_BYTES_AVAILABLE() uart_bytes_available(SUPPORT_UART_RX_FIFO)
#define SUPPORT_PEEK() uart_peek(SUPPORT_UART_RX_FIFO)
#define SUPPORT_READ_BYTE() uart_read_byte(SUPPORT_UART, SUPPORT_UART_RX_FIFO)
#define SUPPORT_READ_BYTES(dest, num_bytes) uart_read_bytes(SUPPORT_UART, SUPPORT_UART_RX_FIFO, dest, num_bytes)
#define SUPPORT_READ_BYTES_UNTIL(terminator, dest, num_bytes) uart_read_bytes_until(SUPPORT_UART, SUPPORT_UART_RX_FIFO, terminator, dest, num_bytes)
#define SUPPORT_GET_STRING(buf, buf_size) uart_get_string(SUPPORT_UART, SUPPORT_UART_RX_FIFO, buf, buf_size)
#define SUPPORT_BYTES_AVAILABLE_FOR_WRITE() uart_bytes_available_for_write(SUPPORT_UART_TX_FIFO)
#define SUPPORT_WRITE_BYTE(val) uart_write_byte(SUPPORT_UART, SUPPORT_UART_TX_FIFO, val)
#define SUPPORT_WRITE_STRING(str) uart_write_string(SUPPORT_UART, SUPPORT_UART_TX_FIFO, str)
#define SUPPORT_WRITE_BYTES(src, num_bytes) uart_write_bytes(SUPPORT_UART, SUPPORT_UART_TX_FIFO, src, num_bytes)
#elif SUPPORT_USE_USBD_CDC
#include "ch32v203_usbd_cdc.h"
#define SUPPORT_BYTES_AVAILABLE() cdc_bytes_available()
#define SUPPORT_PEEK() cdc_peek()
#define SUPPORT_READ_BYTE() cdc_read_byte()
#define SUPPORT_READ_BYTES(dest, num_bytes) cdc_read_bytes(dest, num_bytes)
#define SUPPORT_BYTES_AVAILABLE_FOR_WRITE() cdc_bytes_available_for_write()
#define SUPPORT_WRITE_BYTE(val) cdc_write_byte(val)
#define SUPPORT_WRITE_STRING(str) cdc_write_string(str)
#define SUPPORT_WRITE_BYTES(src, num_bytes) cdc_write_bytes(src, num_bytes)
#elif SUPPORT_USE_USBFSD_CDC
#include "ch32v203_usbfsd_cdc.h"
#define SUPPORT_BYTES_AVAILABLE() cdc_bytes_available()
#define SUPPORT_PEEK() cdc_peek()
#define SUPPORT_READ_BYTE() cdc_read_byte()
#define SUPPORT_READ_BYTES(dest, num_bytes) cdc_read_bytes(dest, num_bytes)
#define SUPPORT_BYTES_AVAILABLE_FOR_WRITE() cdc_bytes_available_for_write()
#define SUPPORT_WRITE_BYTE(val) cdc_write_byte(val)
//TODO: write_string function
#define SUPPORT_WRITE_BYTES(src, num_bytes) cdc_write_bytes(src, num_bytes)
#elif SUPPORT_USE_UART_DMA
#include "ch32v203_uart_dma.h"
#define SUPPORT_BYTES_AVAILABLE() uart_dma_bytes_available(SUPPORT_UART_DMA)
#define SUPPORT_PEEK() uart_dma_peek(SUPPORT_UART_DMA)
#define SUPPORT_READ_BYTE() uart_dma_read_byte(SUPPORT_UART_DMA)
#define SUPPORT_READ_BYTES(dest, num_bytes) uart_dma_read_bytes(SUPPORT_UART_DMA, dest, num_bytes)
#define SUPPORT_READ_BYTES_UNTIL(terminator, des, num_bytes) uart_dma_read_bytes_until(SUPPORT_UART_DMA, terminator, dest, num_bytes)
#define SUPPORT_GET_STRING(buf, buf_size) uart_dma_get_string(SUPPORT_UART_DMA, dest, buf_size)
#define SUPPORT_BYTES_AVAILABLE_FOR_WRITE() uart_dma_bytes_available_for_write(SUPPORT_UART_DMA)
#define SUPPORT_WRITE_BYTE(val) uart_dma_write_byte(SUPPORT_UART_DMA, val)
#define SUPPORT_WRITE_STRING(str) uart_dma_write_string(SUPPORT_UART_DMA, str)
#define SUPPORT_WRITE_BYTES(src, num_bytes) uart_dma_write_bytes(SUPPORT_UART_DMA, src, num_bytes)
#endif

#if SUPPORT_USE_PSEUDO_RANDOM
#include "pseudo_random.h"
#endif

static const char hex_table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};

void support_byte_to_hex(uint8_t value, char* buff)
{

	buff[0] = hex_table[(value >> 4) & 0x0f];
	buff[1] = hex_table[(value) & 0x0f];
	buff[2] = '\0';
}

void support_byte_to_hex_ln(uint8_t value, char* buff)
{

	buff[0] = hex_table[(value >> 4) & 0x0f];
	buff[1] = hex_table[(value) & 0x0f];
	buff[2] = '\n';
	buff[3] = '\0';
}

void support_word_to_hex(uint16_t value, char* buff)
{
	support_byte_to_hex(value >> 8, buff);
	support_byte_to_hex(value & 0xFF, buff + 2);
}

void support_word_to_hex_ln(uint16_t value, char* buff)
{
	support_byte_to_hex(value >> 8, buff);
	support_byte_to_hex_ln(value & 0xFF, buff + 2);
}

void support_print_bytes(uint8_t* src, uint16_t num_bytes)
{
	uint8_t count = 0;
	uint8_t value;
	while(num_bytes)
	{
		value = *src;
		while(SUPPORT_BYTES_AVAILABLE_FOR_WRITE() < 3);
		SUPPORT_WRITE_BYTE(hex_table[(value >> 4) & 0x0f]);
		SUPPORT_WRITE_BYTE(hex_table[(value) & 0x0f]);
		--num_bytes;
		++count;
		++src;
		if(!(count & 0x0F))
		{
			SUPPORT_WRITE_BYTE('\n');
		}
	}
}

void support_print_hex_byte(uint8_t value)
{
	char buff[3];
	support_byte_to_hex_ln(value, buff);
	SUPPORT_WRITE_STRING(buff);
}

void support_print_hex_word(uint16_t value)
{
	char buff[5];
	support_word_to_hex_ln(value, buff);
	SUPPORT_WRITE_STRING(buff);
}

#if SUPPORT_USE_PSEUDO_RANDOM
void support_print_test_data()
{
	char buff[5];
	uint16_t counter = 0;
	uint16_t current_word;
	pseudo_random_seed(0);
	do
	{
		pseudo_random_generate(16);
		current_word = pseudo_random_get_word();
		support_word_to_hex(current_word, buff);
		SUPPORT_WRITE_STRING(buff);
		if((counter & 0x0f) == 0x0f)
			SUPPORT_WRITE_STRING("\n");
		++counter;
	} while(counter != 0);
}
#endif

#ifdef SUPPORT_GET_STRING
uint16_t support_get_hex()
{
	uint8_t hexbuf[6];  //leave space for pesky CR LF
	uint8_t temp;
	uint16_t value = 0;

	for(uint8_t d = 0; d < 6; ++d)
	{
		hexbuf[d] = 0;
	}

	SUPPORT_GET_STRING(hexbuf, 6);

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
#endif

void support_to_caps(char* str)
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
