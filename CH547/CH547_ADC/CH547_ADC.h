#ifndef _CH547_ADC_H_
#define _CH547_ADC_H_

//HINT: Temperature sensor readings are noisy and change significantly depending on the ADC sample rate.
#define ADC_MAX_SCHEDULE_LEN	8
#define ADC_TEMP_VREF_MV	3332
#define ADC_TEMP_TREF_MC	24500
#define ADC_TEMP_RREF_DV	2050
#define ADC_TEMP_SLOPE_C_V	200

#define ADC_CLK_750K	0x00
#define ADC_CLK_1M5		0x01
#define ADC_CLK_3M		0x02
#define ADC_CLK_6M		0x03

#define ADC_CHAN_VDD50		0x40
#define ADC_CHAN_V33		0x50
#define ADC_CHAN_NOISE		0x60
#define ADC_CHAN_TEMP		0x70
#define ADC_CHAN_AIN0_P10	0xF0
#define ADC_CHAN_AIN1_P11	0xF1
#define ADC_CHAN_AIN2_P12	0xF2
#define ADC_CHAN_AIN3_P13	0xF3
#define ADC_CHAN_AIN4_P14	0xF4
#define ADC_CHAN_AIN5_P15	0xF5
#define ADC_CHAN_AIN6_P16	0xF6
#define ADC_CHAN_AIN7_P17	0xF7
#define ADC_CHAN_AIN8_P00	0xF8
#define ADC_CHAN_AIN9_P01	0xF9
#define ADC_CHAN_AIN10_P02	0xFA
#define ADC_CHAN_AIN11_P03	0xFB

//HINT: Use thse defines to enable/disable the digital input buffers.
#define ADC_DI_AIN0_AIN1	0x01
#define ADC_DI_AIN2_AIN3	0x02
#define ADC_DI_AIN4_AIN5	0x04
#define ADC_DI_AIN6_AIN7	0x08
#define ADC_DI_AIN8_AIN9	0x10
#define ADC_DI_AIN10_AIN11	0x20

//HINT The non-inverting comparator input and the ADC input cannot be set independently, changing one will change the other.
// The inverting and non-inverting inputs of the comparator cannot be set completely indpendently.
// The 100% VDD option for the inverting input conflicts with the 50% VDD option for the non-inverting input.
// The 100% VDD option will be overwritten with the 25% VDD option any time the ADC is used, including when the schedule is run.

//HINT: Use the ADC defines to set the comparator non-inverting input.
#define CMP_INN_VDD12P5		0x10
#define CMP_INN_VDD100		0x40
#define CMP_INN_VDD25		0x50
#define CMP_INN_AIN1_P11	0x90
#define CMP_INN_AIN2_P12	0xD0

extern volatile UINT16 adc_results[ADC_MAX_SCHEDULE_LEN];
extern UINT8 adc_schedule[ADC_MAX_SCHEDULE_LEN];
extern UINT8 adc_schedule_len;
extern volatile UINT8 adc_pending_samples;
extern volatile UINT8 cmp_changed;

extern void (*adc_done_callback)(void);
extern void (*cmp_change_callback)(void);

#define adc_interrupt_enable() (IE_ADC = 1)
#define adc_interrupt_disable() (IE_ADC = 0)
#define adc_enable() (ADC_CFG |= bADC_EN)
#define adc_disable() (ADC_CFG &= ~bADC_EN)

#define adc_digital_in_disable(pins) (ADC_PIN |= (pins))
#define adc_digital_in_enable(pins) (ADC_PIN &= ~(pins))

#define cmp_enable() (ADC_CFG |= bCMP_EN)
#define cmp_disable() (ADC_CFG &= ~bCMP_EN)

void adc_init(UINT8 adc_clk);
void adc_init_schedule(void);
void adc_run_schedule(UINT8 num_samples);
void adc_start_single(UINT8 channel);
UINT16 adc_finish_single(void);
UINT16 adc_read_single(UINT8 channel);
signed short adc_convert_temp_cc(UINT16 temp_raw);

void cmp_init(UINT8 inp_chan, UINT8 inn_chan);
UINT8 cmp_get_result(void);
UINT8 cmp_get_change(void);

#endif
