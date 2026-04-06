#ifndef _CH552_TIMER_H_
#define _CH552_TIMER_H_

#define TIMER_0 0
#define TIMER_1 1
#define TIMER_2 2

extern UINT16 timer_overflow_counts[3];

void timer_init(UINT8 timer, void (*callback)(void));
void timer_set_period(UINT8 timer, UINT16 period);
void timer_set_interrupt_enable(UINT8 timer, UINT8 ie);
void timer_start(UINT8 timer);
void timer_stop(UINT8 timer);
UINT16 timer_get_ticks(UINT8 timer);
void timer_short_delay(UINT8 timer, UINT16 num_ticks);
void timer_long_delay(UINT8 timer, UINT16 num_overflows);

#endif
