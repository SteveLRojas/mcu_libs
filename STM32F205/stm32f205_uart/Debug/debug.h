#ifndef __DEBUG_H
#define __DEBUG_H

#include "stdio.h"

/* UART Printf Definition */
#define DEBUG_UART1	1
#define DEBUG_UART2	2
#define DEBUG_UART3	3
#define DEBUG_UART4	4
#define DEBUG_UART5	5
#define DEBUG_UART6 6
#define DEBUG_CDC	7

/* DEBUG UART Definition */
#ifndef DEBUG
#define DEBUG   DEBUG_UART2
#endif

#if(DEBUG)
  #define PRINT(format, ...)    printf(format, ##__VA_ARGS__)
#else
  #define PRINT(X...)
#endif

#endif
