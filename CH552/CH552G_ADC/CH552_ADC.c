#include "CH552.H"
#include "System.h"
#include "CH552_ADC.h"

UINT8 adc_results[4];
UINT8 adc_schedule[4];
UINT8 adc_schedule_len;

UINT8 adc_state;
UINT8 adc_pending_samples;

void adc_isr(void) interrupt INT_NO_ADC
{
	adc_results[adc_state] = ADC_DATA;
	
	if(--adc_pending_samples)
	{
		++adc_state;
		if(adc_state == adc_schedule_len)
			adc_state = 0;
		
		ADC_CHAN1 = 0;
		ADC_CHAN0 = 0;
		ADC_CTRL |= adc_schedule[adc_state];
		ADC_START = 1;
	}
	
	ADC_IF = 0;
}

void adc_init(UINT8 adc_clk)
{
	IE_ADC = 0;
	ADC_CFG |= bADC_EN;
	ADC_CFG &= ~bADC_CLK;
	ADC_CFG |= adc_clk;
}

void adc_init_schedule(void)
{
	adc_state = 0;
	ADC_IF = 0;
	IE_ADC = 1;
}

void adc_run_schedule(UINT8 num_samples)
{
	ADC_CHAN1 = 0;
	ADC_CHAN0 = 0;
	ADC_CTRL |= adc_schedule[adc_state];
	adc_pending_samples = num_samples;
	ADC_START = 1;
}

void adc_start_single(UINT8 channel)
{
	ADC_CHAN1 = 0;
	ADC_CHAN0 = 0;
	ADC_CTRL |= channel;
	IE_ADC = 0;
	ADC_START = 1;
}

UINT8 adc_finish_single(void)
{
	while(ADC_START) {}
	return ADC_DATA;
}

UINT8 adc_read_single(UINT8 channel)
{
	ADC_CHAN1 = 0;
	ADC_CHAN0 = 0;
	ADC_CTRL |= channel;
	IE_ADC = 0;
	ADC_START = 1;
	while(ADC_START) {}
	return ADC_DATA;
}

//HINT: The comparator is mostly useless since it cannot generate interrupts...
#ifdef USE_CMP
void cmp_init(UINT8 pin_cfg)
{
	ADC_CTRL &= 0xF0;
	ADC_CTRL |= pin_cfg;
	ADC_CFG |= bCMP_EN;
}

UINT8 cmp_get_result(void)
{
	return CMPO;
}

UINT8 cmp_get_change(void)
{
	if(CMP_IF)
	{
		CMP_IF = 0;
		return 1;
	}
	else
		return 0;
}
#endif
