#ifndef _CH552_UART_H_
#define _CH552_UART_H_

#define UART_0 0
#define UART_1 1

//pins in RXD TXD order
#define UART_0_P30_P31 0x00
#define UART_0_P12_P13 0x10
#define UART_1_P16_P17 0x00
#define UART_1_P34_P32 0x20

//Read
void uart0_init(UINT8 uart0_tim, UINT32 baud, UINT8 pin_map);
void uart1_init(UINT32 baud, UINT8 pin_map);
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
