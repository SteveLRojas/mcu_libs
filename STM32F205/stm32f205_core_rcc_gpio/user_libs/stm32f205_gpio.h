/*
 * stm32f205_gpio.h
 *
 *  Created on: Jun 11, 2025
 *      Author: Steve
 */

#ifndef _STM32F205_GPIO_H_
#define _STM32F205_GPIO_H_
// HINT: Ports are defined in stm32f205xx.h
// HINT: Modes
#define GPIO_MODE_INPUT		0x00
#define GPIO_MODE_OUTPUT	0x01
#define GPIO_MODE_AFIO		0x02
#define GPIO_MODE_ANALOG	0x03
// HINT: Types (push-pull, open drain)
#define GPIO_TYPE_PP		0x00
#define GPIO_TYPE_OD		0x01
// HINT: Speeds
#define GPIO_SPEED_LOW		0x00
#define GPIO_SPEED_MEDIUM	0x01
#define GPIO_SPEED_HIGH		0x02
#define GPIO_SPEED_HIGHEST	0x03
// HINT: pull types
#define GPIO_PULL_NONE		0x00
#define GPIO_PULL_UP		0x01
#define GPIO_PULL_DOWN		0x02
// HINT: Alternate functions
#define GPIO_FUNCTION_AF0	0x00
#define GPIO_FUNCTION_AF1	0x01
#define GPIO_FUNCTION_AF2	0x02
#define GPIO_FUNCTION_AF3	0x03
#define GPIO_FUNCTION_AF4	0x04
#define GPIO_FUNCTION_AF5	0x05
#define GPIO_FUNCTION_AF6	0x06
#define GPIO_FUNCTION_AF7	0x07
#define GPIO_FUNCTION_AF8	0x08
#define GPIO_FUNCTION_AF9	0x09
#define GPIO_FUNCTION_AF10	0x0A
#define GPIO_FUNCTION_AF11	0x0B
#define GPIO_FUNCTION_AF12	0x0C
#define GPIO_FUNCTION_AF13	0x0D
#define GPIO_FUNCTION_AF14	0x0E
#define GPIO_FUNCTION_AF15	0x0F
// HINT: Pins
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

#define gpio_write_port(port, val) ((port)->ODR = val)
#define gpio_read_port(port) ((port)->IDR)
#define gpio_write_pin(port, pin, val) ((port)->ODR = (val) ? (port)->ODR | (pin) : (port)->ODR & ~(pin))
#define gpio_read_pin(port, pin) ((port)->IDR & (pin) ? 0x01 : 0x00)
#define gpio_set_pin(port, pin) ((port)->BSRR = (pin))
#define gpio_clear_pin(port, pin) ((port)->BSRR = ((uint32_t)(pin)) << 16)
#define gpio_toggle_pin(port, pin) ((port)->ODR ^= (pin))

void gpio_set_mode(GPIO_TypeDef* port, uint32_t mode, uint32_t type, uint32_t speed, uint32_t pull, uint16_t pins);
uint16_t gpio_lock_pin(GPIO_TypeDef* port, uint32_t pins);
void gpio_set_alternate_function(GPIO_TypeDef* port, uint8_t function, uint16_t pins);

#endif /* USER_LIBS_STM32F205_GPIO_H_ */
