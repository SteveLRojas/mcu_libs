#ifndef _CH559_FLASH_H_
#define _CH559_FLASH_H_

#define KEEP_ENDIANNESS 1
#define USE_SINGLE_WORD_FUNCS 1

void flash_erase_code_page(UINT16 address);
void flash_erase_data_page(UINT16 address);
#if USE_SINGLE_WORD_FUNCS
void flash_write_code_word(UINT16 address, UINT16 val);
void flash_write_data_word(UINT16 address, UINT16 val);
#endif
void flash_write_code(UINT16 address, UINT16* src, UINT16 num_words);
void flash_write_data(UINT16 address, UINT16* src, UINT16 num_words);

#endif
