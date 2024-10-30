#include "CH552.H"
#include "pseudo_random.h"

UINT32 pseudo_random_state;

void pseudo_random_generate(UINT8 num_bits)
{
	UINT8 next_bit;
	
	while(num_bits)
	{
		next_bit = (UINT8)(pseudo_random_state >> 31);
		next_bit ^= (UINT8)(pseudo_random_state >> 21);
		next_bit ^= (UINT8)(pseudo_random_state >> 1);
		next_bit ^= (UINT8)(pseudo_random_state);
		next_bit &= 0x01;
		pseudo_random_state = pseudo_random_state << 1;
		pseudo_random_state |= next_bit;
		--num_bits;
	}
}
