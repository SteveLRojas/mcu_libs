#ifndef _DEBUG_H
#define _DEBUG_H

#include "stdio.h"

// Debug interface options
#define DEBUG_UART_1		1
#define DEBUG_UART_2		2
#define DEBUG_UART_3		3
#define DEBUG_UART_4		4
#define DEBUG_UART_DMA_1	5
#define DEBUG_UART_DMA_2	6
#define DEBUG_UART_DMA_3	7
#define DEBUG_UART_DMA_4	8
#define DEBUG_CDC			9

//UART type options
#define DEBUG_UART_TYP_INTERRUPT	0
#define DEBUG_UART_TYP_DMA			1

// Select the debug interface
#ifndef DEBUG
#define DEBUG   DEBUG_UART_1
#endif

// Select the UART type
#define DEBUG_UART_TYP	DEBUG_UART_TYP_INTERRUPT

#if(DEBUG)
  #define PRINT(format, ...)    printf(format, ##__VA_ARGS__)
#else
  #define PRINT(X...)
#endif

#endif
