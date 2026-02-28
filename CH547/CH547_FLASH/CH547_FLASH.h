#ifndef _CH547_FLASH_H_
#define _CH547_FLASH_H_

void flash_erase_code_page(UINT16 address);
void flash_erase_data_page(UINT16 address);
void flash_write_code_byte(UINT16 address, UINT8 val);
void flash_write_code(UINT16 address, UINT8* src, UINT16 num_words);
void flash_write_data_byte(UINT16 address, UINT8 val);
void flash_write_data(UINT16 address, UINT8* src, UINT16 num_words);
void flash_write_otp(UINT8 address, UINT8 val);
void flash_read_otp(UINT8 address, UINT8* dest);

#endif
