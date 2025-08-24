#include "stm32f205xx.h"
#include "debug.h"
#include "fifo.h"
#include "stm32f205_uart.h"
#include <errno.h>

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
	uart_write_bytes(USART6, uart6_tx_fifo, (uint8_t*)buf, (uint16_t)size);
#elif(DEBUG == DEBUG_CDC)
	cdc_write_bytes((uint8_t*)buf, (uint16_t)size);
#endif
	return size;
}

/**
 * Pointer to the current high watermark of the heap usage
 */
static uint8_t *__sbrk_heap_end = NULL;

/**
 * @brief _sbrk() allocates memory to the newlib heap and is used by malloc
 *        and others from the C library
 *
 * @verbatim
 * ############################################################################
 * #  .data  #  .bss  #       newlib heap       #          MSP stack          #
 * #         #        #                         # Reserved by _Min_Stack_Size #
 * ############################################################################
 * ^-- RAM start      ^-- _end                             _estack, RAM end --^
 * @endverbatim
 *
 * This implementation starts allocating at the '_end' linker symbol
 * The '_Min_Stack_Size' linker symbol reserves a memory for the MSP stack
 * The implementation considers '_estack' linker symbol to be RAM end
 * NOTE: If the MSP stack, at any point during execution, grows larger than the
 * reserved size, please increase the '_Min_Stack_Size'.
 *
 * @param incr Memory size
 * @return Pointer to allocated memory
 */
void *_sbrk(ptrdiff_t incr)
{
  extern uint8_t _end; /* Symbol defined in the linker script */
  extern uint8_t _estack; /* Symbol defined in the linker script */
  extern uint32_t _Min_Stack_Size; /* Symbol defined in the linker script */
  const uint32_t stack_limit = (uint32_t)&_estack - (uint32_t)&_Min_Stack_Size;
  const uint8_t *max_heap = (uint8_t *)stack_limit;
  uint8_t *prev_heap_end;

  /* Initialize heap end at first call */
  if (NULL == __sbrk_heap_end)
  {
    __sbrk_heap_end = &_end;
  }

  /* Protect heap from growing into the reserved MSP stack */
  if (__sbrk_heap_end + incr > max_heap)
  {
    errno = ENOMEM;
    return (void *)-1;
  }

  prev_heap_end = __sbrk_heap_end;
  __sbrk_heap_end += incr;

  return (void *)prev_heap_end;
}
