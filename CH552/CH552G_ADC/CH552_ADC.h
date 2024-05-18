#ifndef _CH552_ADC_H_
#define _CH552_ADC_H_

#define ADC_AIN0 0
#define ADC_AIN1 1
#define ADC_AIN2 2
#define ADC_AIN3 3

#define ADC_P11 0
#define ADC_P14 1
#define ADC_P15 2
#define ADC_P32 3

#define ADC_SLOW 0
#define ADC_FAST 1

extern UINT8 adc_results[4];
extern UINT8 adc_schedule[4];
extern UINT8 adc_schedule_len;

void adc_init(UINT8 adc_clk);
void adc_init_schedule(void);
void adc_run_schedule(UINT8 num_samples);
void adc_start_single(UINT8 channel);
UINT8 adc_finish_single(void);
UINT8 adc_read_single(UINT8 channel);

//HINT: The comparator is mostly useless since it cannot generate interrupts...
#ifdef USE_CMP
#define CMP_INP_AIN0 0
#define CMP_INP_AIN2 2
#define CMP_INN_AIN1 0
#define CMP_INN_AIN3 8

#define CMP_INP_P11 0
#define CMP_INP_P15 2
#define CMP_INN_P14 0
#define CMP_INN_P32 8

void cmp_init(UINT8 pin_cfg);
UINT8 cmp_get_result(void);
UINT8 cmp_get_change(void);
#endif

#endif
