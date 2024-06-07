#define NO_XSFR_DEFINE
#include "CH559.H"
#include "CH559_GPIO.h"

UINT8 p0_last_write = 0xFF;
UINT8 p1_last_write = 0xFF;
UINT8 p2_last_write = 0xFF;
UINT8 p3_last_write = 0xFF;

void gpio_set_pin_mode(UINT8 mode, UINT8 port, UINT8 pins)
{
	UINT8 pn_dir;
	UINT8 pn_pu;
	
	switch(port)
	{
		case GPIO_PORT_0:
			pn_dir = P0_DIR;
			pn_pu = P0_PU;
			break;
		case GPIO_PORT_1:
			pn_dir = P1_DIR;
			pn_pu = P1_PU;
			break;
		case GPIO_PORT_2:
			pn_dir = P2_DIR;
			pn_pu = P2_PU;
			break;
		case GPIO_PORT_3:
			pn_dir = P3_DIR;
			pn_pu = P3_PU;
			break;
		case GPIO_PORT_4:
			pn_dir = P4_DIR;
			pn_pu = P4_PU;
			break;
		default:
			return;
	}
	
	switch(mode)
	{
		case GPIO_MODE_OC:
			pn_dir &= ~pins;
			pn_pu  &= ~pins;
			break;
		case GPIO_MODE_OC_PU:
			pn_dir &= ~pins;
			pn_pu |= pins;
			break;
		case GPIO_MODE_OC_PULSE:
			pn_dir |= pins;
			pn_pu &= ~pins;
			break;
		case GPIO_MODE_OC_PU_PULSE:
			pn_dir |= pins;
			pn_pu |= pins;
			break;
		default:
			return;
	}
	
	switch(port)
	{
		case GPIO_PORT_0:
			P0_DIR = pn_dir;
			P0_PU = pn_pu;
			break;
		case GPIO_PORT_1:
			P1_DIR = pn_dir;
			P1_PU = pn_pu;
			break;
		case GPIO_PORT_2:
			P2_DIR = pn_dir;
			P2_PU = pn_pu;
			break;
		case GPIO_PORT_3:
			P3_DIR = pn_dir;
			P3_PU = pn_pu;
			break;
		case GPIO_PORT_4:
			P4_DIR = pn_dir;
			P4_PU = pn_pu;
			break;
		default:
			return;
	}
}

#ifdef USE_PORT_FUNCS
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
			P4_OUT = write_data;
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
			return P4_IN;
		default:
			return;
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
			P4_OUT = write_data ? (P4_OUT | pin) : (P4_OUT & ~pin);
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
			return (P4_IN & pin) ? 0x01 : 0x00;
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
			P4_OUT = (P4_OUT | pin);
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
			P4_OUT = (P4_OUT & ~pin);
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
			P4_OUT = (P4_OUT ^= pin);
			break;
		default:
			return;
	}
}
