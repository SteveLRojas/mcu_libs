/*
 * ch32v203_adc.h
 *
 *  Created on: Apr 26, 2025
 *      Author: Steve
 */

#ifndef _CH32V203_ADC_H_
#define _CH32V203_ADC_H_

extern uint16_t* adc1_results;
extern const uint8_t* adc1_schedule;
extern uint8_t adc1_schedule_len;
extern uint8_t adc1_pending_samples;

extern uint16_t* adc2_results;
extern const uint8_t* adc2_schedule;
extern uint8_t adc2_schedule_len;
extern uint8_t adc2_pending_samples;

#define ADC_IN0		0x00
#define ADC_IN1		0x01
#define ADC_IN2		0x02
#define ADC_IN3		0x03
#define ADC_IN4		0x04
#define ADC_IN5		0x05
#define ADC_IN6		0x06
#define ADC_IN7		0x07
#define ADC_IN8		0x08
#define ADC_IN9		0x09
#define ADC_IN10	0x0A
#define ADC_IN11	0x0B
#define ADC_IN12	0x0C
#define ADC_IN13	0x0D
#define ADC_IN14	0x0E
#define ADC_IN15	0x0F
#define ADC_IN_TEMP	0x10
#define ADC_IN_VREF	0x11

#define adc_enable_interrupt(adc) ((adc)->CTLR1 |= ADC_EOCIE)
#define adc_disable_interrupt(adc) ((adc)->CTLR1 &= ~ADC_EOCIE)

void adc_init(ADC_TypeDef* adc);
void adc_init_schedule(ADC_TypeDef* adc);
void adc_run_schedule(ADC_TypeDef* adc, uint8_t num_samples);
void adc_start_single(ADC_TypeDef* adc, uint8_t channel);
uint16_t adc_finish_single(ADC_TypeDef* adc);
uint16_t adc_read_single(ADC_TypeDef* adc, uint8_t channel);

#endif /* _CH32V203_ADC_H_ */
