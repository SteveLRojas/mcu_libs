/*
 * ch32v203_flash.h
 *
 *  Created on: Mar 15, 2026
 *      Author: dragomir
 */

#ifndef _CH32V203_FLASH_H_
#define _CH32V203_FLASH_H_
//HINT: when writing or erasing the flash the internal RC oscillator (HSI) must be enabled.
//HINT: Addresses used for writing or erasing must be actual flash addresses (starting at 0x08000000), not aliased addresses (starting at 0x00000000).

typedef struct
{
    const uint16_t FLACAP;	//0x1FFFF7E0
    const uint16_t RESERVED1;	//0x1FFFF7E2
    const uint32_t RESERVED2;	//0x1FFFF7E4
    const uint32_t UNIID1;	//0x1FFFF7E8
    const uint32_t UNIID2;	//0x1FFFF7EC
    const uint32_t UNIID3;	//0x1FFFF7F0
} ESIG_TypeDef;

#define ESIG ((ESIG_TypeDef*)0x1FFFF7E0)

void flash_unlock(void);
void flash_lock(void);
void flash_write_single16(uint32_t address, uint16_t val);
void flash_write16(uint32_t address, uint16_t* src, uint16_t num_words);
void flash_erase_page(uint32_t address);
void flash_erase_range(uint32_t start_addr, uint32_t end_addr);
void flash_chip_erase(void);
void flash_fast_unlock(void);
void flash_fast_lock(void);
void flash_fast_write_page(uint32_t address, uint32_t* src);
void flash_fast_erase_page(uint32_t address);
void flash_fast_erase_block_32kb(uint32_t address);
void flash_fast_erase_block_64kb(uint32_t address);
void flash_unlock_option_bytes(void);
void flash_write_option_bytes(uint32_t address, uint16_t val);
void flash_erase_option_bytes(void);

#endif /* S_CH32V203_FLASH_H_ */
