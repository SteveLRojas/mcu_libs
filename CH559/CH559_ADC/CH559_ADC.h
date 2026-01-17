#ifndef _CH559_ADC_H_
#define _CH559_ADC_H_

#define ADC_MAX_SCHEDULE_LEN	8

// HINT: set to 0 for fixed length pulse, 1 for ADC clk sampling.
#define ADC_TRIG_MODE 0

// HINT: pulse length for fixed length trigger mode. Higher values give longer delay (1 to 255).
#define ADC_PULSE_LEN 4

#define ADC_AIN0 0x01
#define ADC_AIN1 0x02
#define ADC_AIN2 0x04
#define ADC_AIN3 0x08
#define ADC_AIN4 0x10
#define ADC_AIN5 0x20
#define ADC_AIN6 0x40
#define ADC_AIN7 0x80

//HINT: OR one of these defines with a clock divider in the range [1,127] to set the ADC clock speed.
// The ADC clock speed must be <= 12MHz. ADC_SLOW uses fsys, ADC_FAST uses fsys * 4.
#define ADC_SLOW 0x00
#define ADC_FAST 0x80

extern volatile UINT16 adc_results[ADC_MAX_SCHEDULE_LEN];
extern UINT8 adc_schedule[ADC_MAX_SCHEDULE_LEN];
extern UINT8 adc_schedule_len;
extern volatile UINT8 adc_pending_samples;

extern void (*adc_done_callback)(void);

#define adc_interrupt_enable() (IE_ADC = 1)
#define adc_interrupt_disable() (IE_ADC = 0)
#define adc_enable() (ADC_SETUP |= bADC_POWER_EN)
#define adc_disable() (ADC_SETUP &= ~bADC_POWER_EN)

#define adc_digital_in_disable(pins) (P1_IE &= ~(pins))
#define adc_digital_in_enable(pins) (P1_IE |= (pins))

void adc_init(UINT8 clk_config, UINT8 pins);
void adc_init_schedule(void);
void adc_run_schedule(UINT8 num_samples);
void adc_start_single(UINT8 channel);
UINT16 adc_finish_single(void);
UINT16 adc_read_single(UINT8 channel);

#endif
