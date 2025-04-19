#ifndef _CH559_GPIO_H_
#define _CH559_GPIO_H_

#define GPIO_PORT_MODE_PP 0
#define GPIO_PORT_MODE_OC 1

// HINT: GPIO_PORT_STRENGTH_20 results in 10mA strength for port P1
#define GPIO_PORT_STRENGTH_5 0
#define GPIO_PORT_STRENGTH_20 1

// HINT: For use with ports in PP mode
#define GPIO_MODE_INPUT 0
#define GPIO_MODE_INPUT_PU 1
#define GPIO_MODE_OUTPUT_PP 2

// HINT: For use with ports in OC mode
#define GPIO_MODE_OC 0
#define GPIO_MODE_OC_PU 1
#define GPIO_MODE_OC_PULSE 2
#define GPIO_MODE_OC_PU_PULSE 3

#define GPIO_PORT_0 0
#define GPIO_PORT_1 1
#define GPIO_PORT_2 2
#define GPIO_PORT_3 3
#define GPIO_PORT_4 4

#define GPIO_PIN_0 0x01
#define GPIO_PIN_1 0x02
#define GPIO_PIN_2 0x04
#define GPIO_PIN_3 0x08
#define GPIO_PIN_4 0x10
#define GPIO_PIN_5 0x20
#define GPIO_PIN_6 0x40
#define GPIO_PIN_7 0x80

// HINT: for ports 0 to 3
#define gpio_set_port_mode(mode, port) (PORT_CFG = (PORT_CFG & ~(0x01 << (port))) | ((mode) << (port)))
#define gpio_set_port_strength(strength, port) (PORT_CFG = (PORT_CFG & ~(0x10 << (port))) | ((strength) << ((port) + 4)))

// HINT: P4 is always in PP mode
#define gpio_set_p4_strength(strength) (P4_CFG = (P4_CFG & ~0x04) | ((strength) << 2))

void gpio_set_pin_mode(UINT8 mode, UINT8 port, UINT8 pins);
#ifdef USE_PORT_FUNCS
void gpio_write_port(UINT8 port, UINT8 write_data);
UINT8 gpio_read_port(UINT8 port);
#endif
void gpio_write_pin(UINT8 port, UINT8 pin, UINT8 write_data);
UINT8 gpio_read_pin(UINT8 port, UINT8 pin);
void gpio_set_pin(UINT8 port, UINT8 pin);
void gpio_clear_pin(UINT8 port, UINT8 pin);
void gpio_toggle_pin(UINT8 port, UINT8 pin);

#endif