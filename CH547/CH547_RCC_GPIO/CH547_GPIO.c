#include "CH547.H"
#include "CH547_GPIO.h"

UINT8 p0_last_write = 0xFF;
UINT8 p1_last_write = 0xFF;
UINT8 p2_last_write = 0xFF;
UINT8 p3_last_write = 0xFF;
UINT8 p4_last_write = 0xFF;
UINT8 p5_last_write = 0xFF;

#if USE_GPIO_INTERRUPTS
void (*gpio_callback)(void) = NULL;

void gpio_isr(void) interrupt INT_NO_GPIO
{
	if(gpio_callback)
		gpio_callback();
}
#endif

void gpio_set_mode(UINT8 mode, UINT8 port, UINT8 pins)
{
	UINT8 Pn_MOD_OC;
	UINT8 Pn_DIR_PU;
	
	switch(port)
	{
		case GPIO_PORT_0:
			Pn_MOD_OC = P0_MOD_OC;
			Pn_DIR_PU = P0_DIR_PU;
			break;
		case GPIO_PORT_1:
			Pn_MOD_OC = P1_MOD_OC;
			Pn_DIR_PU = P1_DIR_PU;
			break;
		case GPIO_PORT_2:
			Pn_MOD_OC = P2_MOD_OC;
			Pn_DIR_PU = P2_DIR_PU;
			break;
		case GPIO_PORT_3:
			Pn_MOD_OC = P3_MOD_OC;
			Pn_DIR_PU = P3_DIR_PU;
			break;
		case GPIO_PORT_4:
			Pn_MOD_OC = P4_MOD_OC;
			Pn_DIR_PU = P4_DIR_PU;
			break;
		default:
			return;
	}
	
	switch(mode)
	{
		case GPIO_MODE_INPUT:
			//clear bits in Pn_MOD_OC and Pn_DIR_PU
			Pn_MOD_OC &= ~pins;
			Pn_DIR_PU &= ~pins;
			break;
		case GPIO_MODE_PP:
			//clear bits in Pn_MOD_OC, set bits in Pn_DIR_PU
			Pn_MOD_OC &= ~pins;
			Pn_DIR_PU |= pins;
			break;
		case GPIO_MODE_OD:
			//set bits in Pn_MOD_OC, clear bits in Pn_DIR_PU
			Pn_MOD_OC |= pins;
			Pn_DIR_PU &= ~pins;
			break;
		case GPIO_MODE_OD_PU:
			//set bits in Pn_MOD_OC and Pn_DIR_PU
			Pn_MOD_OC |= pins;
			Pn_DIR_PU |= pins;
			break;
		default:
			return;
	}
	
	switch(port)
	{
		case GPIO_PORT_0:
			P0_MOD_OC = Pn_MOD_OC;
			P0_DIR_PU = Pn_DIR_PU;
			break;
		case GPIO_PORT_1:
			P1_MOD_OC = Pn_MOD_OC;
			P1_DIR_PU = Pn_DIR_PU;
			break;
		case GPIO_PORT_2:
			P2_MOD_OC = Pn_MOD_OC;
			P2_DIR_PU = Pn_DIR_PU;
			break;
		case GPIO_PORT_3:
			P3_MOD_OC = Pn_MOD_OC;
			P3_DIR_PU = Pn_DIR_PU;
			break;
		case GPIO_PORT_4:
			P4_MOD_OC = Pn_MOD_OC;
			P4_DIR_PU = Pn_DIR_PU;
			break;
		default:
			return;
	}
}

#if USE_PORT_FUNCS
void gpio_write_port(UINT8 port, UINT8 write_data)
{
	switch(port)
	{
		case GPIO_PORT_0:
			P0 = (p0_last_write = write_data);
			break;
		case GPIO_PORT_1:
			P1 = (p1_last_write = write_data);
			break;
		case GPIO_PORT_2:
			P2 = (p2_last_write = write_data);
			break;
		case GPIO_PORT_3:
			P3 = (p3_last_write = write_data);
			break;
		case GPIO_PORT_4:
			P4 = (p4_last_write = write_data);
			break;
		case GPIO_PORT_5:
			P5 = (p5_last_write = write_data);
			break;
		default:
			return;
	}
}

UINT8 gpio_read_port(UINT8 port)
{
	switch(port)
	{
		case GPIO_PORT_0:
			return P0;
		case GPIO_PORT_1:
			return P1;
		case GPIO_PORT_2:
			return P2;
		case GPIO_PORT_3:
			return P3;
		case GPIO_PORT_4:
			return P4;
		case GPIO_PORT_5:
			return P5;
		default:
			return 0;
	}
}
#endif

void gpio_write_pin(UINT8 port, UINT8 pin, UINT8 write_data)
{
	switch(port)
	{
		case GPIO_PORT_0:
			P0 = write_data ? (p0_last_write |= pin) : (p0_last_write &= ~pin);
			break;
		case GPIO_PORT_1:
			P1 = write_data ? (p1_last_write |= pin) : (p1_last_write &= ~pin);
			break;
		case GPIO_PORT_2:
			P2 = write_data ? (p2_last_write |= pin) : (p2_last_write &= ~pin);
			break;
		case GPIO_PORT_3:
			P3 = write_data ? (p3_last_write |= pin) : (p3_last_write &= ~pin);
			break;
		case GPIO_PORT_4:
			P4 = write_data ? (p4_last_write |= pin) : (p4_last_write &= ~pin);
			break;
		case GPIO_PORT_5:
			P5 = write_data ? (p5_last_write |= pin) : (p5_last_write &= ~pin);
			break;
		default:
			return;
	}
}

UINT8 gpio_read_pin(UINT8 port, UINT8 pin)
{
	switch(port)
	{
		case GPIO_PORT_0:
			return (P0 & pin) ? 0x01 : 0x00;
		case GPIO_PORT_1:
			return (P1 & pin) ? 0x01 : 0x00;
		case GPIO_PORT_2:
			return (P2 & pin) ? 0x01 : 0x00;
		case GPIO_PORT_3:
			return (P3 & pin) ? 0x01 : 0x00;
		case GPIO_PORT_4:
			return (P4 & pin) ? 0x01 : 0x00;
		case GPIO_PORT_5:
			return (P5 & pin) ? 0x01 : 0x00;
		default:
			return 0;
	}
}

void gpio_set_pin(UINT8 port, UINT8 pin)
{
	switch(port)
	{
		case GPIO_PORT_0:
			P0 = (p0_last_write |= pin);
			break;
		case GPIO_PORT_1:
			P1 = (p1_last_write |= pin);
			break;
		case GPIO_PORT_2:
			P2 = (p2_last_write |= pin);
			break;
		case GPIO_PORT_3:
			P3 = (p3_last_write |= pin);
			break;
		case GPIO_PORT_4:
			P4 = (p4_last_write |= pin);
			break;
		case GPIO_PORT_5:
			P5 = (p5_last_write |= pin);
			break;
		default:
			return;
	}
}

void gpio_clear_pin(UINT8 port, UINT8 pin)
{
	switch(port)
	{
		case GPIO_PORT_0:
			P0 = (p0_last_write &= ~pin);
			break;
		case GPIO_PORT_1:
			P1 = (p1_last_write &= ~pin);
			break;
		case GPIO_PORT_2:
			P2 = (p2_last_write &= ~pin);
			break;
		case GPIO_PORT_3:
			P3 = (p3_last_write &= ~pin);
			break;
		case GPIO_PORT_4:
			P4 = (p4_last_write &= ~pin);
			break;
		case GPIO_PORT_5:
			P5 = (p5_last_write &= ~pin);
			break;
		default:
			return;
	}
}

void gpio_toggle_pin(UINT8 port, UINT8 pin)
{
	switch(port)
	{
		case GPIO_PORT_0:
			P0 = (p0_last_write ^= pin);
			break;
		case GPIO_PORT_1:
			P1 = (p1_last_write ^= pin);
			break;
		case GPIO_PORT_2:
			P2 = (p2_last_write ^= pin);
			break;
		case GPIO_PORT_3:
			P3 = (p3_last_write ^= pin);
			break;
		case GPIO_PORT_4:
			P4 = (p4_last_write ^= pin);
			break;
		case GPIO_PORT_5:
			P5 = (p5_last_write ^= pin);
			break;
		default:
			return;
	}
}
