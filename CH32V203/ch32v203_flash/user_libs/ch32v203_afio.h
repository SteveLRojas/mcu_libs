/*
 * ch32v203_afio.h
 *
 *  Created on: Jul 18, 2024
 *      Author: Steve
 */

#ifndef _CH32V203_AFIO_H_
#define _CH32V203_AFIO_H_

#define AFIO_PIN_0	0x00
#define AFIO_PIN_1	0x01
#define AFIO_PIN_2	0x02
#define AFIO_PIN_3	0x03
#define AFIO_PIN_4	0x04
#define AFIO_PIN_5	0x05
#define AFIO_PIN_6	0x06
#define AFIO_PIN_7	0x07
#define AFIO_PIN_8	0x08
#define AFIO_PIN_9	0x09
#define AFIO_PIN_10	0x0A
#define AFIO_PIN_11	0x0B
#define AFIO_PIN_12	0x0C
#define AFIO_PIN_13	0x0D
#define AFIO_PIN_14	0x0E
#define AFIO_PIN_15	0x0F

#define AFIO_PORT_A	0x00
#define AFIO_PORT_B	0x01
#define AFIO_PORT_C	0x02
#define AFIO_PORT_D	0x03

//HINT: Do not use defines from the GPIO library, refer to the AFIO defines in ch32v20x.h and the defines provided in this file
#define afio_event_output_config(port, pin) (AFIO->ECR = (AFIO->ECR & AFIO_ECR_EVOE) | ((port) << 4) | (pin))
#define afio_event_output_enable() (AFIO->ECR |= AFIO_ECR_EVOE)
#define afio_event_output_disable() (AFIO->ECR &= ~AFIO_ECR_EVOE)
//HINT: always restore default mapping before applying a new mapping
#define afio_pcfr1_restore_default(function) (AFIO->PCFR1 &= ~(function))
#define afio_pcfr2_restore_default(function) (AFIO->PCFR2 &= ~(function))
#define afio_pcfr1_remap(function_mapping) (AFIO->PCFR1 |= (function_mapping))
#define afio_pcfr2_remap(function_mapping) (AFIO->PCFR2 |= (function_mapping))
#define afio_exti_config(port, pin) (AFIO->EXTICR[(pin) >> 2] = (AFIO->EXTICR[(pin) >> 2] & ~((uint32_t)0x0F << (4 * ((pin) & 0x03)))) | ((uint32_t)(port) << (4 * ((pin) & 0x03))))

#endif /* _CH32V203_AFIO_H_ */
