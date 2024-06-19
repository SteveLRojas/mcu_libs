#ifndef _CH559_ADC_H_
#define _CH559_ADC_H_

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

#define ADC_SLOW 0x00
#define ADC_FAST 0x80

extern UINT16 adc_results[8];
extern UINT8 adc_schedule[8];
extern UINT8 adc_schedule_len;

void adc_init(UINT8 clk_config, UINT8 pins);
void adc_init_schedule(void);
void adc_run_schedule(UINT8 num_samples);
void adc_start_single(UINT8 channel);
UINT16 adc_finish_single(void);
UINT16 adc_read_single(UINT8 channel);

#endif
