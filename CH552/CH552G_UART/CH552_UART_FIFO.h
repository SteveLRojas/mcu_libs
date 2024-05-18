#ifndef _CH552_UART_FIFO_H_
#define _CH552_UART_FIFO_H_

#define USE_UART_0 1
#define USE_UART_1 0

//buffer sizes must be powers of 2
#define UART_TX_BUF_SIZE 64
#define UART_RX_BUF_SIZE 64

extern volatile UINT8* uart_rx_bufs[2];
extern volatile UINT16 uart_rx_counts[2];
extern volatile UINT16 uart_rx_fronts[2];
extern volatile UINT8* uart_tx_bufs[2];
extern volatile UINT16 uart_tx_counts[2];
extern volatile UINT16 uart_tx_fronts[2];

#define uart_rx_num_used(uart_idx) (uart_rx_counts[(uart_idx)])
#define uart_rx_num_free(uart_idx) (UART_RX_BUF_SIZE - uart_rx_counts[(uart_idx)])
#define uart_rx_empty(uart_idx) (uart_rx_counts[(uart_idx)] == 0)
#define uart_rx_full(uart_idx) (uart_rx_counts[(uart_idx)] >= UART_RX_BUF_SIZE)
#define uart_rx_peek(uart_idx) (uart_rx_bufs[(uart_idx)][uart_rx_fronts[(uart_idx)]])

//UINT16 uart_rx_num_used(UINT8 uart_idx);
//UINT16 uart_rx_num_free(UINT8 uart_idx);
//UINT8 uart_rx_empty(UINT8 uart_idx);
//UINT8 uart_rx_full(UINT8 uart_idx);
UINT8 uart_rx_push(UINT8 uart_idx, UINT8 val) reentrant;
UINT8 uart_rx_pop(UINT8 uart_idx);
//UINT8 uart_rx_peek(UINT8 uart_idx);
UINT8 uart_rx_read(UINT8 uart_idx, UINT8* dest, UINT16 num_bytes);
UINT8 uart_rx_write(UINT8 uart_idx, UINT8* src, UINT16 num_bytes);

#define uart_tx_num_used(uart_idx) (uart_tx_counts[(uart_idx)])
#define uart_tx_num_free(uart_idx) (UART_TX_BUF_SIZE - uart_tx_counts[(uart_idx)])
#define uart_tx_empty(uart_idx) (uart_tx_counts[(uart_idx)] == 0)
#define uart_tx_full(uart_idx) (uart_tx_counts[(uart_idx)] >= UART_TX_BUF_SIZE)
#define uart_tx_peek(uart_idx) (uart_tx_bufs[(uart_idx)][uart_tx_fronts[(uart_idx)]])

//UINT16 uart_tx_num_used(UINT8 uart_idx);
//UINT16 uart_tx_num_free(UINT8 uart_idx);
//UINT8 uart_tx_empty(UINT8 uart_idx);
//UINT8 uart_tx_full(UINT8 uart_idx);
UINT8 uart_tx_push(UINT8 uart_idx, UINT8 val);
UINT8 uart_tx_pop(UINT8 uart_idx) reentrant;
//UINT8 uart_tx_peek(UINT8 uart_idx);
UINT8 uart_tx_read(UINT8 uart_idx, UINT8* dest, UINT16 num_bytes);
UINT8 uart_tx_write(UINT8 uart_idx, UINT8* src, UINT16 num_bytes);

#endif
