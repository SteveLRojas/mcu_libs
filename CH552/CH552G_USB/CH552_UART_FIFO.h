#ifndef _CH552_UART_FIFO_H_
#define _CH552_UART_FIFO_H_

//buffer sizes must be powers of 2
#define UART_TX_BUF_SIZE 64
#define UART_RX_BUF_SIZE 64

UINT16 uart_rx_num_used(UINT8 uart_idx);
UINT16 uart_rx_num_free(UINT8 uart_idx);
UINT8 uart_rx_empty(UINT8 uart_idx);
UINT8 uart_rx_full(UINT8 uart_idx);
UINT8 uart_rx_push(UINT8 uart_idx, UINT8 val) reentrant;
UINT8 uart_rx_pop(UINT8 uart_idx);
UINT8 uart_rx_peek(UINT8 uart_idx);
UINT8 uart_rx_read(UINT8 uart_idx, UINT8* dest, UINT16 num_bytes);
UINT8 uart_rx_write(UINT8 uart_idx, UINT8* src, UINT16 num_bytes);

UINT16 uart_tx_num_used(UINT8 uart_idx);
UINT16 uart_tx_num_free(UINT8 uart_idx);
UINT8 uart_tx_empty(UINT8 uart_idx);
UINT8 uart_tx_full(UINT8 uart_idx);
UINT8 uart_tx_push(UINT8 uart_idx, UINT8 val);
UINT8 uart_tx_pop(UINT8 uart_idx) reentrant;
UINT8 uart_tx_peek(UINT8 uart_idx);
UINT8 uart_tx_read(UINT8 uart_idx, UINT8* dest, UINT16 num_bytes);
UINT8 uart_tx_write(UINT8 uart_idx, UINT8* src, UINT16 num_bytes);

#endif
