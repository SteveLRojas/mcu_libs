#include "CH547.H"
#include "CH547_FLASH.h"

//HINT: Flsh memory (code and data) words must be blank before they can be written to.
// It is only possible to erase the flash memory in blocks (pages) of 64 bytes.
// The code and data regions of the flash memory work the same way, but have different lock bits for safety.
// An unprogrammed bit reads as 0, a programmed bit reads as 1.

//Erase a 64-byte page of code. The address given can be any address within the page.
void flash_erase_code_page(UINT16 address)
{
	E_DIS = 1;
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= bCODE_WE;
	SAFE_MOD = 0x00;
	
	ROM_ADDR = address;
	ROM_BUF_MOD = 0x80;
	ROM_DAT_BUF = 0x00;
	ROM_CTRL = ROM_CMD_ERASE;
	
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~bCODE_WE;
	SAFE_MOD = 0x00;
	E_DIS = 0;
}

//Erase a 64-byte page of data. The address given can be any address within the page.
void flash_erase_data_page(UINT16 address)
{
	E_DIS = 1;
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= bDATA_WE;
	SAFE_MOD = 0x00;
	
	ROM_ADDR = address;
	ROM_BUF_MOD = 0x80;
	ROM_DAT_BUF = 0x00;
	ROM_CTRL = ROM_CMD_ERASE;
	
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~bDATA_WE;
	SAFE_MOD = 0x00;
	E_DIS = 0;
}

//Writes a single byte to the code area.
void flash_write_code_byte(UINT16 address, UINT8 val)
{
	E_DIS = 1;
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= bCODE_WE;
	SAFE_MOD = 0x00;
	
	ROM_ADDR = address;
	ROM_BUF_MOD = 0x80;
	ROM_DAT_BUF = val;
	ROM_CTRL = ROM_CMD_PROG;
	
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~bCODE_WE;
	SAFE_MOD = 0x00;
	E_DIS = 0;
}

//Writes multiple bytes to the code area.
void flash_write_code(UINT16 address, UINT8* src, UINT16 num_words)
{
	E_DIS = 1;
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= bCODE_WE;
	SAFE_MOD = 0x00;
	
	ROM_BUF_MOD = 0x80;
	while(num_words)
	{
		ROM_ADDR = address;
		ROM_DAT_BUF = *src;
		ROM_CTRL = ROM_CMD_PROG;
		++src;
		++address;
		--num_words;
	}
	
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~bCODE_WE;
	SAFE_MOD = 0x00;
	E_DIS = 0;
}

//Writes a single byte to the data area.
void flash_write_data_byte(UINT16 address, UINT8 val)
{
	E_DIS = 1;
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= bDATA_WE;
	SAFE_MOD = 0x00;
	
	ROM_ADDR = address;
	ROM_BUF_MOD = 0x80;
	ROM_DAT_BUF = val;
	ROM_CTRL = ROM_CMD_PROG;
	
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~bDATA_WE;
	SAFE_MOD = 0x00;
	E_DIS = 0;
}

//Writes multiple 16-bit words to the data area.
void flash_write_data(UINT16 address, UINT8* src, UINT16 num_words)
{
	E_DIS = 1;
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= bDATA_WE;
	SAFE_MOD = 0x00;
	
	ROM_BUF_MOD = 0x80;
	while(num_words)
	{
		ROM_ADDR = address;
		ROM_DAT_BUF = *src;
		ROM_CTRL = ROM_CMD_PROG;
		++src;
		++address;
		--num_words;
	}
	
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~bDATA_WE;
	SAFE_MOD = 0x00;
	E_DIS = 0;
}

//HINT: OTP has its own address space, valid from 0x20 to 0x3F.
void flash_write_otp(UINT8 address, UINT8 val)
{
	E_DIS = 1;
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= bDATA_WE;
	SAFE_MOD = 0x00;
	
	ROM_ADDR = address;
	ROM_BUF_MOD = 0x80;
	ROM_DAT_BUF = val;
	ROM_CTRL = ROM_CMD_PG_OTP;
	
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~bDATA_WE;
	SAFE_MOD = 0x00;
	E_DIS = 0;
}

//HINT: OTP can only be read in 4-byte blocks
void flash_read_otp(UINT8 address, UINT8* dest)
{
	E_DIS = 1;
	
	ROM_ADDR = address;
	ROM_CTRL = ROM_CMD_RD_OTP;
	dest[0] = ROM_DATA_LL;
	dest[1] = ROM_DATA_LH;
	dest[2] = ROM_DATA_HL;
	dest[3] = ROM_DATA_HH;
	
	E_DIS = 0;
}
