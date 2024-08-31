#include "CH552.H"
#include "System.h"
#include "CH552_TKEY.h"

volatile UINT16 tkey_results_raw[6];
volatile UINT8 tkey_results[6];

UINT8 tkey_schedule[6];
UINT8 tkey_schedule_len;
volatile UINT8 tkey_pending_samples;
signed short tkey_threshold = TKEY_DEFAULT_THRESHOLD;

UINT8 tkey_sample_index;
UINT8 tkey_result_index;
signed short tkey_offsets[6];

void tkey_isr(void) interrupt INT_NO_TKEY
{
	UINT8 channel;
	UINT16 sample = TKEY_DAT;
	TKEY_CTRL &= ~(bTKC_CHAN2 | bTKC_CHAN1 | bTKC_CHAN0);
	TKEY_CTRL |= tkey_schedule[tkey_sample_index];
	
	channel = tkey_schedule[tkey_result_index] - 1;
	tkey_results_raw[tkey_result_index] = sample & 0x3FFF;
	tkey_results[tkey_result_index] = (tkey_offsets[channel] - ((signed short)(sample & 0x3FFF))) > tkey_threshold;
	tkey_result_index = tkey_sample_index;
	
	if(--tkey_pending_samples)
	{
		++tkey_sample_index;
		if(tkey_sample_index == tkey_schedule_len)
			tkey_sample_index = 0;
	}
	else
	{
		IE_TKEY = 0;
	}
}

void tkey_init(UINT8 tkey_speed)
{
	IE_TKEY = 0;
	TKEY_CTRL = tkey_speed;
}

void tkey_init_schedule(void)
{
	tkey_sample_index = 0;
	tkey_result_index = 0;
	IE_TKEY = 0;
}

void tkey_calibrate(void)
{
	UINT8 i;
	UINT8 d;
	UINT8 channel;
	
	for(d = 0; d < 6; ++d)
	{
		tkey_offsets[d] = 0;
	}

	for(d = 0; d < 4; ++d)
	{
		tkey_run_schedule(tkey_schedule_len);
		while(tkey_pending_samples);
		for(i = 0; i < 6; ++i)
		{
			channel = tkey_schedule[i] - 1;
			tkey_offsets[channel] += tkey_results_raw[i];
		}
	}
	
	for(d = 0; d < 6; ++d)
	{
		tkey_offsets[d] = (UINT16)tkey_offsets[d] >> 2;
	}
}

void tkey_run_schedule(UINT8 num_samples)
{
	TKEY_CTRL &= ~(bTKC_CHAN2 | bTKC_CHAN1 | bTKC_CHAN0);
	TKEY_CTRL |= tkey_schedule[tkey_sample_index];
	++tkey_sample_index;
	tkey_pending_samples = num_samples;
	IE_TKEY = 1;
}

void tkey_start_single(UINT8 channel)
{
	IE_TKEY = 0;
	TKEY_CTRL &= ~(bTKC_CHAN2 | bTKC_CHAN1 | bTKC_CHAN0);
	TKEY_CTRL |= channel;
}

UINT8 tkey_finish_single(void)
{
	signed short sample;
	UINT8 channel = (TKEY_CTRL & (bTKC_CHAN2 | bTKC_CHAN1 | bTKC_CHAN0)) - 1;
	while(!(TKEY_CTRL & bTKC_IF));
	sample = (TKEY_DAT & 0x3FFF);
	return (tkey_offsets[channel] - sample) > tkey_threshold;
}

UINT8 tkey_read_single(UINT8 channel)
{
	tkey_start_single(channel);
	return tkey_finish_single();
}

UINT16 tkey_finish_single_raw(void)
{
	while(!(TKEY_CTRL & bTKC_IF));
	return (TKEY_DAT & 0x3FFF);
}

UINT16 tkey_read_single_raw(UINT8 channel)
{
	tkey_start_single(channel);
	return tkey_finish_single_raw();
}
