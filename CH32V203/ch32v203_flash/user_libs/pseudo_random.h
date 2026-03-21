#ifndef _PSEUDO_RANDOM_H_
#define _PSEUDO_RANDOM_H_

extern uint32_t pseudo_random_state;

#define pseudo_random_seed(seed) (pseudo_random_state = (seed))
#define pseudo_random_get_byte() ((uint8_t)pseudo_random_state)
#define pseudo_random_get_word() ((uint16_t)pseudo_random_state)
#define pseudo_random_get_dword() (pseudo_random_state)

void pseudo_random_generate(uint8_t num_bits);

#endif
