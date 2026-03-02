#include "CH547.H"
#include "CH547_ADC.h"

volatile UINT16 adc_results[ADC_MAX_SCHEDULE_LEN];
UINT8 adc_schedule[ADC_MAX_SCHEDULE_LEN];
UINT8 adc_schedule_len;
UINT8 adc_state;
volatile UINT8 adc_pending_samples;
#if ADC_TKEY_MAX_SCHEDULE_LEN
signed short adc_tkey_offsets[ADC_TKEY_MAX_SCHEDULE_LEN];
signed short adc_tkey_threshold = ADC_TKEY_DEF_THRESHOLD;
volatile UINT16 adc_tkey_results_raw[ADC_TKEY_MAX_SCHEDULE_LEN];
volatile UINT8 adc_tkey_results[ADC_TKEY_MAX_SCHEDULE_LEN];
UINT8 adc_tkey_schedule[ADC_TKEY_MAX_SCHEDULE_LEN];
UINT8 adc_tkey_schedule_len;
UINT8 adc_tkey_state;
volatile UINT8 adc_tkey_pending_samples;
#endif
volatile UINT8 adc_cmp_changed;

void (*adc_done_callback)(void) = NULL;
#if ADC_TKEY_MAX_SCHEDULE_LEN
void (*adc_tkey_done_callback)(void) = NULL;
#endif
void (*adc_cmp_change_callback)(void) = NULL;

//HINT: The ADC shares the interrupt number with UART2, they cannot be used at the same time.
void adc_isr(void) interrupt INT_NO_ADC
{
	UINT8 channel;
	if(ADC_CTRL & bADC_IF)
	{
#if ADC_TKEY_MAX_SCHEDULE_LEN
		if(adc_tkey_pending_samples)
		{
			adc_tkey_results_raw[adc_tkey_state] = ADC_DAT;
			adc_tkey_results[adc_tkey_state] = ((adc_tkey_offsets[adc_tkey_state] - (signed short)ADC_DAT) > adc_tkey_threshold);
			
			++adc_tkey_state;
			if(adc_tkey_state == adc_tkey_schedule_len)
			{
				adc_tkey_state = 0;
				if(adc_tkey_done_callback)
				{
					adc_tkey_done_callback();
				}
			}
			
			ADC_CTRL = bADC_IF;
			if(--adc_tkey_pending_samples)
			{
				channel = adc_tkey_schedule[adc_tkey_state];
				ADC_CHAN &= 0xC0;
				ADC_CHAN |= channel & 0x3F;
				channel = channel >> 2;
				ADC_CFG &= ~(bADC_AIN_EN | bVDD_REF_EN);
				ADC_CFG |= channel & (bADC_AIN_EN | bVDD_REF_EN);
				TKEY_CTRL = ADC_TKEY_CHARGE_TIM;
			}
		}
		else
		{
			adc_results[adc_state] = ADC_DAT;
			
			++adc_state;
			if(adc_state == adc_schedule_len)
			{
				adc_state = 0;
				if(adc_done_callback)
				{
					adc_done_callback();
				}
			}
			
			ADC_CTRL = bADC_IF;
			if(--adc_pending_samples)
			{
				channel = adc_schedule[adc_state];
				ADC_CHAN &= 0xC0;
				ADC_CHAN |= channel & 0x3F;
				channel = channel >> 2;
				ADC_CFG &= ~(bADC_AIN_EN | bVDD_REF_EN);
				ADC_CFG |= channel & (bADC_AIN_EN | bVDD_REF_EN);
				ADC_CTRL = bADC_START;
			}
		}
#else
		adc_results[adc_state] = ADC_DAT;
			
		++adc_state;
		if(adc_state == adc_schedule_len)
		{
			adc_state = 0;
			if(adc_done_callback)
			{
				adc_done_callback();
			}
		}
		
		ADC_CTRL = bADC_IF;
		if(--adc_pending_samples)
		{
			channel = adc_schedule[adc_state];
			ADC_CHAN &= 0xC0;
			ADC_CHAN |= channel & 0x3F;
			channel = channel >> 2;
			ADC_CFG &= ~(bADC_AIN_EN | bVDD_REF_EN);
			ADC_CFG |= channel & (bADC_AIN_EN | bVDD_REF_EN);
			ADC_CTRL = bADC_START;
		}
#endif
	}
	
	if(ADC_CTRL & bCMP_IF)
	{
		adc_cmp_changed = 1;
		if(adc_cmp_change_callback)
		{
			adc_cmp_change_callback();
		}
		
		ADC_CTRL = bCMP_IF;
	}
}

void adc_init(UINT8 adc_clk)
{
	IE_ADC = 0;
	ADC_CFG |= bADC_EN;
	ADC_CFG &= ~(bADC_CLK1 | bADC_CLK0);
	ADC_CFG |= adc_clk;
	adc_pending_samples = 0;
#if ADC_TKEY_MAX_SCHEDULE_LEN
	adc_tkey_pending_samples = 0;
#endif
}

void adc_init_schedule(void)
{
	adc_state = 0;
	ADC_CTRL = bADC_IF;
	IE_ADC = 1;
}

void adc_run_schedule(UINT8 num_samples)
{
	UINT8 channel;
	
	channel = adc_schedule[adc_state];
	ADC_CHAN &= 0xC0;
	ADC_CHAN |= channel & 0x3F;
	channel = channel >> 2;
	ADC_CFG &= ~(bADC_AIN_EN | bVDD_REF_EN);
	ADC_CFG |= channel & (bADC_AIN_EN | bVDD_REF_EN);
	adc_pending_samples = num_samples;
	ADC_CTRL = bADC_START;
}

void adc_start_single(UINT8 channel)
{
	ADC_CHAN &= 0xC0;
	ADC_CHAN |= channel & 0x3F;
	channel = channel >> 2;
	ADC_CFG &= ~(bADC_AIN_EN | bVDD_REF_EN);
	ADC_CFG |= channel & (bADC_AIN_EN | bVDD_REF_EN);
	IE_ADC = 0;
	ADC_CTRL = bADC_START;
}

UINT16 adc_finish_single(void)
{
	while(ADC_CTRL & bADC_START) {}
	return ADC_DAT;
}

UINT16 adc_read_single(UINT8 channel)
{
	adc_start_single(channel);
	return adc_finish_single();
}
#if ADC_TKEY_MAX_SCHEDULE_LEN
void adc_tkey_init_schedule(void)
{
	adc_tkey_state = 0;
	ADC_CTRL = bADC_IF;
	IE_ADC = 1;
}

//HINT: The TKEY schedule must be initialized before calling this function.
void adc_tkey_calibrate(void)
{
	UINT8 i;
	UINT8 d;
	
	for(d = 0; d < adc_tkey_schedule_len; ++d)
	{
		adc_tkey_offsets[d] = 0;
	}

	for(d = 0; d < 16; ++d)
	{
		adc_tkey_run_schedule(adc_tkey_schedule_len);
		while(adc_tkey_pending_samples);
		for(i = 0; i < adc_tkey_schedule_len; ++i)
		{
			adc_tkey_offsets[i] += adc_tkey_results_raw[i];
		}
	}
	
	for(d = 0; d < adc_tkey_schedule_len; ++d)
	{
		adc_tkey_offsets[d] = (UINT16)adc_tkey_offsets[d] >> 4;
	}
}

void adc_tkey_run_schedule(UINT8 num_samples)
{
	UINT8 channel;
	
	channel = adc_tkey_schedule[adc_tkey_state];
	ADC_CHAN &= 0xC0;
	ADC_CHAN |= channel & 0x3F;
	channel = channel >> 2;
	ADC_CFG &= ~(bADC_AIN_EN | bVDD_REF_EN);
	ADC_CFG |= channel & (bADC_AIN_EN | bVDD_REF_EN);
	adc_tkey_pending_samples = num_samples;
	TKEY_CTRL = ADC_TKEY_CHARGE_TIM;
}

void adc_tkey_start_single(UINT8 schedule_idx)
{
	UINT8 channel;
	
	channel = adc_tkey_schedule[schedule_idx];
	ADC_CHAN &= 0xC0;
	ADC_CHAN |= channel & 0x3F;
	channel = channel >> 2;
	ADC_CFG &= ~(bADC_AIN_EN | bVDD_REF_EN);
	ADC_CFG |= channel & (bADC_AIN_EN | bVDD_REF_EN);
	IE_ADC = 0;
	TKEY_CTRL = ADC_TKEY_CHARGE_TIM;
}

UINT8 adc_tkey_finish_single(UINT8 schedule_idx)
{
	while(ADC_CTRL & bTKEY_ACT);
	return (adc_tkey_offsets[schedule_idx] - (signed short)ADC_DAT) > adc_tkey_threshold;
}

UINT8 adc_tkey_read_single(UINT8 schedule_idx)
{
	adc_tkey_start_single(schedule_idx);
	return adc_tkey_finish_single(schedule_idx);
}

UINT16 adc_tkey_finish_single_raw(void)
{
	while(ADC_CTRL & bTKEY_ACT);
	return ADC_DAT;
}

UINT16 adc_tkey_read_single_raw(UINT8 schedule_idx)
{
	adc_tkey_start_single(schedule_idx);
	return adc_tkey_finish_single_raw();
}
#endif
//HINT: Converts raw ADC value to temperature in centi-celsius (divide by 100 for integer celsius)
signed short adc_convert_temp_cc(UINT16 temp_raw)
{
	signed short deci;
	signed short centi;
	
	deci = (signed short)(((unsigned long)ADC_TEMP_SLOPE_C_V * (unsigned long)ADC_TEMP_VREF_MV / 4096) / 10) * (temp_raw - ADC_TEMP_RREF_DV);
	centi = (signed short)(((unsigned long)ADC_TEMP_SLOPE_C_V * (unsigned long)ADC_TEMP_VREF_MV / 4096) % 10) * (temp_raw - ADC_TEMP_RREF_DV);
	
	return ((ADC_TEMP_TREF_MC + 5) / 10) + deci + (centi / 10);
}

//HINT: once enabled the comparator will wake up the MCU if the output changes. This cannot be disabled except by turning off the comparator.
// The non-inverting comparator input and the ADC input cannot be set independently, changing one will change the other.
void adc_cmp_init(UINT8 inp_chan, UINT8 inn_chan)
{
	ADC_CHAN = inp_chan & 0x3F;
	inp_chan = inp_chan >> 2;
	ADC_CFG &= ~(bADC_AIN_EN | bVDD_REF_EN);
	ADC_CFG |= inp_chan & bADC_AIN_EN;
	
	ADC_CHAN |= inn_chan & 0xC0;
	ADC_CFG |= inn_chan & bVDD_REF_EN;
	
	ADC_CFG |= bCMP_EN;
	adc_cmp_changed = 0;
}

UINT8 adc_cmp_get_result(void)
{
	return ADC_CTRL >> 7;
}

UINT8 adc_cmp_get_change(void)
{
	if(adc_cmp_changed | (ADC_CTRL & bCMP_IF))
	{
		ADC_CTRL = bCMP_IF;
		adc_cmp_changed = 0;
		return 1;
	}
	else
		return 0;
}
