#include "ch32v30x.h"
#include "debug.h"
#include "fifo.h"
#include "ch32v307_uart.h"

#if(DEBUG == DEBUG_CDC)
#include "ch32v203_usbd_cdc.h"
#endif


__attribute__((used)) int _write(int fd, char* buf, int size)
{
#if(DEBUG == DEBUG_UART1)
	uart_write_bytes(USART1, uart1_tx_fifo, (uint8_t*)buf, (uint16_t)size);
#elif(DEBUG == DEBUG_UART2)
	uart_write_bytes(USART2, uart2_tx_fifo, (uint8_t*)buf, (uint16_t)size);
#elif(DEBUG == DEBUG_UART3)
	uart_write_bytes(USART3, uart3_tx_fifo, (uint8_t*)buf, (uint16_t)size);
#elif(DEBUG == DEBUG_UART4)
	uart_write_bytes(UART4, uart4_tx_fifo, (uint8_t*)buf, (uint16_t)size);
#elif(DEBUG == DEBUG_UART5)
	uart_write_bytes(UART5, uart5_tx_fifo, (uint8_t*)buf, (uint16_t)size);
#elif(DEBUG == DEBUG_UART6)
	uart_write_bytes(UART6, uart6_tx_fifo, (uint8_t*)buf, (uint16_t)size);
#elif(DEBUG == DEBUG_UART7)
	uart_write_bytes(UART7, uart7_tx_fifo, (uint8_t*)buf, (uint16_t)size);
#elif(DEBUG == DEBUG_UART8)
	uart_write_bytes(UART8, uart8_tx_fifo, (uint8_t*)buf, (uint16_t)size);
#elif(DEBUG == DEBUG_CDC)
	cdc_write_bytes((uint8_t*)buf, (uint16_t)size);
#endif
	return size;
}

/* Change the spatial position of data segment. */
__attribute__((used)) void* _sbrk(ptrdiff_t incr)
{
	extern char _end[];
	extern char _heap_end[];
	static char *curbrk = _end;

	if((curbrk + incr < _end) || (curbrk + incr > _heap_end))
		return NULL - 1;

	curbrk += incr;
	return curbrk - incr;
}
