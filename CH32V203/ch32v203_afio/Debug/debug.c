#include "ch32v20x.h"
#include "ch32v203_core.h"
#include "ch32v203_rcc.h"
#include "debug.h"

#if(DEBUG_TYP == DEBUG_TYP_CDC_USBD)
#include "ch32v203_usbd_cdc.h"
#elif(DEBUG_TYP == DEBUG_TYP_UART_INT)
#include "fifo.h"
#include "ch32v203_uart.h"
#elif((DEBUG_TYP == DEBUG_TYP_UART_DMA) || (DEBUG_TYP == DEBUG_TYP_UART_DBG))
#include "ch32v203_uart_dma.h"
#endif


__attribute__((used))
int _write(int fd, char* buf, int size)
{
#if(DEBUG_TYP == DEBUG_TYP_UART_INT)
	uart_write_bytes(DEBUG_UART, DEBUG_TX_FIFO, (uint8_t*)buf, (uint16_t)size);
#elif(DEBUG_TYP == DEBUG_TYP_UART_DMA)
	uart_dma_write_bytes(DEBUG_UART_DMA, (uint8_t*)buf, (uint16_t)size);
#elif(DEBUG_TYP == DEBUG_TYP_UART_DBG)
	uart_dma_debug_write(DEBUG_UART, (uint8_t*)buf, (uint16_t)size);
#elif(DEBUG_TYP == DEBUG_TYP_CDC_USBD)
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
