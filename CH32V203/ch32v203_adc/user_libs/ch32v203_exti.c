#include "ch32v20x.h"
#include "ch32v203_exti.h"

void EXTI0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI9_5_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI15_10_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void (*exti0_callback)(void) = 0;
void (*exti1_callback)(void) = 0;
void (*exti2_callback)(void) = 0;
void (*exti3_callback)(void) = 0;
void (*exti4_callback)(void) = 0;
void (*exti5_callback)(void) = 0;
void (*exti6_callback)(void) = 0;
void (*exti7_callback)(void) = 0;
void (*exti8_callback)(void) = 0;
void (*exti9_callback)(void) = 0;
void (*exti10_callback)(void) = 0;
void (*exti11_callback)(void) = 0;
void (*exti12_callback)(void) = 0;
void (*exti13_callback)(void) = 0;
void (*exti14_callback)(void) = 0;
void (*exti15_callback)(void) = 0;

void EXTI0_IRQHandler(void)
{
	if(exti0_callback)
		exti0_callback();
	EXTI->INTFR = EXTI_CHAN_0;
}

void EXTI1_IRQHandler(void)
{
	if(exti1_callback)
		exti1_callback();
	EXTI->INTFR = EXTI_CHAN_1;
}

void EXTI2_IRQHandler(void)
{
	if(exti2_callback)
		exti2_callback();
	EXTI->INTFR = EXTI_CHAN_2;
}

void EXTI3_IRQHandler(void)
{
	if(exti3_callback)
		exti3_callback();
	EXTI->INTFR = EXTI_CHAN_3;
}

void EXTI4_IRQHandler(void)
{
	if(exti4_callback)
		exti4_callback();
	EXTI->INTFR = EXTI_CHAN_4;
}

void EXTI9_5_IRQHandler(void)
{
	if(EXTI->INTFR & EXTI_CHAN_5)
	{
		if(exti5_callback)
			exti5_callback();
		EXTI->INTFR = EXTI_CHAN_5;
	}
	if(EXTI->INTFR & EXTI_CHAN_6)
	{
		if(exti6_callback)
			exti6_callback();
		EXTI->INTFR = EXTI_CHAN_6;
	}
	if(EXTI->INTFR & EXTI_CHAN_7)
	{
		if(exti7_callback)
			exti7_callback();
		EXTI->INTFR = EXTI_CHAN_7;
	}
	if(EXTI->INTFR & EXTI_CHAN_8)
	{
		if(exti8_callback)
			exti8_callback();
		EXTI->INTFR = EXTI_CHAN_8;
	}
	if(EXTI->INTFR & EXTI_CHAN_9)
	{
		if(exti9_callback)
			exti9_callback();
		EXTI->INTFR = EXTI_CHAN_9;
	}
}

void EXTI15_10_IRQHandler(void)
{
	if(EXTI->INTFR & EXTI_CHAN_10)
	{
		if(exti10_callback)
			exti10_callback();
		EXTI->INTFR = EXTI_CHAN_10;
	}
	if(EXTI->INTFR & EXTI_CHAN_11)
	{
		if(exti11_callback)
			exti11_callback();
		EXTI->INTFR = EXTI_CHAN_11;
	}
	if(EXTI->INTFR & EXTI_CHAN_12)
	{
		if(exti12_callback)
			exti12_callback();
		EXTI->INTFR = EXTI_CHAN_12;
	}
	if(EXTI->INTFR & EXTI_CHAN_13)
	{
		if(exti13_callback)
			exti13_callback();
		EXTI->INTFR = EXTI_CHAN_13;
	}
	if(EXTI->INTFR & EXTI_CHAN_14)
	{
		if(exti14_callback)
			exti14_callback();
		EXTI->INTFR = EXTI_CHAN_14;
	}
	if(EXTI->INTFR & EXTI_CHAN_15)
	{
		if(exti15_callback)
			exti15_callback();
		EXTI->INTFR = EXTI_CHAN_15;
	}
}
