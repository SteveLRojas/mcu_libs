#define NO_XSFR_DEFINE
#include "CH559.H"
#include "CH559_ADC.h"

UINT16 adc_results[8];
UINT8 adc_schedule[8];
UINT8 adc_schedule_len;

UINT8 adc_state;
UINT8 adc_pending_samples;

void adc_isr(void) interrupt INT_NO_ADC
{
#if ADC_TRIG_MODE == 0
	UINT8 len_count;
#endif
	adc_results[adc_state] = ADC_FIFO;
	
	if(--adc_pending_samples)
	{
		++adc_state;
		if(adc_state == adc_schedule_len)
			adc_state = 0;
		
		ADC_CHANN = adc_schedule[adc_state];
#if ADC_TRIG_MODE == 0
		ADC_CTRL = bADC_SAMPLE;
		len_count = ADC_PULSE_LEN;
		do
		{
			++SAFE_MOD;
		} while(--len_count);
		ADC_CTRL = 0x00;
#else
		ADC_CTRL = bADC_SAMPLE;
		while(ADC_SETUP & bADC_CLOCK);
		while(!(ADC_SETUP & bADC_CLOCK));
		ADC_CTRL = 0x00;
#endif		
	}
	
	ADC_STAT = 0xFF;
}

void adc_init(UINT8 clk_config, UINT8 pins)
{
	IE_ADC = 0;
	P1_IE = ~pins;
	ADC_CK_SE = clk_config;
	ADC_STAT = 0xFF;	//clear all flags
	ADC_CTRL = 0x00;	//manual trigger mode, channel selected by ADC_CHANN
	ADC_CHANN = ADC_AIN0;
	ADC_SETUP = bADC_IE_ACT | bADC_POWER_EN | bADC_EXT_SW_EN;
	ADC_EX_SW = bADC_RESOLUTION;	//11-bit resolution
	SAFE_MOD = ADC_FIFO_L;
	SAFE_MOD = ADC_FIFO_H;
	SAFE_MOD = ADC_FIFO_L;
	SAFE_MOD = ADC_FIFO_H;	//clear the fifo
}

void adc_init_schedule(void)
{
	adc_state = 0;
	ADC_STAT = 0xFF;
	IE_ADC = 1;
}

void adc_run_schedule(UINT8 num_samples)
{
#if ADC_TRIG_MODE == 0
	UINT8 len_count;
	
	ADC_CHANN = adc_schedule[adc_state];
	adc_pending_samples = num_samples;
	ADC_CTRL = bADC_SAMPLE;
	len_count = ADC_PULSE_LEN;
	do
	{
		++SAFE_MOD;
	} while(--len_count);
	ADC_CTRL = 0x00;
#else
	ADC_CHANN = adc_schedule[adc_state];
	adc_pending_samples = num_samples;
	ADC_CTRL = bADC_SAMPLE;
	while(ADC_SETUP & bADC_CLOCK);
	while(!(ADC_SETUP & bADC_CLOCK));
	ADC_CTRL = 0x00;
#endif
}

void adc_start_single(UINT8 channel)
{
#if ADC_TRIG_MODE == 0
	UINT8 len_count;
	
	ADC_CHANN = channel;
	IE_ADC = 0;
	ADC_CTRL = bADC_SAMPLE;
	len_count = ADC_PULSE_LEN;
	do
	{
		++SAFE_MOD;
	} while(--len_count);
	ADC_CTRL = 0x00;
#else
	ADC_CHANN = channel;
	IE_ADC = 0;
	ADC_CTRL = bADC_SAMPLE;
	while(ADC_SETUP & bADC_CLOCK);
	while(!(ADC_SETUP & bADC_CLOCK));
	ADC_CTRL = 0x00;
#endif
}

UINT16 adc_finish_single(void)
{
	while(!(ADC_STAT & bADC_IF_ACT));
	ADC_STAT = bADC_IF_ACT;
	return ADC_FIFO;
}

UINT16 adc_read_single(UINT8 channel)
{
#if ADC_TRIG_MODE == 0
	UINT8 len_count;
	
	ADC_CHANN = channel;
	IE_ADC = 0;
	ADC_CTRL = bADC_SAMPLE;
	len_count = ADC_PULSE_LEN;
	do
	{
		++SAFE_MOD;
	} while(--len_count);
	ADC_CTRL = 0x00;
#else
	ADC_CHANN = channel;
	IE_ADC = 0;
	ADC_CTRL = bADC_SAMPLE;
	while(ADC_SETUP & bADC_CLOCK);
	while(!(ADC_SETUP & bADC_CLOCK));
	ADC_CTRL = 0x00;
#endif
	while(!(ADC_STAT & bADC_IF_ACT));
	ADC_STAT = bADC_IF_ACT;
	return ADC_FIFO;
}
