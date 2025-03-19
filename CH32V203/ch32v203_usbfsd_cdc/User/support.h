/*
 * support.h
 *
 *  Created on: Dec 9, 2023
 *      Author: Steve
 */

#ifndef SUPPORT_H_
#define SUPPORT_H_

void byte_to_hex(uint8_t value, char* buff);
void word_to_hex(uint16_t, char*);
void print_hex_byte(uint8_t);
void print_hex_word(uint16_t);
void print_test_data();
uint16_t get_hex();
void to_caps(char* str);

#endif /* SUPPORT_H_ */
