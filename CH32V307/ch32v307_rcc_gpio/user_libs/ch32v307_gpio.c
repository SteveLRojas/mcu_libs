/*
 * ch32v307_gpio.c
 *
 *  Created on: Jul 19, 2025
 *      Author: Steve
 */
#include "ch32v30x.h"
#include "ch32v307_gpio.h"

void gpio_set_mode(GPIO_TypeDef* port, uint8_t mode, uint16_t pins)
{
	uint32_t mask = 0x0F;
	uint32_t wide_mode = mode;

	for(uint8_t d = 0; d < 8; ++d)
	{
		if(pins & 0x01)
		{
			port->CFGLR &= ~mask;
			port->CFGLR |= wide_mode;
		}
		mask = mask << 4;
		wide_mode = wide_mode << 4;
		pins = pins >> 1;
	}

	mask = 0x0F;
	wide_mode = mode;

	for(uint8_t d = 0; d < 8; ++d)
	{
		if(pins & 0x01)
		{
			port->CFGHR &= ~mask;
			port->CFGHR |= wide_mode;
		}
		mask = mask << 4;
		wide_mode = wide_mode << 4;
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
