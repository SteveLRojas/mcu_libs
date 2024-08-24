/*
 * ch32v203_gpio.h
 *
 *  Created on: Jun 26, 2024
 *      Author: Steve
 */

#ifndef _CH32V203_GPIO_H_
#define _CH32V203_GPIO_H_

// HINT: Input modes
#define GPIO_MODE_ANALOG_IN 0x00
#define GPIO_MODE_FLOAT_IN 0x04
#define GPIO_MODE_PULL_IN 0x08
// HINT: Output modes
#define GPIO_MODE_PP_OUT 0x00
#define GPIO_MODE_OD_OUT 0x04
#define GPIO_MODE_AFIO_PP 0x08
#define GPIO_MODE_AFIO_OD 0x0C

#define GPIO_DIR_SPD_IN 0x00
#define GPIO_DIR_SPD_OUT_10MHZ 0x01
#define GPIO_DIR_SPD_OUT_2MHZ 0x02
#define GPIO_DIR_SPD_OUT_50MHZ 0x03

#define GPIO_PIN_0 0x0001
#define GPIO_PIN_1 0x0002
#define GPIO_PIN_2 0x0004
#define GPIO_PIN_3 0x0008
#define GPIO_PIN_4 0x0010
#define GPIO_PIN_5 0x0020
#define GPIO_PIN_6 0x0040
#define GPIO_PIN_7 0x0080
#define GPIO_PIN_8 0x0100
#define GPIO_PIN_9 0x0200
#define GPIO_PIN_10 0x0400
#define GPIO_PIN_11 0x0800
#define GPIO_PIN_12 0x1000
#define GPIO_PIN_13 0x2000
#define GPIO_PIN_14 0x4000
#define GPIO_PIN_15 0x8000

#define gpio_write_port(port, val) ((port)->OUTDR = val)
#define gpio_read_port(port) ((port)->INDR)
#define gpio_write_pin(port, pin, val) ((port)->OUTDR = (val) ? (port)->OUTDR | (pin) : (port)->OUTDR & ~(pin))
#define gpio_read_pin(port, pin) ((port)->INDR & (pin) ? 0x01 : 0x00)
#define gpio_set_pin(port, pin) ((port)->BSHR = (pin))
#define gpio_clear_pin(port, pin) ((port)->BCR = (pin))
#define gpio_toggle_pin(port, pin) ((port)->OUTDR ^= (pin))

void gpio_set_mode(GPIO_TypeDef* port, uint8_t mode, uint16_t pins);
uint16_t gpio_lock_pin(GPIO_TypeDef* port, uint16_t pins);

#endif /* _CH32V203_GPIO_H_ */
