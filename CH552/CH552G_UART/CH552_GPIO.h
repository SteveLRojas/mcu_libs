#ifndef _CH552_GPIO_H_
#define _CH552_GPIO_H_

#define GPIO_MODE_INPUT 0
#define GPIO_MODE_PP 1
#define GPIO_MODE_OD 2
#define GPIO_MODE_OD_PU 3

#define GPIO_PORT_1 1
//Do not touch port 2, it is used for paging only
#define GPIO_PORT_3 3

#define GPIO_PIN_0 0x01
#define GPIO_PIN_1 0x02
#define GPIO_PIN_2 0x04
#define GPIO_PIN_3 0x08
#define GPIO_PIN_4 0x10
#define GPIO_PIN_5 0x20
#define GPIO_PIN_6 0x40
#define GPIO_PIN_7 0x80

void gpio_set_mode(UINT8 mode, UINT8 port, UINT8 pins);
#ifdef USE_PORT_FUNCS
void gpio_write_port(UINT8 port, UINT8 write_data);
UINT8 gpio_read_port(UINT8 port);
#endif
void gpio_write_pin(UINT8 port, UINT8 pin, UINT8 write_data);
UINT8 gpio_read_pin(UINT8 port, UINT8 pin);
void gpio_set_pin(UINT8 port, UINT8 pin);
void gpio_clear_pin(UINT8 port, UINT8 pin);

#endif