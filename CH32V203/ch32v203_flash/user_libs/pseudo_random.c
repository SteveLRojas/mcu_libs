#include <stdint.h>
#include "pseudo_random.h"

uint32_t pseudo_random_state;

void pseudo_random_generate(uint8_t num_bits)
{
	uint8_t next_bit;
	
	while(num_bits)
	{
		next_bit = (uint8_t)(pseudo_random_state >> 31);
		next_bit ^= (uint8_t)(pseudo_random_state >> 21);
		next_bit ^= (uint8_t)(pseudo_random_state >> 1);
		next_bit ^= (uint8_t)(pseudo_random_state);
		next_bit &= 0x01;
		pseudo_random_state = pseudo_random_state << 1;
		pseudo_random_state |= next_bit;
		--num_bits;
	}
}
