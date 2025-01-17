#ifndef _CH552_UART_H_
#define _CH552_UART_H_

// HINT: user options
#define USE_UART_0 1
#define USE_UART_1 0

//buffer sizes must be powers of 2
#define UART_TX_BUF_SIZE 64
#define UART_RX_BUF_SIZE 64

// HINT: do not change any defines below this line
#define UART_0 0
#define UART_1 1

//pins in RXD TXD order
#define UART_0_P30_P31 0x00
#define UART_0_P12_P13 0x10
#define UART_1_P16_P17 0x00
#define UART_1_P34_P32 0x20

#define TIMER_0 0
#define TIMER_1 1
#define TIMER_2 2

//Read
#if USE_UART_0
void uart0_init(UINT8 uart0_tim, UINT32 baud, UINT8 pin_map);
#endif
#if USE_UART_1
void uart1_init(UINT32 baud, UINT8 pin_map);
#endif
UINT16 uart_bytes_available(UINT8 uart);
UINT8 uart_peek(UINT8 uart);
UINT8 uart_read_byte(UINT8 uart);
void uart_read_bytes(UINT8 uart, UINT8* dest, UINT16 num_bytes);
UINT16 uart_read_bytes_until(UINT8 uart, UINT8 terminator, UINT8* dest, UINT16 num_bytes);
UINT16 uart_get_string(UINT8 uart, char* buf, UINT16 buf_size);
//Write
UINT16 uart_bytes_available_for_write(UINT8 uart);
void uart_write_byte(UINT8 uart, UINT8 val);
UINT16 uart_write_string(UINT8 uart, char* str);
void uart_write_bytes(UINT8 uart, UINT8* src, UINT16 num_bytes);

#endif
