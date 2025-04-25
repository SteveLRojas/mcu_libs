#ifndef _PSEUDO_RANDOM_H_
#define _PSEUDO_RANDOM_H_

extern UINT32 pseudo_random_state;

#define pseudo_random_seed(seed) (pseudo_random_state = (seed))
#define pseudo_random_get_byte() ((UINT8)pseudo_random_state)
#define pseudo_random_get_word() ((UINT16)pseudo_random_state)
#define pseudo_random_get_dword() (pseudo_random_state)

void pseudo_random_generate(UINT8 num_bits);

#endif