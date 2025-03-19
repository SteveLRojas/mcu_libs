/*
 * ch32v203_usbd.h
 *
 *  Created on: Jul 23, 2024
 *      Author: Steve
 */

#ifndef USER_LIBS_CH32V203_USBD_H_
#define USER_LIBS_CH32V203_USBD_H_

//HINT: The PMA contains the buffer description table and the endpoint buffers. It is a 16-bit memory connected to a 32-bit bus.
//Odd numbered 16-bit words are not implemented and must not be read or written, but are still counted for address calculation.
//Because the PMA is a 16-bit memory, writing single bytes to it is not possible. All PMA reads and writes must be done as 16-bit operations.
//Writing to the odd numbered words will write to the even numbered word at the even address below. Attempting to write single bytes writes to both bytes in the word.
//PMA access is slower than memory access, avoid frequent access to the PMA.

typedef struct _USBD_ENDPOINT_ENTRY
{
	volatile uint16_t EPR;
	uint16_t      RESERVED;
} usbd_endpoint_entry_t;

typedef struct _USBD_COMMON
{
	volatile uint16_t CNTR;
	uint16_t      RESERVED0;
	volatile uint16_t ISTR;
	uint16_t      RESERVED1;
	volatile uint16_t FNR;
	uint16_t      RESERVED2;
	volatile uint16_t DADDR;
	uint16_t      RESERVED3;
	volatile uint16_t BTABLE;
	uint16_t      RESERVED4;
} usbd_common_t;

typedef struct _USBD_BTABLE_ENTRY
{
	volatile uint16_t ADDR_TX;
	uint16_t RESERVED0;
	volatile uint16_t COUNT_TX;
	uint16_t RESERVED1;
	volatile uint16_t ADDR_RX;
	uint16_t RESERVED2;
	volatile uint16_t COUNT_RX;
	uint16_t RESERVED3;
} usbd_btable_entry_t;


/********************  Bit definition for USBD_CNTR register  ********************/
#define USBD_FRES		((uint16_t)0x0001)
#define USBD_PDWN		((uint16_t)0x0002)
#define USBD_LPMODE		((uint16_t)0x0004)
#define USBD_FSUSP		((uint16_t)0x0008)
#define USBD_RESUME		((uint16_t)0x0010)
#define USBD_ESOFM		((uint16_t)0x0100)
#define USBD_SOFM		((uint16_t)0x0200)
#define USBD_RESETM		((uint16_t)0x0400)
#define USBD_SUSPM		((uint16_t)0x0800)
#define USBD_WKUPM		((uint16_t)0x1000)
#define USBD_ERRM		((uint16_t)0x2000)
#define USBD_PMAOVRM	((uint16_t)0x4000)
#define USBD_CTRM		((uint16_t)0x8000)

/********************  Bit definition for USBD_ISTR register  ********************/
#define USBD_EP_ID		((uint16_t)0x000F)
#define USBD_DIR		((uint16_t)0x0010)
#define USBD_ESOF		((uint16_t)0x0100)
#define USBD_SOF		((uint16_t)0x0200)
#define USBD_RESET		((uint16_t)0x0400)
#define USBD_SUSP		((uint16_t)0x0800)
#define USBD_WKUP		((uint16_t)0x1000)
#define USBD_ERR		((uint16_t)0x2000)
#define USBD_PMAOVR		((uint16_t)0x4000)
#define USBD_CTR		((uint16_t)0x8000)

/********************  Bit definition for USBD_FNR register  ********************/
#define USBD_FN			((uint16_t)0x07FF)
#define USBD_LSOF		((uint16_t)0x1800)
#define USBD_LCK		((uint16_t)0x2000)
#define USBD_RXDM		((uint16_t)0x4000)
#define USBD_RXDP		((uint16_t)0x8000)

/********************  Bit definition for USBD_DADDR register  ********************/
#define USBD_ADD		((uint16_t)0x007F)
#define USBD_EF			((uint16_t)0x0080)

/********************  Bit definition for USBD_EPRx register  ********************/
#define USBD_EA			((uint16_t)0x000F)
#define USBD_STAT_TX	((uint16_t)0x0030)
#define USBD_DTOG_TX	((uint16_t)0x0040)
#define USBD_CTR_TX		((uint16_t)0x0080)
#define USBD_EP_KIND	((uint16_t)0x0100)
#define USBD_EPTYPE		((uint16_t)0x0600)
#define USBD_SETUP		((uint16_t)0x0800)
#define USBD_STAT_RX	((uint16_t)0x3000)
#define USBD_DTOG_RX	((uint16_t)0x4000)
#define USBD_CTR_RX		((uint16_t)0x8000)

#define USBD_BTABLE_ENTRY_SIZE 8
#define USBD_BTABLE_SIZE (8 * USBD_BTABLE_ENTRY_SIZE)
#define USBD_BTABLE_ENTRY_SIZE_AC 16
#define USBD_BTABLE_SIZE_AC (8 * USBD_BTABLE_ENTRY_SIZE_AC)

#define USBD_ENDPOINT_BASE	0x40005C00
#define USBD_COMMON_BASE	0x40005C40
#define USBD_PMA_BASE		0x40006000
#define USBD_BUFFER_BASE	(USBD_PMA_BASE + USBD_BTABLE_SIZE_AC)

#define USBD_ENDPOINT	((usbd_endpoint_entry_t*)USBD_ENDPOINT_BASE)
#define USBD_COMMON		((usbd_common_t*)USBD_COMMON_BASE)
#define USBD_BTABLE		((usbd_btable_entry_t*)USBD_PMA_BASE)

//HINT: User Interface begins below this line.
#define EP_0 0
#define EP_1 1
#define EP_2 2
#define EP_3 3
#define EP_4 4
#define EP_5 5
#define EP_6 6
#define EP_7 7

#define USBD_INT_TRANSFER	0x8000
#define USBD_INT_WAKEUP		0x1000
#define USBD_INT_SUSPEND	0x0800
#define USBD_INT_RESET		0x0400
#define USBD_INT_SOF		0x0200
#define USBD_INT_ESOF		0x0100

#define USBD_EP_TYPE_BULK		0x0000
#define USBD_EP_TYPE_BULK_DBL	0x0100
#define USBD_EP_TYPE_CONTROL	0x0200
#define USBD_EP_TYPE_ISO		0x0400
#define USBD_EP_TYPE_INTERRUPT	0x0600

#define USBD_OUT_TOG_0	0x0000
#define USBD_OUT_TOG_1	0x4000
#define USBD_IN_TOG_0	0x0000
#define USBD_IN_TOG_1	0x0040

#define USBD_OUT_RES_DISABLED	0x0000
#define USBD_OUT_RES_STALL		0x1000
#define USBD_OUT_RES_NAK		0x2000
#define USBD_OUT_RES_ACK		0x3000
#define USBD_IN_RES_DISABLED	0x0000
#define USBD_IN_RES_STALL		0x0010
#define USBD_IN_RES_NAK			0x0020
#define USBD_IN_RES_ACK			0x0030

typedef struct _USBD_CONFIG
{
	uint16_t tx_buf_size[8];
	uint16_t rx_buf_size[8];
	uint16_t ep_type[8];
	uint16_t int_en;
} usbd_config_t;

extern void (*usbd_sof_callback)(void);
extern void (*usbd_esof_callback)(void);
extern void (*usbd_out_callback)(uint8_t);
extern void (*usbd_in_callback)(uint8_t);
extern void (*usbd_setup_callback)(uint8_t);
extern void (*usbd_reset_callback)(void);
extern void (*usbd_wakeup_callback)(void);
extern void (*usbd_suspend_callback)(void);
extern volatile uint16_t sof_count;

#define usbd_enable_interrupts(interrupts) (USBD_COMMON->CNTR |= (interrupts))
#define usbd_disable_interrupts(interrupts) (USBD_COMMON->CNTR &= ~(interrupts))
#define usbd_set_addr(addr) (USBD_COMMON->DADDR = (USBD_COMMON->DADDR & ~USBD_ADD) | (addr))

#define usbd_set_tx_len(ep, len) (USBD_BTABLE[ep].COUNT_TX = (len))
#define usbd_set_tx_0_len(ep, len) (USBD_BTABLE[ep].COUNT_TX = (len))
#define usbd_set_tx_1_len(ep, len) (USBD_BTABLE[ep].COUNT_RX = (len))
#define usbd_get_rx_len(ep) (USBD_BTABLE[ep].COUNT_RX & 0x03FF)
#define usbd_get_rx_0_len(ep) (USBD_BTABLE[ep].COUNT_TX & 0x03FF)
#define usbd_get_rx_1_len(ep) (USBD_BTABLE[ep].COUNT_RX & 0x03FF)

#define usbd_set_out_toggle(ep, tog) (USBD_ENDPOINT[ep].EPR = (USBD_ENDPOINT[ep].EPR & ~(USBD_STAT_RX | USBD_DTOG_TX | USBD_STAT_TX)) ^ (tog))
#define usbd_set_in_toggle(ep, tog) (USBD_ENDPOINT[ep].EPR = (USBD_ENDPOINT[ep].EPR & ~(USBD_DTOG_RX | USBD_STAT_RX | USBD_STAT_TX)) ^ (tog))
#define usbd_get_out_toggle(ep) (USBD_ENDPOINT[ep].EPR & USBD_DTOG_RX)
#define usbd_get_in_toggle(ep) (USBD_ENDPOINT[ep].EPR & USBD_DTOG_TX)

#define usbd_set_out_res(ep, res) (USBD_ENDPOINT[ep].EPR = (USBD_ENDPOINT[ep].EPR & ~(USBD_DTOG_RX | USBD_DTOG_TX | USBD_STAT_TX)) ^ (res))
#define usbd_set_in_res(ep, res) (USBD_ENDPOINT[ep].EPR = (USBD_ENDPOINT[ep].EPR & ~(USBD_DTOG_RX | USBD_STAT_RX | USBD_DTOG_TX)) ^ (res))
#define usbd_get_out_res(ep) (USBD_ENDPOINT[ep].EPR & USBD_STAT_RX)
#define usbd_get_in_res(ep) (USBD_ENDPOINT[ep].EPR & USBD_STAT_TX)

#define usbd_set_tog_res(ep, tog_res) (USBD_ENDPOINT[ep].EPR ^= (tog_res))

//HINT: usbd_read_pma_word and usbd_write_pma_word take an offset in bytes (the offset must be an even number).
#define usbd_read_pma_word(offset) (*((uint16_t*)(USBD_PMA_BASE + ((offset) << 1))))
#define usbd_write_pma_word(offset, data) (*((uint16_t*)(USBD_PMA_BASE + ((offset) << 1))) = (data))
#define usbd_get_tx_buf_offset(ep) (USBD_BTABLE[ep].ADDR_TX)
#define usbd_get_tx_0_buf_offset(ep) (USBD_BTABLE[ep].ADDR_TX)
#define usbd_get_tx_1_buf_offset(ep) (USBD_BTABLE[ep].ADDR_RX)
#define usbd_get_rx_buf_offset(ep) (USBD_BTABLE[ep].ADDR_RX)
#define usbd_get_rx_0_buf_offset(ep) (USBD_BTABLE[ep].ADDR_TX)
#define usbd_get_rx_1_buf_offset(ep) (USBD_BTABLE[ep].ADDR_RX)

void usbd_init(const usbd_config_t* usbd_config);
void usbd_disable(void);
void usbd_write_to_pma(uint16_t offset, const uint16_t* source, uint16_t num_words);
void usbd_read_from_pma(uint16_t offset, uint16_t* dest, uint16_t num_words);
void usbd_init_pma_val(uint16_t offset, uint16_t val, uint16_t num_words);

#endif /* USER_LIBS_CH32V203_USBD_H_ */
