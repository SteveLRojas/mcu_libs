#ifndef _USB_CDC_H_
#define _USB_CDC_H_

// HINT: FIFO sizes must be powers of 2
#define CDC_RX_FIFO_SIZE 128
#define CDC_TX_FIFO_SIZE 128

#define CDC_TIMEOUT_MS 25

// Endpoint sizes must be a multiple of 2 in order to keep the buffers aligned. CH559 does not support odd addresses.
#define CDC_ENDP0_SIZE 8
#define CDC_ENDP1_SIZE 8
#define CDC_ENDP2_SIZE 64
#define CDC_ENDP3_SIZE 64

#define CDC_ENDP0_BUF_SIZE ((CDC_ENDP0_SIZE + 2) > 64) ? 64 : (CDC_ENDP0_SIZE + 2)
#define CDC_ENDP1_BUF_SIZE ((CDC_ENDP1_SIZE + 2) > 64) ? 64 : (CDC_ENDP1_SIZE + 2)
#define CDC_ENDP2_BUF_SIZE ((CDC_ENDP2_SIZE + 2) > 64) ? 64 : (CDC_ENDP2_SIZE + 2)
#define CDC_ENDP3_BUF_SIZE ((CDC_ENDP3_SIZE + 2) > 64) ? 64 : (CDC_ENDP3_SIZE + 2)

// All fields are single bytes because C51 is big endian... USB spec is little endian.
typedef struct _CDC_LINE_CODING {
	UINT8 baud_rate_ll;
	UINT8 baud_rate_lh;
	UINT8 baud_rate_hl;
	UINT8 baud_rate_hh;
	UINT8 stop_bits;
	UINT8 parity;
	UINT8 data_bits;
} cdc_line_coding_t;

extern UINT8 cdc_control_line_state;

void cdc_init(void);
//void cdc_set_serial_state(UINT8 val);
UINT16 cdc_bytes_available(void);
UINT8 cdc_peek(void);
UINT8 cdc_read_byte(void);
void cdc_read_bytes(UINT8* dest, UINT16 num_bytes);
UINT16 cdc_bytes_available_for_write(void);
void cdc_write_byte(UINT8 val);
void cdc_write_bytes(UINT8* src, UINT16 num_bytes);

#endif
