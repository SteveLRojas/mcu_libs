/*
 * ch32v203_afio.h
 *
 *  Created on: Jul 18, 2024
 *      Author: Steve
 */

#ifndef _CH32V203_AFIO_H_
#define _CH32V203_AFIO_H_

//HINT: Do not use defines from the GPIO library, refer to the AFIO defines in ch32v20x.h
#define afio_event_output_config(port, pin) (AFIO->ECR = (AFIO->ECR & AFIO_ECR_EVOE) | (port) | (pin))
#define afio_event_output_enable() (AFIO->ECR |= AFIO_ECR_EVOE)
#define afio_event_output_disable() (AFIO->ECR &= ~AFIO_ECR_EVOE)
//HINT: always restore default mapping before applying a new mapping
#define afio_pcfr1_restore_default(function) (AFIO->PCFR1 &= ~(function))
#define afio_pcfr2_restore_default(function) (AFIO->PCFR2 &= ~(function))
#define afio_pcfr1_remap(function_mapping) (AFIO->PCFR1 |= (function_mapping))
#define afio_pcfr2_remap(function_mapping) (AFIO->PCFR2 |= (function_mapping))
#define afio_exti_config(port, pin) (AFIO->EXTICR[(pin) >> 2] = (AFIO->EXTICR[(pin) >> 2] & ~((uint32_t)0x0F << (4 * ((pin) & 0x03)))) | ((uint32_t)(port) << (4 * ((pin) & 0x03))))

#endif /* _CH32V203_AFIO_H_ */
