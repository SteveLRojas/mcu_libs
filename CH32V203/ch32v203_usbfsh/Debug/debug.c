/********************************** (C) COPYRIGHT  *******************************
 * File Name          : debug.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : This file contains all the functions prototypes for UART
 *                      Printf , Delay functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32v20x.h"
#include "ch32v203_core.h"
#include "debug.h"
#include "fifo.h"
#include "ch32v203_uart.h"
#include "ch32v203_rcc.h"

#if(DEBUG == DEBUG_CDC)
#include "ch32v203_usbd_cdc.h"
#endif


__attribute__((used))
int _write(int fd, char* buf, int size)
{
#if(DEBUG == DEBUG_UART1)
	uart_write_bytes(USART1, uart1_tx_fifo, (uint8_t*)buf, (uint16_t)size);
#elif(DEBUG == DEBUG_UART2)
	uart_write_bytes(USART2, uart2_tx_fifo, (uint8_t*)buf, (uint16_t)size);
#elif(DEBUG == DEBUG_UART3)
	uart_write_bytes(USART3, uart3_tx_fifo, (uint8_t*)buf, (uint16_t)size);
#elif(DEBUG == DEBUG_UART4)
	uart_write_bytes(USART4, uart4_tx_fifo, (uint8_t*)buf, (uint16_t)size);
#elif(DEBUG == DEBUG_CDC)
	cdc_write_bytes((uint8_t*)buf, (uint16_t)size);
#endif
	return size;
}

/*********************************************************************
 * @fn      _sbrk
 *
 * @brief   Change the spatial position of data segment.
 *
 * @return  size: Data length
 */
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
