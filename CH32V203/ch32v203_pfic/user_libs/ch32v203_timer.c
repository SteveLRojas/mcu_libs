/*
 * ch32v203_timer.c
 *
 *  Created on: Jul 4, 2024
 *      Author: Steve
 */
#include "ch32v20x.h"
#include "ch32v203_timer.h"

void timer_init(TIM_TypeDef* timer, uint16_t prescaler, uint16_t period)
{
	timer->DMAINTENR = 0;
	timer->CTLR1 = 0;
	timer->CTLR2 = 0;
	timer->SMCFGR = 0;
	timer->CHCTLR1 = 0;
	timer->CHCTLR2 = 0;
	timer->CCER = 0;
	timer->CNT = 0;
	timer->PSC = prescaler;
	timer->ATRLR = period;
	timer->RPTCR = 0;
	timer->SWEVGR = TIM_UG;
	timer->INTFR = 0;
}

// HINT: overwrites the previously specified period
void timer_short_delay(TIM_TypeDef* timer, uint16_t num_ticks)
{
	timer->CNT = 0;
	timer->ATRLR = num_ticks;
	timer->INTFR &= ~TIM_UIF;
	timer->CTLR1 |= TIM_CEN;
	while(!(timer->INTFR & TIM_UIF));
	timer->CTLR1 &= ~TIM_CEN;
}

// HINT: disable the timer interrupts before calling this function
void timer_long_delay(TIM_TypeDef* timer, uint16_t num_overflows)
{
	timer->CNT = 0;
	timer->CTLR1 |= TIM_CEN;

	do
	{
		timer->INTFR &= ~TIM_UIF;
		while(!(timer->INTFR & TIM_UIF));
		--num_overflows;
	} while(num_overflows);
	timer->CTLR1 &= ~TIM_CEN;
}
