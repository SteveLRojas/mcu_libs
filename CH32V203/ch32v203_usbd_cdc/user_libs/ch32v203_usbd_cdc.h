/*
 * ch32v203_usbd_cdc.h
 *
 *  Created on: Aug 9, 2024
 *      Author: Steve
 */

#ifndef _CH32V203_USBD_CDC_H_
#define _CH32V203_USBD_CDC_H_

#define CDC_TIMEOUT_MS 25

#define CDC_ENDP0_SIZE 8
#define CDC_ENDP1_SIZE 16
#define CDC_ENDP2_SIZE 64
#define CDC_ENDP3_SIZE 64

typedef struct _CDC_LINE_CODING {
	uint32_t baud_rate;
	uint8_t stop_bits;
	uint8_t parity;
	uint8_t data_bits;
	uint8_t reserved;
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

extern volatile cdc_line_coding_t cdc_line_coding;
extern volatile uint8_t cdc_control_line_state;

void cdc_init(void);
void cdc_set_serial_state(uint8_t val);
uint16_t cdc_bytes_available(void);
uint8_t cdc_peek(void);
uint8_t cdc_read_byte(void);
void cdc_read_bytes(uint8_t* dest, uint16_t num_bytes);
uint16_t cdc_bytes_available_for_write(void);
void cdc_write_byte(uint8_t val);
void cdc_write_bytes(const uint8_t* src, uint16_t num_bytes);

#endif /* _CH32V203_USBD_CDC_H_ */
