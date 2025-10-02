/*
 * ch32v203_adc.c
 *
 *  Created on: Apr 26, 2025
 *      Author: Steve
 */
#include "ch32v20x.h"
#include "ch32v203_adc.h"

// The maximum clock frequency of the ADC is 14 MHz, but usually it runs at 12 MHz.

uint16_t* adc1_results = 0;
const uint8_t* adc1_schedule = 0;
uint8_t adc1_schedule_len;
uint8_t adc1_state;
uint8_t adc1_pending_samples;

uint16_t* adc2_results = 0;
const uint8_t* adc2_schedule = 0;
uint8_t adc2_schedule_len;
uint8_t adc2_state;
uint8_t adc2_pending_samples;

void ADC1_2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void ADC1_2_IRQHandler(void)
{
	if(ADC1->STATR & ADC_EOC)
	{
		adc1_results[adc1_state] = ADC1->RDATAR;

		if(--adc1_pending_samples)
		{
			++adc1_state;
			if(adc1_state == adc1_schedule_len)
				adc1_state = 0;

			ADC1->RSQR3 = adc1_schedule[adc1_state];
			ADC1->CTLR2 |= ADC_ADON;
		}
	}

	if(ADC2->STATR & ADC_EOC)
	{
		adc2_results[adc2_state] = ADC2->RDATAR;

		if(--adc2_pending_samples)
		{
			++adc2_state;
			if(adc2_state == adc2_schedule_len)
				adc2_state = 0;

			ADC2->RSQR3 = adc2_schedule[adc2_state];
			ADC2->CTLR2 |= ADC_ADON;
		}
	}
}

void adc_init(ADC_TypeDef* adc)
{
	//TODO: Add reset macros to RCC library and use them to reset the ADC
	adc->CTLR1 = 0;
	adc->CTLR2 = 0;
	//adc->CTLR1 = ADC_SCAN | ADC_DISCEN;	//enable scan mode, enable discontinuous mode
	adc->CTLR2 = ADC_ADON;	//enable the ADC
	adc->SAMPTR1 = 0x00249249;
	adc->SAMPTR2 = 0x09249249;
	adc->RSQR1 = 0x00100000;	//use 1 channel
	//adc->CTLR1 = ADC_EOCIE;	//enable ADC interrupt
}

void adc_init_schedule(ADC_TypeDef* adc)
{
	if(adc == ADC1)
	{
		if(adc1_results && adc1_schedule)
		{
			adc1_state = 0;
			ADC1->STATR = ~ADC_EOC;
			adc_enable_interrupt(ADC1);
		}
	}
	else if(adc == ADC2)
	{
		if(adc2_results && adc2_schedule)
		{
			adc2_state = 0;
			ADC2->STATR = ~ADC_EOC;
			adc_enable_interrupt(ADC2);
		}
	}
}

void adc_run_schedule(ADC_TypeDef* adc, uint8_t num_samples)
{
	if(adc == ADC1)
	{
		ADC1->RSQR3 = adc1_schedule[adc1_state];
		adc1_pending_samples = num_samples;
		ADC1->CTLR2 |= ADC_ADON;
	}
	else if(adc == ADC2)
	{
		ADC2->RSQR3 = adc2_schedule[adc2_state];
		adc2_pending_samples = num_samples;
		ADC2->CTLR2 |= ADC_ADON;
	}
}

//HINT: disable ADC interrupt before using this function
void adc_start_single(ADC_TypeDef* adc, uint8_t channel)
{
	adc->RSQR3 = channel;
	adc->STATR = ~ADC_EOC;
	adc->CTLR2 |= ADC_ADON;
}

uint16_t adc_finish_single(ADC_TypeDef* adc)
{
	while(!(adc->STATR & ADC_EOC));
	return (uint16_t)(adc->RDATAR);
}

//HINT: disable ADC interrupt before using this function
uint16_t adc_read_single(ADC_TypeDef* adc, uint8_t channel)
{
	adc->RSQR3 = channel;
	adc->STATR = ~ADC_EOC;
	adc->CTLR2 |= ADC_ADON;
	while(!(adc->STATR & ADC_EOC));
	return (uint16_t)(adc->RDATAR);
}
