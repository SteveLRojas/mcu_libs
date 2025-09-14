#ifndef _CH559_USB_CDC_H_
#define _CH559_USB_CDC_H_

#define CDC_TIMEOUT_MS 25

// Endpoint sizes must be a multiple of 2 in order to keep the buffers aligned. CH559 does not support odd addresses.
#define CDC_ENDP0_SIZE 8
#define CDC_ENDP1_SIZE 16
#define CDC_ENDP2_SIZE 64
#define CDC_ENDP3_SIZE 64

// CH552 writes the CRC when receiving: the buffer can be overflown by up to 2 bytes, but never past 64 bytes...
#define CDC_ENDP0_BUF_SIZE ((CDC_ENDP0_SIZE + 2) > 64) ? 64 : (CDC_ENDP0_SIZE + 2)
#define CDC_ENDP2_BUF_SIZE ((CDC_ENDP2_SIZE + 2) > 64) ? 64 : (CDC_ENDP2_SIZE + 2)

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

// Serial State defines
#define CDC_SS_OVERRUN		0x40
#define CDC_SS_PARITY		0x20
#define CDC_SS_FRAMING		0x10
#define CDC_SS_RINGSIGNAL	0x08
#define CDC_SS_BREAK		0x04
#define CDC_SS_TXCARRIER	0x02
#define CDC_SS_RXCARRIER	0x01
//HINT: TXCARRIER corresponds to the RS-232 signal DSR, and RXCARRIER to DCD.

//Control Line State define
#define CDC_CLS_RTS	0x02
#define CDC_CLS_DTR	0x01

extern volatile UINT8 cdc_config;
extern volatile cdc_line_coding_t cdc_line_coding;
extern volatile UINT8 cdc_control_line_state;

void cdc_init(void);
void cdc_set_serial_state(UINT8 val);
UINT16 cdc_bytes_available(void);
UINT8 cdc_peek(void);
UINT8 cdc_read_byte(void);
void cdc_read_bytes(UINT8* dest, UINT16 num_bytes);
UINT16 cdc_bytes_available_for_write(void);
void cdc_write_byte(UINT8 val);
void cdc_write_bytes(UINT8* src, UINT16 num_bytes);
void cdc_write_string(char* src);

#endif
