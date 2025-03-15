#include "CH552.H"
#include "CH552_RCC.h"
#include "CH552_TIMER.h"
#include "CH552_PS2_HOST.h"
#include "CH552_PS2H_KB.h"

#define PS2H_KB_S_RX_WDATA	0
#define PS2H_KB_S_RX_WRESP	1

#define PS2H_KB_S_TIM_IDLE	0
#define PS2H_KB_S_TIM_SEND1	1
#define PS2H_KB_S_TIM_WTX1	2
#define PS2H_KB_S_TIM_SEND2	3
#define PS2H_KB_S_TIM_WTX2	4
#define PS2H_KB_S_TIM_WRX1	5
#define PS2H_KB_S_TIM_WRX2	6
#define PS2H_KB_S_TIM_WBAT	7
#define PS2H_KB_S_TIM_RETR	8
#define PS2H_KB_S_TIM_WTXR	9

#define PS2H_KB_S_SCAN_IDLE		0
#define PS2H_KB_S_SCAN_E0		1
#define PS2H_KB_S_SCAN_BREAK2	2
#define PS2H_KB_S_SCAN_BREAK3 	3
#define PS2H_KB_S_SCAN_PAUSE2	4
#define PS2H_KB_S_SCAN_PAUSE3	5
#define PS2H_KB_S_SCAN_PAUSE4	6
#define PS2H_KB_S_SCAN_PAUSE5	7
#define PS2H_KB_S_SCAN_PAUSE6	8
#define PS2H_KB_S_SCAN_PAUSE7	9
#define PS2H_KB_S_SCAN_PAUSE8	10

void ps2h_kb_press_key(UINT16 key);
void ps2h_kb_release_key(UINT16 key);
void ps2h_kb_start_tx(void);

volatile UINT16 ps2h_kb_pressed_keys[PS2H_KB_MAX_NUM_PRESSED];
volatile UINT8 ps2h_kb_status;
UINT8 ps2h_kb_retry_count;
UINT8 ps2h_kb_timer;
UINT8 ps2h_kb_rx_state;
UINT8 ps2h_kb_timer_state;
UINT8 ps2h_kb_rec_state;
UINT8 ps2h_kb_scan_state;
UINT8 ps2h_kb_tx_buf[2];

UINT8 ps2h_kb_rx_buf[PS2H_KB_RX_BUF_SIZE];
UINT8 ps2h_kb_rx_count;
UINT8 ps2h_kb_rx_front;
UINT8 ps2h_kb_rx_back;

//alternative: set flag, change timer time scale, and trigger timer to do further processing on a lower priority interrupt?
void ps2h_kb_on_rx(void)
{
	if(ps2_p0_rx_error)
	{
		if(ps2h_kb_retry_count > PS2H_KB_RETRY_LIMIT)
		{
			ps2h_kb_status |= PS2H_KB_STAT_DATA_ERROR;
		}
		else
		{
			ps2h_kb_timer_state = PS2H_KB_S_TIM_RETR;
			ps2h_kb_start_tx();
			ps2h_kb_rec_state = ps2h_kb_timer_state;	//save state
		}
		return;
	}
	
	if(ps2h_kb_rx_state == PS2H_KB_S_RX_WDATA)
	{
		ps2h_kb_rx_buf[ps2h_kb_rx_back] = ps2_p0_shift;
		ps2h_kb_rx_back += 1;
		ps2h_kb_rx_back &= (PS2H_KB_RX_BUF_SIZE - 1);
		ps2h_kb_rx_count += 1;
	}
	else	//if(ps2h_kb_rx_state == PS2H_KB_S_RX_WRESP)
	{
		if(ps2_p0_shift != 0xFA)
			ps2h_kb_status |= PS2H_KB_STAT_PROT_ERROR;
		
		if(ps2h_kb_timer_state == PS2H_KB_S_TIM_WRX2)
		{
			ps2h_kb_timer_state = PS2H_KB_S_TIM_SEND1;
			ps2h_kb_start_tx();
		}
		else	//if(ps2h_kb_timer_state == PS2H_KB_S_TIM_WRX1)
		{
			ps2h_kb_timer_state = PS2H_KB_S_TIM_IDLE;
			ps2h_kb_rx_state = PS2H_KB_S_RX_WDATA;
		}
	}
	ps2h_kb_retry_count = 0;
}

void ps2h_kb_on_tx(void)
{
	timer_stop(ps2h_kb_timer);
	
	if(ps2_p0_tx_error)
	{
		ps2h_kb_timer_state = PS2H_KB_S_TIM_IDLE;
		ps2h_kb_status |= PS2H_KB_STAT_DATA_ERROR;
		return;
	}
	
	switch(ps2h_kb_timer_state)
	{
		case PS2H_KB_S_TIM_WTX1:
			ps2h_kb_timer_state = PS2H_KB_S_TIM_WRX1;
			ps2h_kb_rx_state = PS2H_KB_S_RX_WRESP;
			break;
		case PS2H_KB_S_TIM_WTX2:
			ps2h_kb_timer_state = PS2H_KB_S_TIM_WRX2;
			ps2h_kb_rx_state = PS2H_KB_S_RX_WRESP;
			break;
		case PS2H_KB_S_TIM_WTXR:
			ps2h_kb_timer_state = ps2h_kb_rec_state;	//restore state
			break;
		default: ;
	}
}

void ps2h_kb_on_timer(void)
{
	timer_stop(ps2h_kb_timer);
	switch(ps2h_kb_timer_state)
	{
		case PS2H_KB_S_TIM_SEND1:
			ps2_p0_shift = ps2h_kb_tx_buf[0];
			ps2_p0_set_line_state(PS2_S_TX_REQUEST);
			timer_set_period(ps2h_kb_timer, FREQ_SYS / 1000ul);	//period is 1ms, also clears overflow count
			timer_start(ps2h_kb_timer);
			ps2h_kb_timer_state = PS2H_KB_S_TIM_WTX1;
			break;
		case PS2H_KB_S_TIM_WTX1:
			if(timer_overflow_counts[ps2h_kb_timer] >= 17)
			{
				ps2h_kb_status |= PS2H_KB_STAT_TIM_ERROR;
				ps2h_kb_timer_state = PS2H_KB_S_TIM_IDLE;
			}
			else
			{
				timer_start(ps2h_kb_timer);
			}
			break;
		case PS2H_KB_S_TIM_SEND2:
			ps2_p0_shift = ps2h_kb_tx_buf[0];
			ps2h_kb_tx_buf[0] = ps2h_kb_tx_buf[1];
			ps2_p0_set_line_state(PS2_S_TX_REQUEST);
			timer_set_period(ps2h_kb_timer, FREQ_SYS / 1000ul);	//period is 1ms, also clears overflow count
			timer_start(ps2h_kb_timer);
			ps2h_kb_timer_state = PS2H_KB_S_TIM_WTX2;
			break;
		case PS2H_KB_S_TIM_WTX2:
			if(timer_overflow_counts[ps2h_kb_timer] >= 17)
			{
				ps2h_kb_status |= PS2H_KB_STAT_TIM_ERROR;
				ps2h_kb_timer_state = PS2H_KB_S_TIM_IDLE;
			}
			else
			{
				timer_start(ps2h_kb_timer);
			}
			break;
		case PS2H_KB_S_TIM_WRX1:
			if(timer_overflow_counts[ps2h_kb_timer] >= 20)
			{
				ps2h_kb_status |= PS2H_KB_STAT_TIM_ERROR;
				ps2h_kb_timer_state = PS2H_KB_S_TIM_IDLE;
			}
			else
			{
				timer_start(ps2h_kb_timer);
			}
			break;
		case PS2H_KB_S_TIM_WRX2:
			if(timer_overflow_counts[ps2h_kb_timer] >= 20)
			{
				ps2h_kb_status |= PS2H_KB_STAT_TIM_ERROR;
				ps2h_kb_timer_state = PS2H_KB_S_TIM_IDLE;
			}
			else
			{
				timer_start(ps2h_kb_timer);
			}
			break;
		case PS2H_KB_S_TIM_WBAT:
			if((ps2h_kb_status & PS2H_KB_STAT_BAT_PASSED) || (ps2h_kb_rx_count && (ps2h_kb_rx_buf[ps2h_kb_rx_front] == 0xAA)))
				return;
			if(timer_overflow_counts[ps2h_kb_timer] >= 750)
			{
				ps2h_kb_timer_state = PS2H_KB_S_TIM_SEND1;
				ps2h_kb_tx_buf[0] = 0xFF;
				ps2h_kb_start_tx();
			}
			else
			{
				timer_start(ps2h_kb_timer);
			}
			break;
		case PS2H_KB_S_TIM_RETR:
			ps2h_kb_retry_count += 1;
			ps2_p0_shift = 0xFE;
			ps2_p0_set_line_state(PS2_S_TX_REQUEST);
			timer_set_period(ps2h_kb_timer, FREQ_SYS / 1000ul);	//period is 1ms, also clears overflow count
			timer_start(ps2h_kb_timer);
			ps2h_kb_timer_state = PS2H_KB_S_TIM_WTXR;
			break;
		case PS2H_KB_S_TIM_WTXR:
			if(timer_overflow_counts[ps2h_kb_timer] >= 20)
			{
				ps2h_kb_status |= PS2H_KB_STAT_TIM_ERROR;
				ps2h_kb_timer_state = PS2H_KB_S_TIM_IDLE;
			}
			else
			{
				timer_start(ps2h_kb_timer);
			}
			break;
		default: ;
	}
}

void ps2h_kb_init(UINT8 timer)
{
	UINT8 idx;
	
	ps2h_kb_timer = timer;
	timer_init(ps2h_kb_timer, ps2h_kb_on_timer);
	ps2_p0_rx_callback = ps2h_kb_on_rx;
	ps2_p0_tx_callback = ps2h_kb_on_tx;
	ps2_host_p0_init();
	//need to wait for BAT, send reset command only if no BAT
	ps2h_kb_status = PS2H_KB_STAT_NO_ERROR;
	ps2h_kb_retry_count = 0;
	ps2h_kb_rx_state = PS2H_KB_S_RX_WDATA;
	ps2h_kb_timer_state = PS2H_KB_S_TIM_WBAT;
	ps2h_kb_scan_state = PS2H_KB_S_SCAN_IDLE;
	ps2_p0_set_line_state(PS2_S_RX_ENABLED);
	timer_set_period(ps2h_kb_timer, FREQ_SYS / 1000ul);	//period is 1ms, also clears overflow count
	timer_start(ps2h_kb_timer);
	
	ps2h_kb_rx_count = 0;
	ps2h_kb_rx_front = 0;
	ps2h_kb_rx_back = 0;
	
	for(idx = 0; idx < PS2H_KB_MAX_NUM_PRESSED; ++idx)
	{
		ps2h_kb_pressed_keys[idx] = 0;
	}
}

void ps2h_kb_update_keys(void)
{
	UINT8 temp;
	
	while(ps2h_kb_rx_count)
	{
		temp = ps2h_kb_rx_buf[ps2h_kb_rx_front];
		ps2h_kb_rx_front += 1;
		ps2h_kb_rx_front &= (PS2H_KB_RX_BUF_SIZE - 1);
		ps2h_kb_rx_count -= 1;	//This needs to be atomic since interrupts are not disabled... check compiled code, should be a single DEC instruction
		
		if(temp == 0xAA)	//ignore self test passed
		{
			ps2h_kb_status |= PS2H_KB_STAT_BAT_PASSED;
			return;
		}
		
		switch(ps2h_kb_scan_state)
		{
			case PS2H_KB_S_SCAN_IDLE:
				if(temp == 0xF0)
					ps2h_kb_scan_state = PS2H_KB_S_SCAN_BREAK2;
				else if(temp == 0xE0)
					ps2h_kb_scan_state = PS2H_KB_S_SCAN_E0;
				else if(temp == 0xE1)
					ps2h_kb_scan_state = PS2H_KB_S_SCAN_PAUSE2;
				else
					ps2h_kb_press_key(temp);
				break;
			case PS2H_KB_S_SCAN_E0:
				if(temp == 0xF0)
					ps2h_kb_scan_state = PS2H_KB_S_SCAN_BREAK3;
				else
				{
					ps2h_kb_press_key(0xE000 | temp);
					ps2h_kb_scan_state = PS2H_KB_S_SCAN_IDLE;
				}
				break;
			case PS2H_KB_S_SCAN_BREAK2:
				ps2h_kb_release_key(temp);
				ps2h_kb_scan_state = PS2H_KB_S_SCAN_IDLE;
				break;
			case PS2H_KB_S_SCAN_BREAK3:
				ps2h_kb_release_key(0xE000 | temp);
				ps2h_kb_scan_state = PS2H_KB_S_SCAN_IDLE;
				break;
			case PS2H_KB_S_SCAN_PAUSE2:
				if(temp == 0x14)
					ps2h_kb_scan_state = PS2H_KB_S_SCAN_PAUSE3;
				else
				{
					ps2h_kb_status |= PS2H_KB_STAT_PROT_ERROR;
					ps2h_kb_scan_state = PS2H_KB_S_SCAN_IDLE;
				}
				break;
			case PS2H_KB_S_SCAN_PAUSE3:
				if(temp == 0x77)
					ps2h_kb_scan_state = PS2H_KB_S_SCAN_PAUSE4;
				else
				{
					ps2h_kb_status |= PS2H_KB_STAT_PROT_ERROR;
					ps2h_kb_scan_state = PS2H_KB_S_SCAN_IDLE;
				}
				break;
			case PS2H_KB_S_SCAN_PAUSE4:
				if(temp == 0xE1)
					ps2h_kb_scan_state = PS2H_KB_S_SCAN_PAUSE5;
				else
				{
					ps2h_kb_status |= PS2H_KB_STAT_PROT_ERROR;
					ps2h_kb_scan_state = PS2H_KB_S_SCAN_IDLE;
				}
				break;
			case PS2H_KB_S_SCAN_PAUSE5:
				if(temp == 0xF0)
					ps2h_kb_scan_state = PS2H_KB_S_SCAN_PAUSE6;
				else
				{
					ps2h_kb_status |= PS2H_KB_STAT_PROT_ERROR;
					ps2h_kb_scan_state = PS2H_KB_S_SCAN_IDLE;
				}
				break;
			case PS2H_KB_S_SCAN_PAUSE6:
				if(temp == 0x14)
					ps2h_kb_scan_state = PS2H_KB_S_SCAN_PAUSE7;
				else
				{
					ps2h_kb_status |= PS2H_KB_STAT_PROT_ERROR;
					ps2h_kb_scan_state = PS2H_KB_S_SCAN_IDLE;
				}
				break;
			case PS2H_KB_S_SCAN_PAUSE7:
				if(temp == 0xF0)
					ps2h_kb_scan_state = PS2H_KB_S_SCAN_PAUSE8;
				else
				{
					ps2h_kb_status |= PS2H_KB_STAT_PROT_ERROR;
					ps2h_kb_scan_state = PS2H_KB_S_SCAN_IDLE;
				}
				break;
			case PS2H_KB_S_SCAN_PAUSE8:
				if(temp == 0x77)
					ps2h_kb_status |= PS2H_KB_STAT_PAUSE_KEY;
				else
					ps2h_kb_status |= PS2H_KB_STAT_PROT_ERROR;
				ps2h_kb_scan_state = PS2H_KB_S_SCAN_IDLE;
				break;
			default: ps2h_kb_scan_state = PS2H_KB_S_SCAN_IDLE;
		}
	}
}

void ps2h_kb_set_led(UINT8 led_status)
{
	ps2h_kb_timer_state = PS2H_KB_S_TIM_SEND2;
	ps2h_kb_tx_buf[0] = 0xED;
	ps2h_kb_tx_buf[1] = led_status;
	ps2h_kb_start_tx();
}

void ps2h_kb_reset(void)
{
	UINT8 idx;
	
	ps2h_kb_status = PS2H_KB_STAT_NO_ERROR;
	ps2h_kb_retry_count = 0;
	
	ps2h_kb_rx_state = PS2H_KB_S_RX_WDATA;
	ps2h_kb_timer_state = PS2H_KB_S_TIM_SEND1;
	ps2h_kb_scan_state = PS2H_KB_S_SCAN_IDLE;
	ps2h_kb_tx_buf[0] = 0xFF;
	ps2h_kb_start_tx();
	
	ps2h_kb_rx_count = 0;
	ps2h_kb_rx_front = 0;
	ps2h_kb_rx_back = 0;
	
	for(idx = 0; idx < PS2H_KB_MAX_NUM_PRESSED; ++idx)
	{
		ps2h_kb_pressed_keys[idx] = 0;
	}
}

void ps2h_kb_press_key(UINT16 key)
{
	UINT8 idx;
	
	for(idx = 0; idx < PS2H_KB_MAX_NUM_PRESSED; ++idx)
	{
		if(ps2h_kb_pressed_keys[idx] == key)
			return;	//key is already pressed
	}
	
	for(idx = 0; idx < PS2H_KB_MAX_NUM_PRESSED; ++idx)
	{
		if(!ps2h_kb_pressed_keys[idx])
		{
			ps2h_kb_pressed_keys[idx] = key;
			return;
		}
	}
	
	ps2h_kb_status |= PS2H_KB_STAT_ROLLOVER;
	for(idx = 0; idx < PS2H_KB_MAX_NUM_PRESSED; ++idx)
	{
		ps2h_kb_pressed_keys[idx] = 0;
	}
}

void ps2h_kb_release_key(UINT16 key)
{
	UINT8 idx;
	
	for(idx = 0; idx < PS2H_KB_MAX_NUM_PRESSED; ++idx)
	{
		if(ps2h_kb_pressed_keys[idx] == key)
		{
			ps2h_kb_pressed_keys[idx] = 0;
			return;
		}
	}
}

void ps2h_kb_start_tx(void)
{
	ps2_p0_set_line_state(PS2_S_COM_DISABLED);
	timer_set_period(ps2h_kb_timer, FREQ_SYS / 10000ul);	//period is 100us
	timer_start(ps2h_kb_timer);
}
