#ifndef _CH552_ADC_H_
#define _CH552_ADC_H_

#define ADC_USE_CMP 1
#define ADC_MAX_SCHEDULE_LEN	4

#define ADC_AIN0_P11 0
#define ADC_AIN1_P14 1
#define ADC_AIN2_P15 2
#define ADC_AIN3_P32 3

//HINT: ADC_SLOW takes 384 Fosc cycles per conversion, ADC_FAST takes 96 cycles.
#define ADC_SLOW 0
#define ADC_FAST 1

extern volatile UINT8 adc_results[ADC_MAX_SCHEDULE_LEN];
extern UINT8 adc_schedule[ADC_MAX_SCHEDULE_LEN];
extern UINT8 adc_schedule_len;
extern volatile UINT8 adc_pending_samples;

extern void (*adc_done_callback)(void);

#define adc_interrupt_enable() (IE_ADC = 1)
#define adc_interrupt_disable() (IE_ADC = 0)
#define adc_enable() (ADC_CFG |= bADC_EN)
#define adc_disable() (ADC_CFG &= ~bADC_EN)

void adc_init(UINT8 adc_clk);
void adc_init_schedule(void);
void adc_run_schedule(UINT8 num_samples);
void adc_start_single(UINT8 channel);
UINT8 adc_finish_single(void);
UINT8 adc_read_single(UINT8 channel);

//HINT: The comparator does not generate interrupts...
// The non_inverting input of the comparator is connected to the ADC input. The ADC and comparator pin selection is not independent.
#if ADC_USE_CMP
#define CMP_INP_AIN0_P11 0
#define CMP_INP_AIN1_P14 1
#define CMP_INP_AIN2_P15 2
#define CMP_INP_AIN3_P32 3
#define CMP_INN_AIN1_P14 0
#define CMP_INN_AIN3_P32 8

#define cmp_enable() (ADC_CFG |= bCMP_EN)
#define cmp_disable() (ADC_CFG &= ~bCMP_EN)

void cmp_init(UINT8 pin_cfg);
UINT8 cmp_get_result(void);
UINT8 cmp_get_change(void);
#endif

#endif
