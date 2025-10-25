/*
 * ch32v203_sd_card.h
 *
 *  Created on: Oct 5, 2025
 *      Author: dragomir
 */

#ifndef _CH32V203_SD_CARD_H_
#define _CH32V203_SD_CARD_H_

//HINT: The user must provide a 32-bit time counter with a reasonable unit (100us to 1ms).
// The time counter must be declared by the user (or other library), and the name of it must be specified here.
// An extern definition for the name specified here will be created in ch32v203_sd_card.c.
// It is suggested to use an overflow counter implemented by the ch32v203_timer library.
#define sd_card_time_var	timer1_overflow_count

//HINT: SD_CARD_TIMEOUT is in units of the time variable defined above
#define SD_CARD_EN_READ_BUSY_CHECK	1
#define SD_CARD_EN_WRITE_BUSY_CHECK	1
#define SD_CARD_TIMEOUT				500
#define SD_CARD_INIT_SPI_CLK_DIV	SPI_CLK_DIV_256
#define SD_CARD_RUN_SPI_CLK_DIV		SPI_CLK_DIV_32
#define SD_CARD_SPI_MODULE			SPI1
#define SD_CARD_SPI_NCS_PORT		GPIOA
#define SD_CARD_SPI_NCS_PIN			GPIO_PIN_4

#define SD_CARD_R1_IDLE			0x01
#define SD_CARD_R1_ERASE_RESET	0x02
#define SD_CARD_R1_ILLEGAL_CMD	0x04
#define SD_CARD_R1_CRC_ERROR	0x08
#define SD_CARD_R1_ERASE_ERROR	0x10
#define SD_CARD_R1_ADDR_ERROR	0x20
#define SD_CARD_R1_PARAM_ERROR	0x40

#define SD_CARD_TYP_SD10	0x01
#define SD_CARD_TYP_SD20	0x02
#define SD_CARD_TYP_MMC		0x04
#define SD_CARD_TYP_SDSC	0x08
#define SD_CARD_TYP_SDHC	0x10

#define SD_CARD_STAT_CMD1_FAIL	0x0001
#define SD_CARD_STAT_CMD9_FAIL	0x0002
#define SD_CARD_STAT_CMD16_FAIL	0x0004
#define SD_CARD_STAT_CMD17_FAIL	0x0008
#define SD_CARD_STAT_CMD24_FAIL	0x0010
#define SD_CARD_STAT_CMD41_FAIL	0x0020
#define SD_CARD_STAT_CMD58_FAIL	0x0040
#define SD_CARD_STAT_READ_FAIL	0x0080
#define SD_CARD_STAT_WRITE_FAIL	0x0100
#define SD_CARD_STAT_INIT_FAIL	0x0200
#define SD_CARD_STAT_GNB_FAIL	0x0400
#define SD_CARD_STAT_TIMEOUT	0x0800
#define SD_CARD_STAT_CARD_BUSY	0x1000

extern uint8_t sd_card_type;
extern uint16_t sd_card_status;

#define sd_card_cs_set() gpio_set_pin(SD_CARD_SPI_NCS_PORT, SD_CARD_SPI_NCS_PIN)
#define sd_card_cs_clear() gpio_clear_pin(SD_CARD_SPI_NCS_PORT, SD_CARD_SPI_NCS_PIN)

void sd_card_init();
uint8_t sd_card_send_command(uint8_t command, uint32_t value, uint8_t resp_size);
uint8_t sd_card_calc_crc7(uint8_t* datagram, uint8_t len);
void sd_card_read_block(uint32_t block, uint8_t* dest);
void sd_card_write_block(uint32_t block, uint8_t* source);
uint32_t sd_card_get_num_blocks(void);
uint8_t sd_card_wait_not_busy(void);
uint8_t sd_card_is_busy(void);

#endif /* _CH32V203_SD_CARD_H_ */
