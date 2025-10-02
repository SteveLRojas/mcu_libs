/*
 * ch32v203_timer.c
 *
 *  Created on: Jul 4, 2024
 *      Author: Steve
 */
#include "ch32v20x.h"
#include "ch32v203_timer.h"

#if TIMER_IMPLEMENT_ISR
void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void (*timer1_callback)(void) = 0;
void (*timer2_callback)(void) = 0;
void (*timer3_callback)(void) = 0;
void (*timer4_callback)(void) = 0;

void TIM1_UP_IRQHandler(void)
{
	if(timer1_callback)
		timer1_callback();
	timer_clear_interrupt_flag(TIM1);
}

void TIM2_IRQHandler(void)
{
	if(timer2_callback)
		timer2_callback();
	timer_clear_interrupt_flag(TIM2);
}

void TIM3_IRQHandler(void)
{
	if(timer3_callback)
		timer3_callback();
	timer_clear_interrupt_flag(TIM3);
}

void TIM4_IRQHandler(void)
{
	if(timer4_callback)
		timer4_callback();
	timer_clear_interrupt_flag(TIM4);
}
#endif

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
