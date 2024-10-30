#include "CH552.H"
#include "CH552_FLASH.h"

void flash_write_byte(UINT8 offset, UINT8 val)
{
	E_DIS = 1;
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= bDATA_WE;
	SAFE_MOD = 0x00;
	
	ROM_ADDR_H = 0xC0;
	ROM_ADDR_L = offset << 1;
	ROM_DATA_L = val;
	ROM_CTRL = ROM_CMD_WRITE;
	
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~bDATA_WE;
	SAFE_MOD = 0x00;
	E_DIS = 0;
}

void flash_write(UINT8 offset, UINT8* src, UINT8 num_bytes)
{
	E_DIS = 1;
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= bDATA_WE;
	SAFE_MOD = 0x00;
	
	ROM_ADDR_H = 0xC0;
	offset = offset << 1;
	while(num_bytes)
	{
		ROM_ADDR_L = offset;
		ROM_DATA_L = *src;
		ROM_CTRL = ROM_CMD_WRITE;
		++src;
		offset += 2;
		--num_bytes;
	}
	
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~bDATA_WE;
	SAFE_MOD = 0x00;
	E_DIS = 0;
}

UINT8 flash_read_byte(UINT8 offset)
{
	ROM_ADDR_H = 0xC0;
	ROM_ADDR_L = offset << 1;
	ROM_CTRL = ROM_CMD_READ;
	return ROM_DATA_L;
}

void flash_read(UINT8 offset, UINT8* dest, UINT8 num_bytes)
{
	ROM_ADDR_H = 0xC0;
	offset = offset << 1;
	while(num_bytes)
	{
		ROM_ADDR_L = offset;
		ROM_CTRL = ROM_CMD_READ;
		*dest = ROM_DATA_L;
		++dest;
		offset += 2;
		--num_bytes;
	}
}

void flash_write_code_word(UINT16 offset, UINT16 val)
{
	E_DIS = 1;
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= bCODE_WE;
	SAFE_MOD = 0x00;
	E_DIS = 0;
	
	ROM_ADDR = offset;
#if KEEP_ENDIANNESS
	ROM_DATA_L = ((UINT8*)&val)[0];
	ROM_DATA_H = ((UINT8*)&val)[1];
#else
	ROM_DATA = val;
#endif
	ROM_CTRL = ROM_CMD_WRITE;
	
	E_DIS = 1;
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~bCODE_WE;
	SAFE_MOD = 0x00;
	E_DIS = 0;
}

void flash_write_code(UINT16 offset, UINT16* src, UINT8 num_words)
{
	E_DIS = 1;
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= bCODE_WE;
	SAFE_MOD = 0x00;
	E_DIS = 0;
	
	while(num_words)
	{
		ROM_ADDR = offset;
#if KEEP_ENDIANNESS
		ROM_DATA_L = ((UINT8*)src)[0];
		ROM_DATA_H = ((UINT8*)src)[1];
#else
		ROM_DATA = *src;
#endif
		ROM_CTRL = ROM_CMD_WRITE;
		++src;
		offset += 2;
		--num_words;
	}
	
	E_DIS = 1;
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~bCODE_WE;
	SAFE_MOD = 0x00;
	E_DIS = 0;
}
