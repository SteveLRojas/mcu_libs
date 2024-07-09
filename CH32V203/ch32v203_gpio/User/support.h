/*
 * support.h
 *
 *  Created on: Dec 9, 2023
 *      Author: Steve
 */

#ifndef SUPPORT_H_
#define SUPPORT_H_

typedef union Bitfield_16
{
	struct Bits_16
	{
		unsigned bit0 : 1;
		unsigned bit1 : 1;
		unsigned bit2 : 1;
		unsigned bit3 : 1;
		unsigned bit4 : 1;
		unsigned bit5 : 1;
		unsigned bit6 : 1;
		unsigned bit7 : 1;
		unsigned bit8 : 1;
		unsigned bit9 : 1;
		unsigned bit10 : 1;
		unsigned bit11 : 1;
		unsigned bit12 : 1;
		unsigned bit13 : 1;
		unsigned bit14 : 1;
		unsigned bit15 : 1;
	} bits;
	unsigned int value;
} bitfield_16_t;

//extern bitfield_16_t bitfield_16;

void random_init(uint16_t seed);
uint16_t get_word();
uint16_t build_word();
uint8_t get_test_byte();
void byte_to_hex(uint8_t value, char* buff);
void word_to_hex(uint16_t, char*);
void print_hex_byte(uint8_t);
void print_hex_word(uint16_t);
void print_test_data();
uint16_t get_hex();
void to_caps(char* str);

#endif /* SUPPORT_H_ */
