#include "CH552.H"
#include "CH552_PS2_HOST.h"

#if PS2_USE_PORT_0
volatile UINT8 ps2_p0_rx_enabled;
volatile UINT8 ps2_p0_tx_enabled;
UINT8 ps2_p0_bit_count;
volatile UINT8 ps2_p0_shift;
volatile UINT8 ps2_p0_rx_error;
volatile UINT8 ps2_p0_tx_error;
UINT8 ps2_p0_parity;

void (*ps2_p0_rx_callback)(void) = NULL;
void (*ps2_p0_tx_callback)(void) = NULL;
#endif

#if PS2_USE_PORT_1
volatile UINT8 ps2_p1_rx_enabled;
volatile UINT8 ps2_p1_tx_enabled;
UINT8 ps2_p1_bit_count;
volatile UINT8 ps2_p1_shift;
volatile UINT8 ps2_p1_rx_error;
volatile UINT8 ps2_p1_tx_error;
UINT8 ps2_p1_parity;

void (*ps2_p1_rx_callback)(void) = NULL;
void (*ps2_p1_tx_callback)(void) = NULL;
#endif

// Interrupt triggered on the falling edge of PS/2 clock
#if PS2_USE_PORT_0
void int0_isr(void) interrupt INT_NO_INT0
{
	if(ps2_p0_rx_enabled)
	{
		if(!ps2_p0_bit_count)	//start
		{
			if(PS2_P0_DATA_PIN)
			{
				return;
			}
			else
			{
				ps2_p0_rx_error = 0;
				ps2_p0_parity = 0;
			}
		}
		else if(ps2_p0_bit_count == 9)	//parity
		{
			if(PS2_P0_DATA_PIN)
				ps2_p0_parity = ~ps2_p0_parity;
			ps2_p0_rx_error |= ~ps2_p0_parity;
		}
		else if(ps2_p0_bit_count == 10)	//stop
		{
			ps2_p0_rx_error |= !PS2_P0_DATA_PIN;
			ps2_p0_bit_count = 0;
			if(ps2_p0_rx_callback)
				ps2_p0_rx_callback();
			return;
		}
		else	//data
		{
			ps2_p0_shift = ps2_p0_shift >> 1;
			if(PS2_P0_DATA_PIN)
			{
				ps2_p0_shift |= 0x80;
				ps2_p0_parity = ~ps2_p0_parity;
			}
		}
		ps2_p0_bit_count += 1;
	}
	else if(ps2_p0_tx_enabled)
	{
		if(ps2_p0_bit_count == 8)
		{
			PS2_P0_DATA_PIN = ps2_p0_parity ^ 0x01;
		}
		else if(ps2_p0_bit_count == 9)
		{
			PS2_P0_DATA_PIN = 1;
		}
		else if(ps2_p0_bit_count == 10)
		{
			ps2_p0_tx_error = PS2_P0_DATA_PIN;
			ps2_p0_tx_enabled = 0;
			ps2_p0_rx_enabled = 1;
			ps2_p0_bit_count = 0;
			if(ps2_p0_tx_callback)
				ps2_p0_tx_callback();
			return;
		}
		else
		{
			PS2_P0_DATA_PIN = ps2_p0_shift & 0x01;
			ps2_p0_parity ^= ps2_p0_shift & 0x01;
			ps2_p0_shift = ps2_p0_shift >> 1;
		}
		ps2_p0_bit_count += 1;
	}
}
#endif

// Interrupt triggered on the falling edge of PS/2 port 1 clock
#if PS2_USE_PORT_1
void int1_isr(void) interrupt INT_NO_INT1
{
	if(ps2_p1_rx_enabled)
	{
		if(!ps2_p1_bit_count)	//start
		{
			if(PS2_P1_DATA_PIN)
			{
				return;
			}
			else
			{
				ps2_p1_rx_error = 0;
				ps2_p1_parity = 0;
			}
		}
		else if(ps2_p1_bit_count == 9)	//parity
		{
			if(PS2_P1_DATA_PIN)
				ps2_p1_parity = ~ps2_p1_parity;
			ps2_p1_rx_error |= ~ps2_p1_parity;
		}
		else if(ps2_p1_bit_count == 10)	//stop
		{
			ps2_p1_rx_error |= !PS2_P1_DATA_PIN;
			ps2_p1_bit_count = 0;
			if(ps2_p1_rx_callback)
				ps2_p1_rx_callback();
			return;
		}
		else	//data
		{
			ps2_p1_shift = ps2_p1_shift >> 1;
			if(PS2_P1_DATA_PIN)
			{
				ps2_p1_shift |= 0x80;
				ps2_p1_parity = ~ps2_p1_parity;
			}
		}
		ps2_p1_bit_count += 1;
	}
	else if(ps2_p1_tx_enabled)
	{
		if(ps2_p1_bit_count == 8)
		{
			PS2_P1_DATA_PIN = ps2_p1_parity ^ 0x01;
		}
		else if(ps2_p1_bit_count == 9)
		{
			PS2_P1_DATA_PIN = 1;
		}
		else if(ps2_p1_bit_count == 10)
		{
			ps2_p1_tx_error = PS2_P1_DATA_PIN;
			ps2_p1_tx_enabled = 0;
			ps2_p1_rx_enabled = 1;
			ps2_p1_bit_count = 0;
			if(ps2_p1_tx_callback)
				ps2_p1_tx_callback();
			return;
		}
		else
		{
			PS2_P1_DATA_PIN = ps2_p1_shift & 0x01;
			ps2_p1_parity ^= ps2_p1_shift & 0x01;
			ps2_p1_shift = ps2_p1_shift >> 1;
		}
		ps2_p1_bit_count += 1;
	}
}
#endif

#if PS2_USE_PORT_0
void ps2_host_p0_init(void)
{
	ps2_p0_rx_error = 0;
	ps2_p0_tx_error = 0;
	ps2_p0_parity = 0;
	IT0 = 1;	//Interrupt type set to falling edge
	ps2_p0_set_line_state(PS2_S_RX_ENABLED);
}
#endif

#if PS2_USE_PORT_1
void ps2_host_p1_init(void)
{
	ps2_p1_rx_error = 0;
	ps2_p1_tx_error = 0;
	ps2_p1_parity = 0;
	IT1 = 1;	//Interrupt type set to falling edge
	ps2_p1_set_line_state(PS2_S_RX_ENABLED);	
}
#endif

#if PS2_USE_PORT_0
void ps2_p0_set_line_state(UINT8 line_state)
{
	switch(line_state)
	{
		case PS2_S_RX_ENABLED:
			INT0 = 1;
			PS2_P0_DATA_PIN = 1;
			ps2_p0_rx_enabled = 1;
			ps2_p0_tx_enabled = 0;
			ps2_p0_bit_count = 0;
			IE0 = 0;	//clear interrupt flag
			EX0 = 1;	//enable interrupt
			break;
		case PS2_S_COM_DISABLED:
			EX0 = 0;	//disable clock edge interrupt
			INT0 = 0;
			PS2_P0_DATA_PIN = 1;
			ps2_p0_rx_enabled = 0;
			ps2_p0_tx_enabled = 0;
			break;
		case PS2_S_TX_REQUEST:
			INT0 = 1;
			PS2_P0_DATA_PIN = 0;
			ps2_p0_tx_enabled = 1;
			ps2_p0_bit_count = 0;
			ps2_p0_parity = 0;
			IE0 = 0;	//clear clock interrupt flag
			EX0 = 1;	//enable clock edge interrupt
			break;
		default:
			EX0 = 0;	//disable clock edge interrupt
			INT0 = 1;
			PS2_P0_DATA_PIN = 1;
	}
}
#endif

#if PS2_USE_PORT_1
void ps2_p1_set_line_state(UINT8 line_state)
{
	switch(line_state)
	{
		case PS2_S_RX_ENABLED:
			INT1 = 1;
			PS2_P1_DATA_PIN = 1;
			ps2_p1_rx_enabled = 1;
			ps2_p1_tx_enabled = 0;
			ps2_p1_bit_count = 0;
			IE1 = 0;	//clear interrupt flag
			EX1 = 1;	//enable interrupt
			break;
		case PS2_S_COM_DISABLED:
			EX1 = 0;	//disable clock edge interrupt
			INT1 = 0;
			PS2_P1_DATA_PIN = 1;
			ps2_p1_rx_enabled = 0;
			ps2_p1_tx_enabled = 0;
			break;
		case PS2_S_TX_REQUEST:
			INT1 = 1;
			PS2_P1_DATA_PIN = 0;
			ps2_p1_tx_enabled = 1;
			ps2_p1_bit_count = 0;
			ps2_p1_parity = 0;
			IE1 = 0;	//clear clock interrupt flag
			EX1 = 1;	//enable clock edge interrupt
			break;
		default:
			EX1 = 0;	//disable clock edge interrupt
			INT1 = 1;
			PS2_P1_DATA_PIN = 1;
	}
}
#endif
