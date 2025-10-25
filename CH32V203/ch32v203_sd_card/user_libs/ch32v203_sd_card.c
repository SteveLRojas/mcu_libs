/*
 * ch32v203_sd_card.c
 *
 *  Created on: Oct 5, 2025
 *      Author: dragomir
 */

#include "ch32v20x.h"
#include "ch32v203_gpio.h"
#include "ch32v203_spi.h"
#include "ch32v203_sd_card.h"

//TODO: implement optional CRC
//TODO: Extend the SPI library with bulk transfer functions and use those...
extern volatile uint32_t sd_card_time_var;	//HINT: name defined in ch32v203_sd_card.h
uint32_t sd_card_response;
uint8_t sd_card_type;
uint16_t sd_card_status;

void sd_card_init()
{
	uint32_t initial_time;
	uint8_t status;
	spi_init(SD_CARD_SPI_MODULE, SPI_8_BIT | SD_CARD_INIT_SPI_CLK_DIV | SPI_MODE_0);
	gpio_set_pin(SD_CARD_SPI_NCS_PORT, SD_CARD_SPI_NCS_PIN);

	for(uint8_t d = 0; d < 10; ++d)
	{
		spi_transfer(SD_CARD_SPI_MODULE, 0x00FF);
	}

	// send CMD0 to make the thing go idle in SPI mode
	initial_time = sd_card_time_var;
	while((sd_card_time_var - initial_time) <= SD_CARD_TIMEOUT)
	{
		sd_card_cs_clear();
		status = sd_card_send_command(0, 0, 1);
		sd_card_cs_set();
		spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
		if(status == SD_CARD_R1_IDLE)
			break;
	}
	spi_transfer(SD_CARD_SPI_MODULE, 0xFF);

	//is it SD v2.0?
	sd_card_type = 0;
	sd_card_status = 0;
	sd_card_cs_clear();
	status = sd_card_send_command(8, 0x1AA, 5);
	sd_card_cs_set();
	spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
	if(!(status & SD_CARD_R1_ILLEGAL_CMD))
	{
		sd_card_type |= SD_CARD_TYP_SD20;
		initial_time = sd_card_time_var;
		while((sd_card_time_var - initial_time) <= SD_CARD_TIMEOUT)
		{
			sd_card_cs_clear();
			status = sd_card_send_command(55, 0, 1);
			sd_card_cs_set();
			spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
			if((status & (0x80 | SD_CARD_R1_ILLEGAL_CMD)) == SD_CARD_R1_ILLEGAL_CMD)
				break;
			sd_card_cs_clear();
			status = sd_card_send_command(41, 0x40000000, 1);
			sd_card_cs_set();
			spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
			if(status == 0x00)
				break;
		}
		if(status != 0x00)
		{
			sd_card_status |= SD_CARD_STAT_CMD41_FAIL | SD_CARD_STAT_INIT_FAIL;
			return;
		}

		sd_card_cs_clear();
		status = sd_card_send_command(58, 0, 5);
		sd_card_cs_set();
		spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
		if(status)
		{
			sd_card_status |= SD_CARD_STAT_CMD58_FAIL | SD_CARD_STAT_INIT_FAIL;
			return;
		}
		if((sd_card_response & 0x40000000) == 0x40000000)
			sd_card_type |= SD_CARD_TYP_SDHC;
		else
			sd_card_type |= SD_CARD_TYP_SDSC;
	}
	else	//Could be SD v1.0 or MMC
	{
		initial_time = sd_card_time_var;
		while((sd_card_time_var - initial_time) <= SD_CARD_TIMEOUT)
		{
			sd_card_cs_clear();
			status = sd_card_send_command(55, 0, 1);
			sd_card_cs_set();
			spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
			if((status & (0x80 | SD_CARD_R1_ILLEGAL_CMD)) == SD_CARD_R1_ILLEGAL_CMD)
				break;
			sd_card_cs_clear();
			status = sd_card_send_command(41, 0x00000000, 1);
			sd_card_cs_set();
			spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
			if(status == 0x00)
				break;
		}

		if(status & SD_CARD_R1_ILLEGAL_CMD)
		{
			// check if the thing is an MMC card
			initial_time = sd_card_time_var;
			while((sd_card_time_var - initial_time) <= SD_CARD_TIMEOUT)
			{
				sd_card_cs_clear();
				status = sd_card_send_command(1, 0, 1);
				sd_card_cs_set();
				spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
				if(status == 0x00)
					break;
			}

			if(status != 0x00)
			{
				sd_card_status |= SD_CARD_STAT_CMD1_FAIL | SD_CARD_STAT_INIT_FAIL;
				return;
			}
			sd_card_type |= SD_CARD_TYP_MMC;
		}
		else
		{
			sd_card_type |= SD_CARD_TYP_SD10;
			if(status != 0x00)
			{
				sd_card_status |= SD_CARD_STAT_CMD41_FAIL | SD_CARD_STAT_INIT_FAIL;
				return;
			}
		}
	}

	// set the block length to 512 bytes
	if(!(sd_card_type & SD_CARD_TYP_SDHC))
	{
		sd_card_cs_clear();
		status = sd_card_send_command(16, 512, 1);
		sd_card_cs_set();
		spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
		if(status)
		{
			sd_card_status |= SD_CARD_STAT_CMD16_FAIL;
			return;
		}
	}

	spi_init(SD_CARD_SPI_MODULE, SPI_8_BIT | SD_CARD_RUN_SPI_CLK_DIV | SPI_MODE_0);
}

uint8_t sd_card_send_command(uint8_t command, uint32_t value, uint8_t resp_size)
{
	uint8_t datagram[6];
	uint8_t status;

	datagram[0] = 0x40 | command;
	datagram[1] = ((uint8_t*)&value)[3];
	datagram[2] = ((uint8_t*)&value)[2];
	datagram[3] = ((uint8_t*)&value)[1];
	datagram[4] = ((uint8_t*)&value)[0];
	datagram[5] = (sd_card_calc_crc7(datagram, 5) << 1) | 0x01;

	for(uint8_t d = 0; d < 6; ++d)
	{
		spi_transfer(SD_CARD_SPI_MODULE, datagram[d]);
	}

	for(uint8_t d = 0; d < 9; ++d)
	{
		status = (uint8_t)spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
		if(!(status & 0x80))
			break;
	}

	sd_card_response = 0;
	for(uint8_t d = 1; d < resp_size; ++d)
	{
		sd_card_response = sd_card_response << 8;
		sd_card_response |= 0xFF & spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
	}
	return status;
}

//HINT: This function does not mask off bit 7 of the return value, the user must do it if needed.
uint8_t sd_card_calc_crc7(uint8_t* datagram, uint8_t len)
{
	uint8_t crc7 = 0;

	for(uint8_t d = 0; d < len; ++d)
	{
		uint8_t current_byte = datagram[d];
		for(uint8_t i = 0; i < 8; ++i)
		{
			crc7 = crc7 << 1;
			if((current_byte & 0x80) ^ (crc7 & 0x80))
				crc7 = crc7 ^ 0x09;
			current_byte = current_byte << 1;
		}
	}
	return crc7;
}

void sd_card_read_block(uint32_t block, uint8_t* dest)
{
	uint32_t initial_time;
	uint8_t status;

	if(!(sd_card_type & SD_CARD_TYP_SDHC))
		block <<= 9;

	sd_card_cs_clear();
#if SD_CARD_EN_READ_BUSY_CHECK
	// Wait while the card is busy
	initial_time = sd_card_time_var;
	while((sd_card_time_var - initial_time) <= SD_CARD_TIMEOUT)
	{
		status = (uint8_t)spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
		if(status)
			break;
	}
	if(!status)
	{
		sd_card_cs_set();
		sd_card_status |= SD_CARD_STAT_READ_FAIL | SD_CARD_STAT_TIMEOUT;
		spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
		return;
	}
#endif
	status = sd_card_send_command(17, block, 1);
	if(status)
	{
		sd_card_cs_set();
		sd_card_status |= SD_CARD_STAT_CMD17_FAIL | SD_CARD_STAT_READ_FAIL;
		spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
		return;
	}

	initial_time = sd_card_time_var;
	while((sd_card_time_var - initial_time) <= SD_CARD_TIMEOUT)
	{
		status = (uint8_t)spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
		if(status == 0xFE)
			break;
	}
	if(status != 0xFE)
	{
		sd_card_cs_set();
		sd_card_status |= SD_CARD_STAT_READ_FAIL | SD_CARD_STAT_TIMEOUT;
		spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
		return;
	}

	for(uint16_t d = 0; d < 512; ++d)
	{
		dest[d] = (uint8_t)spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
	}

	// discard CRC bytes
	spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
	spi_transfer(SD_CARD_SPI_MODULE, 0xFF);

	sd_card_cs_set();
	spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
	return;
}

void sd_card_write_block(uint32_t block, uint8_t* source)
{
	uint8_t status;

	if(!(sd_card_type & SD_CARD_TYP_SDHC))
		block <<= 9;

	sd_card_cs_clear();
#if SD_CARD_EN_WRITE_BUSY_CHECK
	// Wait while the card is busy
	uint32_t initial_time;
	initial_time = sd_card_time_var;
	while((sd_card_time_var - initial_time) <= SD_CARD_TIMEOUT)
	{
		status = (uint8_t)spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
		if(status)
			break;
	}
	if(!status)
	{
		sd_card_cs_set();
		sd_card_status |= SD_CARD_STAT_WRITE_FAIL | SD_CARD_STAT_TIMEOUT;
		spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
		return;
	}
#endif
	status = sd_card_send_command(24, block, 1);
	if(status)
	{
		sd_card_cs_set();
		sd_card_status |= SD_CARD_STAT_CMD24_FAIL | SD_CARD_STAT_WRITE_FAIL;
		spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
		return;
	}

	// Send dummy byte and data start token
	spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
	spi_transfer(SD_CARD_SPI_MODULE, 0xFE);

	for(uint16_t d = 0; d < 512; ++d)
	{
		spi_transfer(SD_CARD_SPI_MODULE, source[d]);
	}

	// Send dummy CRC
	spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
	spi_transfer(SD_CARD_SPI_MODULE, 0xFF);

	// Get data response
	status = (uint8_t)spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
	if((status & 0x1F) != 0x05)
	{
		sd_card_cs_set();
		sd_card_status |= SD_CARD_STAT_WRITE_FAIL;
		spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
		return;
	}

	sd_card_cs_set();
	spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
}

uint32_t sd_card_get_num_blocks(void)
{
	uint32_t num_blocks;
	uint32_t initial_time;
	uint8_t csd[16];
	uint32_t c_size;
	uint8_t read_bl_len;
	uint8_t c_size_mult;
	uint8_t status;

	sd_card_cs_clear();
	status = sd_card_send_command(9, 0, 1);
	if(status)
	{
		sd_card_cs_set();
		sd_card_status |= SD_CARD_STAT_CMD9_FAIL;
		spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
		return 0;
	}

	initial_time = sd_card_time_var;
	while((sd_card_time_var - initial_time) <= SD_CARD_TIMEOUT)
	{
		status = (uint8_t)spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
		if(status == 0xFE)
			break;
	}
	if(status != 0xFE)
	{
		sd_card_cs_set();
		sd_card_status |= SD_CARD_STAT_GNB_FAIL | SD_CARD_STAT_TIMEOUT;
		spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
		return 0;
	}

	for(uint16_t d = 0; d < 16; ++d)
	{
		csd[d] = (uint8_t)spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
	}

	// discard CRC bytes
	spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
	spi_transfer(SD_CARD_SPI_MODULE, 0xFF);

	sd_card_cs_set();
	spi_transfer(SD_CARD_SPI_MODULE, 0xFF);

	//Parse the CSD
	if((csd[0] >> 6 == 0) || (sd_card_type & SD_CARD_TYP_MMC))	// SD CSD v1.0 or MMC CSD
	{
		c_size = ((uint32_t)(csd[6] & 0x03) << 10) | ((uint32_t)csd[7] << 2) | ((csd[8] >> 6) & 0x03);
		c_size_mult = ((csd[9] & 0x03) << 1) | ((csd[10] >> 7) & 0x01);
		read_bl_len = csd[5] & 0x0F;
		num_blocks = (uint32_t)(c_size + 1) << (c_size_mult + read_bl_len - 7);
	}
	else if(csd[0] >> 6 == 1)	//SD CSD v2.0
	{
		c_size = ((uint32_t)(csd[7] & 0x3F) << 16) | ((uint32_t)csd[8] << 8) | csd[9];
		num_blocks = (c_size + 1) << 10;
	}
	else
	{
		sd_card_status |= SD_CARD_STAT_GNB_FAIL;
		return 0;
	}

	return num_blocks;
}

uint8_t sd_card_wait_not_busy(void)
{
	uint32_t initial_time;
	uint8_t status = 0;

	sd_card_cs_clear();
	initial_time = sd_card_time_var;
	while((sd_card_time_var - initial_time) <= SD_CARD_TIMEOUT)
	{
		status = (uint8_t)spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
		if(status == 0xFF)
			break;
	}
	if(!status)
	{
		sd_card_cs_set();
		sd_card_status |= SD_CARD_STAT_TIMEOUT;
		spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
		return status;
	}
	sd_card_cs_set();
	spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
	return status;
}

uint8_t sd_card_is_busy(void)
{
	uint8_t busy;
	sd_card_cs_clear();
	busy = (uint8_t)spi_transfer(SD_CARD_SPI_MODULE, 0xFF) == 0x00;
	sd_card_cs_set();
	spi_transfer(SD_CARD_SPI_MODULE, 0xFF);
	return busy;
}
