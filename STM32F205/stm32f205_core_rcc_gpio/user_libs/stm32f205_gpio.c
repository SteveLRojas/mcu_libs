#include "stm32f205xx.h"
#include "stm32f205_gpio.h"

void gpio_set_mode(GPIO_TypeDef* port, uint32_t mode, uint32_t type, uint32_t speed, uint32_t pull, uint16_t pins)
{
	uint32_t mask_2b = 0x03;
	uint32_t mask_1b = 0x01;

	for(uint8_t d = 0; d < 16; ++d)
	{
		if(pins & 0x01)
		{
			port->MODER &= ~mask_2b;
			port->MODER |= mode;
			port->OTYPER &= ~mask_1b;
			port->OTYPER |= type;
			port->OSPEEDR &= ~mask_2b;
			port->OSPEEDR |= speed;
			port->PUPDR &= ~mask_2b;
			port->PUPDR |= pull;
		}
		mask_2b = mask_2b << 2;
		mask_1b = mask_1b << 1;
		mode = mode << 2;
		type = type << 1;
		speed = speed << 2;
		pull = pull << 2;
		pins = pins >> 1;
	}
}

uint16_t gpio_lock_pin(GPIO_TypeDef* port, uint32_t pins)
{
	port->LCKR = pins | 0x00010000;
	port->LCKR = pins;
	port->LCKR = pins | 0x00010000;
	pins = port->LCKR;
	return (uint16_t)(pins | port->LCKR);
}

void gpio_set_alternate_function(GPIO_TypeDef* port, uint8_t function, uint16_t pins)
{
	uint32_t mask = 0x0F;
	uint32_t wide_function = function;

	for(uint8_t d = 0; d < 8; ++d)
	{
		if(pins & 0x01)
		{
			port->AFR[0] &= ~mask;
			port->AFR[0] |= wide_function;
		}
		mask = mask << 4;
		wide_function = wide_function << 4;
		pins = pins >> 1;
	}

	mask = 0x0F;
	wide_function = function;

	for(uint8_t d = 0; d < 8; ++d)
	{
		if(pins & 0x01)
		{
			port->AFR[1] &= ~mask;
			port->AFR[1] |= wide_function;
		}
		mask = mask << 4;
		wide_function = wide_function << 4;
		pins = pins >> 1;
	}
}
