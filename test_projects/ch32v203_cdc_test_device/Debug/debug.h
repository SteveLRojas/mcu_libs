#ifndef _DEBUG_H
#define _DEBUG_H

#include "stdio.h"

// Debug interface options
#define DEBUG_TYP_UART_INT	1
#define DEBUG_TYP_UART_DMA	2
#define DEBUG_TYP_UART_DBG	3
#define DEBUG_TYP_CDC_USBD	4

//UART type options
#define DEBUG_UART		USART1
#define DEBUG_TX_FIFO	uart1_tx_fifo
#define DEBUG_UART_DMA	uart_dma_1

// Select the debug interface
#ifndef DEBUG_TYP
#define DEBUG_TYP   DEBUG_TYP_UART_DBG
#endif

#if(DEBUG)
  #define PRINT(format, ...)    printf(format, ##__VA_ARGS__)
#else
  #define PRINT(X...)
#endif

#endif
