/*
 * ch32v203_timer.h
 *
 *  Created on: Jul 4, 2024
 *      Author: Steve
 */

#ifndef _CH32V203_TIMER_H_
#define _CH32V203_TIMER_H_

#define timer_set_period(timer, period) ((timer)->ATRLR = (period))
#define timer_clear_interrupt_flag(timer) ((timer)->INTFR = 0)
#define timer_enable_interrupt(timer) ((timer)->DMAINTENR |= TIM_UIE)
#define timer_disable_interrupt(timer) ((timer)->DMAINTENR &= ~TIM_UIE)
#define timer_start(timer) ((timer)->CTLR1 |= TIM_CEN)
#define timer_stop(timer) ((timer)->CTLR1 &= ~TIM_CEN)
#define timer_get_ticks(timer) ((timer)->CNT)

void timer_init(TIM_TypeDef* timer, uint16_t prescaler, uint16_t period);
void timer_short_delay(TIM_TypeDef* timer, uint16_t num_ticks);
void timer_long_delay(TIM_TypeDef* timer, uint16_t num_overflows);

#endif /* _CH32V203_TIMER_H_ */
