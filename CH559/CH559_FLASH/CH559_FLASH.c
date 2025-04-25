#define NO_XSFR_DEFINE
#include "CH559.H"
#include "CH559_FLASH.h"

//HINT: Flsh memory (code and data) words must be blank before they can be written to.
// It is only possible to erase the flash memory in blocks (pages) of 1KB (512 16-bit words).
// The code and data regions of the flash memory work the same way, but have different lock bits for safety.

//Erase a 1 KB page of code. The address given can be any address within the page.
void flash_erase_code_page(UINT16 address)
{
	E_DIS = 1;
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= bCODE_WE;
	SAFE_MOD = 0x00;
	
	ROM_ADDR = address;
	ROM_CTRL = ROM_CMD_ERASE;
	
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~bCODE_WE;
	SAFE_MOD = 0x00;
	E_DIS = 0;
}

//Erase a 1 KB page of data. The address given can be any address within the page.
void flash_erase_data_page(UINT16 address)
{
	E_DIS = 1;
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= bDATA_WE;
	SAFE_MOD = 0x00;
	
	ROM_ADDR = address;
	ROM_CTRL = ROM_CMD_ERASE;
	
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~bDATA_WE;
	SAFE_MOD = 0x00;
	E_DIS = 0;
}

//Writes a single 16-bit word to the code area.
#if USE_SINGLE_WORD_FUNCS
void flash_write_code_word(UINT16 address, UINT16 val)
{
	E_DIS = 1;
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= bCODE_WE;
	SAFE_MOD = 0x00;
	
	ROM_ADDR = address;
#if KEEP_ENDIANNESS
	ROM_DATA_L = ((UINT8*)&val)[0];
	ROM_DATA_H = ((UINT8*)&val)[1];
#else
	ROM_DATA = val;
#endif
	ROM_CTRL = ROM_CMD_PROG;
	
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~bCODE_WE;
	SAFE_MOD = 0x00;
	E_DIS = 0;
}
#endif

//Writes multiple 16-bit words to the code area.
void flash_write_code(UINT16 address, UINT16* src, UINT16 num_words)
{
	E_DIS = 1;
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= bCODE_WE;
	SAFE_MOD = 0x00;
	
	while(num_words)
	{
		ROM_ADDR = address;
#if KEEP_ENDIANNESS
		ROM_DATA_L = ((UINT8*)src)[0];
		ROM_DATA_H = ((UINT8*)src)[1];
#else
		ROM_DATA = *src;
#endif
		ROM_CTRL = ROM_CMD_PROG;
		++src;
		address += 2;
		--num_words;
	}
	
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~bCODE_WE;
	SAFE_MOD = 0x00;
	E_DIS = 0;
}

//Writes a single 16-bit word to the data area.
#if USE_SINGLE_WORD_FUNCS
void flash_write_data_word(UINT16 address, UINT16 val)
{
	E_DIS = 1;
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= bDATA_WE;
	SAFE_MOD = 0x00;
	
	ROM_ADDR = address;
#if KEEP_ENDIANNESS
	ROM_DATA_L = ((UINT8*)&val)[0];
	ROM_DATA_H = ((UINT8*)&val)[1];
#else
	ROM_DATA = val;
#endif
	ROM_CTRL = ROM_CMD_PROG;
	
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~bDATA_WE;
	SAFE_MOD = 0x00;
	E_DIS = 0;
}
#endif

//Writes multiple 16-bit words to the data area.
void flash_write_data(UINT16 address, UINT16* src, UINT16 num_words)
{
	E_DIS = 1;
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= bDATA_WE;
	SAFE_MOD = 0x00;
	
	while(num_words)
	{
		ROM_ADDR = address;
#if KEEP_ENDIANNESS
		ROM_DATA_L = ((UINT8*)src)[0];
		ROM_DATA_H = ((UINT8*)src)[1];
#else
		ROM_DATA = *src;
#endif
		ROM_CTRL = ROM_CMD_PROG;
		++src;
		address += 2;
		--num_words;
	}
	
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~bDATA_WE;
	SAFE_MOD = 0x00;
	E_DIS = 0;
}
