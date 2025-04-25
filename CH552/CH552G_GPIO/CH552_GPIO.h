#ifndef _CH552_GPIO_H_
#define _CH552_GPIO_H_

#define USE_PORT_FUNCS 0
#define USE_GPIO_INTERRUPTS 1

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

#if USE_GPIO_INTERRUPTS
#define GPIO_INT_MODE_LEVEL	0x00
#define GPIO_INT_MODE_EDGE	0x80

#define GPIO_INT_RXD1_FALL	0x40
#define GPIO_INT_P1_5_FALL	0x20
#define GPIO_INT_P1_4_FALL	0x10
#define GPIO_INT_P1_3_FALL	0x08
#define GPIO_INT_RST_RISE	0x04
#define GPIO_INT_P3_1_FALL	0x02
#define GPIO_INT_RXD0_FALL	0x01

extern void (*gpio_callback)(void);

#define gpio_global_interrupt_enable() (IE_GPIO = 1)
#define gpio_global_interrupt_disable() (IE_GPIO = 0)
#define gpio_set_interrupt_mode(mode) (GPIO_IE = (GPIO_IE & ~bIE_IO_EDGE) | (mode))
#define gpio_enable_interrupts(interrupts) (GPIO_IE |= (interrupts))
#define gpio_disable_interrupts(interrupts) (GPIO_IE &= ~(interrupts))
#endif

void gpio_set_mode(UINT8 mode, UINT8 port, UINT8 pins);
#if USE_PORT_FUNCS
void gpio_write_port(UINT8 port, UINT8 write_data);
UINT8 gpio_read_port(UINT8 port);
#endif
void gpio_write_pin(UINT8 port, UINT8 pin, UINT8 write_data);
UINT8 gpio_read_pin(UINT8 port, UINT8 pin);
void gpio_set_pin(UINT8 port, UINT8 pin);
void gpio_clear_pin(UINT8 port, UINT8 pin);

#endif