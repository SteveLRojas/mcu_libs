#include "ch32v20x.h"
#include "ch32v203_core.h"
#include "ch32v203_rcc.h"
#include "debug.h"

#if(DEBUG == DEBUG_CDC)
#include "ch32v203_usbd_cdc.h"
#elif DEBUG_UART_TYP == DEBUG_UART_TYP_INTERRUPT
#include "fifo.h"
#include "ch32v203_uart.h"
#elif DEBUG_UART_TYP == DEBUG_UART_TYP_DMA
#include "ch32v203_uart_dma.h"
#endif


__attribute__((used))
int _write(int fd, char* buf, int size)
{
#if(DEBUG == DEBUG_UART_1)
	uart_write_bytes(USART1, uart1_tx_fifo, (uint8_t*)buf, (uint16_t)size);
#elif(DEBUG == DEBUG_UART_2)
	uart_write_bytes(USART2, uart2_tx_fifo, (uint8_t*)buf, (uint16_t)size);
#elif(DEBUG == DEBUG_UART_3)
	uart_write_bytes(USART3, uart3_tx_fifo, (uint8_t*)buf, (uint16_t)size);
#elif(DEBUG == DEBUG_UART_4)
	uart_write_bytes(USART4, uart4_tx_fifo, (uint8_t*)buf, (uint16_t)size);
#elif(DEBUG == DEBUG_UART_DMA_1)
	uart_write_bytes(uart_dma_1, (uint8_t*)buf, (uint16_t)size);
#elif(DEBUG == DEBUG_UART_DMA_2)
	uart_write_bytes(uart_dma_2, (uint8_t*)buf, (uint16_t)size);
#elif(DEBUG == DEBUG_UART_DMA_3)
	uart_write_bytes(uart_dma_3, (uint8_t*)buf, (uint16_t)size);
#elif(DEBUG == DEBUG_UART_DMA_4)
	uart_write_bytes(uart_dma_4, (uint8_t*)buf, (uint16_t)size);
#elif(DEBUG == DEBUG_CDC)
	cdc_write_bytes((uint8_t*)buf, (uint16_t)size);
#endif
	return size;
}

// Change the spatial position of data segment. Returns data length.
__attribute__((used))
void* _sbrk(ptrdiff_t incr)
{
	extern char _end[];
	extern char _heap_end[];
	static char *curbrk = _end;

	if((curbrk + incr < _end) || (curbrk + incr > _heap_end))
		return NULL - 1;

	curbrk += incr;
	return curbrk - incr;
}
