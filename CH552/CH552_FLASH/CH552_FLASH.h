#ifndef _CH552_FLASH_H_
#define _CH552_FLASH_H_

#define KEEP_ENDIANNESS 1

void flash_write_byte(UINT8 offset, UINT8 val);
void flash_write(UINT8 offset, UINT8* src, UINT8 num_bytes);
UINT8 flash_read_byte(UINT8 offset);
void flash_read(UINT8 offset, UINT8* dest, UINT8 num_bytes);
void flash_write_code_word(UINT16 offset, UINT16 val);
void flash_write_code(UINT16 offset, UINT16* src, UINT8 num_words);

#endif
