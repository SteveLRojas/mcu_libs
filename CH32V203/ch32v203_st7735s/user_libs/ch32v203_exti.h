/*
 * ch32v203_exti.h
 *
 *  Created on: Mar 13, 2025
 *      Author: Steve
 */

#ifndef _CH32V203_EXTI_H_
#define _CH32V203_EXTI_H_

#define EXTI_CHAN_0		((uint32_t)0x00000001)
#define EXTI_CHAN_1		((uint32_t)0x00000002)
#define EXTI_CHAN_2		((uint32_t)0x00000004)
#define EXTI_CHAN_3		((uint32_t)0x00000008)
#define EXTI_CHAN_4		((uint32_t)0x00000010)
#define EXTI_CHAN_5		((uint32_t)0x00000020)
#define EXTI_CHAN_6		((uint32_t)0x00000040)
#define EXTI_CHAN_7		((uint32_t)0x00000080)
#define EXTI_CHAN_8		((uint32_t)0x00000100)
#define EXTI_CHAN_9		((uint32_t)0x00000200)
#define EXTI_CHAN_10	((uint32_t)0x00000400)
#define EXTI_CHAN_11	((uint32_t)0x00000800)
#define EXTI_CHAN_12	((uint32_t)0x00001000)
#define EXTI_CHAN_13	((uint32_t)0x00002000)
#define EXTI_CHAN_14	((uint32_t)0x00004000)
#define EXTI_CHAN_15	((uint32_t)0x00008000)
//HINT: Channels beyond 15 are intended to be used by other libraries, not user code.
#define EXTI_CHAN_16	((uint32_t)0x00010000)
#define EXTI_CHAN_17	((uint32_t)0x00020000)
#define EXTI_CHAN_18	((uint32_t)0x00040000)
#define EXTI_CHAN_19	((uint32_t)0x00080000)

extern void (*exti0_callback)(void);
extern void (*exti1_callback)(void);
extern void (*exti2_callback)(void);
extern void (*exti3_callback)(void);
extern void (*exti4_callback)(void);
extern void (*exti5_callback)(void);
extern void (*exti6_callback)(void);
extern void (*exti7_callback)(void);
extern void (*exti8_callback)(void);
extern void (*exti9_callback)(void);
extern void (*exti10_callback)(void);
extern void (*exti11_callback)(void);
extern void (*exti12_callback)(void);
extern void (*exti13_callback)(void);
extern void (*exti14_callback)(void);
extern void (*exti15_callback)(void);

#define exti_enable_interrupt(channel) (EXTI->INTENR |= (channel))
#define exti_disable_interrupt(channel) (EXTI->INTENR &= ~(channel))
#define exti_enable_event(channel) (EXTI->EVENR |= (channel))
#define exti_disable_event(channel) (EXTI->EVENR &= ~(channel))
#define exti_enable_rising_edge(channel) (EXTI->RTENR |= (channel))
#define exti_disable_rising_edge(channel) (EXTI->RTENR &= ~(channel))
#define exti_enable_falling_edge(channel) (EXTI->FTENR |= (channel))
#define exti_disable_falling_edge(channel) (EXTI->FTENR &= ~(channel))
#define exti_trigger_software_interrupt(channel) (EXTI->SWIEVR |= (channel))
#define exti_get_interrupt_flags() (EXTI->INTFR)
#define exti_clear_interrupt_flag(channel) (EXTI->INTFR = (channel))

#endif /* _CH32V203_EXTI_H_ */
