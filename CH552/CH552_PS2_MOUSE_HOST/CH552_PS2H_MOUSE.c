#include "CH552.H"
#include "CH552_RCC.h"
#include "CH552_TIMER.h"
#include "CH552_PS2_HOST.h"
#include "CH552_PS2H_MOUSE.h"

#define PS2H_MS_S_RX_WDATA	0
#define PS2H_MS_S_RX_WRESP	1

#define PS2H_MS_S_TIM_WBAT	0
#define PS2H_MS_S_TIM_WID	1
#define PS2H_MS_S_TIM_SEND2	2
#define PS2H_MS_S_TIM_WTX2	3
#define PS2H_MS_S_TIM_WRX2	4
#define PS2H_MS_S_TIM_SEND1	5
#define PS2H_MS_S_TIM_WTX1	6
#define PS2H_MS_S_TIM_WRX1	7
#define PS2H_MS_S_TIM_RETR	8
#define PS2H_MS_S_TIM_WTXR	9
#define PS2H_MS_S_TIM_IDLE	10

#define PS2H_MS_S_PRS_WBAT	0
#define PS2H_MS_S_PRS_WID	1
#define PS2H_MS_S_PRS_BTNS	2
#define PS2H_MS_S_PRS_MOVX	3
#define PS2H_MS_S_PRS_MOVY	4
#define PS2H_MS_S_PRS_MOVZ	5

void ps2h_ms_start_tx(void);

volatile UINT8 ps2h_ms_status;
volatile UINT8 ps2h_ms_buttons;
volatile signed char ps2h_ms_x_rel;
volatile signed char ps2h_ms_y_rel;
volatile signed char ps2h_ms_z_rel;
UINT8 ps2h_ms_retry_count;
UINT8 ps2h_ms_timer;
UINT8 ps2h_ms_rx_state;
UINT8 ps2h_ms_timer_state;
UINT8 ps2h_ms_rec_state;
UINT8 ps2h_ms_parse_state;
UINT8 ps2h_ms_tx_buf[2];

UINT8 ps2h_ms_rx_buf[PS2H_MS_RX_BUF_SIZE];
UINT8 ps2h_ms_rx_count;
UINT8 ps2h_ms_rx_front;
UINT8 ps2h_ms_rx_back;

void ps2h_ms_on_rx(void)
{
#if PS2H_MS_USE_RX_BENCHMARK_PIN
	PS2H_MS_RX_BENCHMARK_PIN = 0;
#endif
	if(ps2_p0_rx_error)
	{
		if(ps2h_ms_retry_count > PS2H_MS_RETRY_LIMIT)
		{
			ps2h_ms_status |= PS2H_MS_STAT_DATA_ERROR;
		}
		else
		{
			ps2h_ms_rec_state = ps2h_ms_timer_state;	//save state
			ps2h_ms_timer_state = PS2H_MS_S_TIM_RETR;
			ps2h_ms_start_tx();
		}
#if PS2H_MS_USE_RX_BENCHMARK_PIN
		PS2H_MS_RX_BENCHMARK_PIN = 1;
#endif
		return;
	}
	
	if(ps2h_ms_rx_state == PS2H_MS_S_RX_WDATA)
	{
		ps2h_ms_rx_buf[ps2h_ms_rx_back] = ps2_p0_shift;
		ps2h_ms_rx_back += 1;
		ps2h_ms_rx_back &= (PS2H_MS_RX_BUF_SIZE - 1);
		ps2h_ms_rx_count += 1;
	}
	else	//if(ps2h_ms_rx_state == PS2H_KB_S_RX_WRESP)
	{
		if(ps2_p0_shift != 0xFA)
			ps2h_ms_status |= PS2H_MS_STAT_PROT_ERROR;
		
		if(ps2h_ms_timer_state == PS2H_MS_S_TIM_WRX2)
		{
			ps2h_ms_timer_state = PS2H_MS_S_TIM_SEND1;
			ps2h_ms_start_tx();
		}
		else	//if(ps2h_ms_timer_state == PS2H_MS_S_TIM_WRX1)
		{
			ps2h_ms_status |= PS2H_MS_STAT_READY;
			ps2h_ms_timer_state = PS2H_MS_S_TIM_IDLE;
			ps2h_ms_rx_state = PS2H_MS_S_RX_WDATA;
		}
	}
	ps2h_ms_retry_count = 0;
#if PS2H_MS_USE_RX_BENCHMARK_PIN
	PS2H_MS_RX_BENCHMARK_PIN = 1;
#endif
}

//HINT: Here we can assume that the timer is active and the period is 1ms
void ps2h_ms_on_tx(void)
{
	if(ps2_p0_tx_error)
	{
		timer_stop(ps2h_ms_timer);
		ps2h_ms_timer_state = PS2H_MS_S_TIM_IDLE;
		ps2h_ms_status |= PS2H_MS_STAT_DATA_ERROR | PS2H_MS_STAT_READY;
		return;
	}
	
	switch(ps2h_ms_timer_state)
	{
		case PS2H_MS_S_TIM_WTX2:
			timer_overflow_counts[ps2h_ms_timer] = 0;
			ps2h_ms_timer_state = PS2H_MS_S_TIM_WRX2;
			ps2h_ms_rx_state = PS2H_MS_S_RX_WRESP;
			break;
		case PS2H_MS_S_TIM_WTX1:
			timer_overflow_counts[ps2h_ms_timer] = 0;
			ps2h_ms_timer_state = PS2H_MS_S_TIM_WRX1;
			ps2h_ms_rx_state = PS2H_MS_S_RX_WRESP;
			break;
		case PS2H_MS_S_TIM_WTXR:
			timer_overflow_counts[ps2h_ms_timer] = 0;
			ps2h_ms_timer_state = ps2h_ms_rec_state;	//restore state
			break;
		default: ;
	}
}

void ps2h_ms_on_timer(void)
{
#if PS2H_MS_USE_TIM_BENCHMARK_PIN
	PS2H_MS_TIM_BENCHMARK_PIN = 0;
#endif
	timer_stop(ps2h_ms_timer);
	switch(ps2h_ms_timer_state)
	{
		case PS2H_MS_S_TIM_WBAT:
			if((ps2h_ms_status & PS2H_MS_STAT_BAT_PASSED) || (ps2h_ms_rx_count && (ps2h_ms_rx_buf[ps2h_ms_rx_front] == 0xAA)))
			{
				ps2h_ms_timer_state = PS2H_MS_S_TIM_WID;
				timer_overflow_counts[ps2h_ms_timer] = 0;
				timer_start(ps2h_ms_timer);
#if PS2H_MS_USE_TIM_BENCHMARK_PIN
				PS2H_MS_TIM_BENCHMARK_PIN = 1;
#endif
				return;
			}
			if(timer_overflow_counts[ps2h_ms_timer] >= 750)
			{
				ps2h_ms_timer_state = PS2H_MS_S_TIM_SEND1;	//prepare to send
				ps2h_ms_tx_buf[0] = PS2H_MS_COM_1B_RESET;
				ps2h_ms_start_tx();
			}
			else
			{
				timer_start(ps2h_ms_timer);
			}
			break;
		case PS2H_MS_S_TIM_WID:
			if((ps2h_ms_status & PS2H_MS_STAT_DEVICE_ID) || (ps2h_ms_rx_count && (ps2h_ms_rx_buf[ps2h_ms_rx_front] == 0x00)))
			{
				ps2h_ms_timer_state = PS2H_MS_S_TIM_IDLE;
				ps2h_ms_status |= PS2H_MS_STAT_READY;
#if PS2H_MS_USE_TIM_BENCHMARK_PIN
				PS2H_MS_TIM_BENCHMARK_PIN = 1;
#endif
				return;
			}
			if(timer_overflow_counts[ps2h_ms_timer] >= 750)
			{
				ps2h_ms_timer_state = PS2H_MS_S_TIM_IDLE;
				ps2h_ms_status |= PS2H_MS_STAT_TIM_ERROR | PS2H_MS_STAT_READY;
			}
			else
			{
				timer_start(ps2h_ms_timer);
			}
			break;
		case PS2H_MS_S_TIM_SEND2:
			ps2_p0_shift = ps2h_ms_tx_buf[1];
			ps2_p0_set_line_state(PS2_S_TX_REQUEST);
			timer_set_period(ps2h_ms_timer, FREQ_SYS / 1000ul);	//period is 1ms, also clears overflow count
			timer_start(ps2h_ms_timer);
			ps2h_ms_timer_state = PS2H_MS_S_TIM_WTX2;
			break;
		case PS2H_MS_S_TIM_WTX2:
			if(timer_overflow_counts[ps2h_ms_timer] >= 17)
			{
				ps2h_ms_status |= PS2H_MS_STAT_TIM_ERROR | PS2H_MS_STAT_READY;
				ps2h_ms_timer_state = PS2H_MS_S_TIM_IDLE;
			}
			else
			{
				timer_start(ps2h_ms_timer);
			}
			break;
		case PS2H_MS_S_TIM_WRX2:
			if(timer_overflow_counts[ps2h_ms_timer] >= 20)
			{
				ps2h_ms_status |= PS2H_MS_STAT_TIM_ERROR | PS2H_MS_STAT_READY;
				ps2h_ms_timer_state = PS2H_MS_S_TIM_IDLE;
			}
			else
			{
				timer_start(ps2h_ms_timer);
			}
			break;
		case PS2H_MS_S_TIM_SEND1:
			ps2_p0_shift = ps2h_ms_tx_buf[0];
			ps2_p0_set_line_state(PS2_S_TX_REQUEST);
			timer_set_period(ps2h_ms_timer, FREQ_SYS / 1000ul);	//period is 1ms, also clears overflow count
			timer_start(ps2h_ms_timer);
			ps2h_ms_timer_state = PS2H_MS_S_TIM_WTX1;
			break;
		case PS2H_MS_S_TIM_WTX1:
			if(timer_overflow_counts[ps2h_ms_timer] > 17)
			{
				ps2h_ms_status |= PS2H_MS_STAT_TIM_ERROR | PS2H_MS_STAT_READY;
				ps2h_ms_timer_state = PS2H_MS_S_TIM_IDLE;
			}
			else
			{
				timer_start(ps2h_ms_timer);
			}
			break;
		case PS2H_MS_S_TIM_WRX1:
			if(timer_overflow_counts[ps2h_ms_timer] >= 20)
			{
				ps2h_ms_status |= PS2H_MS_STAT_TIM_ERROR | PS2H_MS_STAT_READY;
				ps2h_ms_timer_state = PS2H_MS_S_TIM_IDLE;
			}
			else
			{
				timer_start(ps2h_ms_timer);
			}
			break;
		case PS2H_MS_S_TIM_RETR:
			ps2h_ms_retry_count += 1;
			ps2_p0_shift = PS2H_MS_COM_1B_RESEND;
			ps2_p0_set_line_state(PS2_S_TX_REQUEST);
			timer_set_period(ps2h_ms_timer, FREQ_SYS / 1000ul);	//period is 1ms, also clears overflow count
			timer_start(ps2h_ms_timer);
			ps2h_ms_timer_state = PS2H_MS_S_TIM_WTXR;
			break;
		case PS2H_MS_S_TIM_WTXR:
			if(timer_overflow_counts[ps2h_ms_timer] >= 20)
			{
				ps2h_ms_status |= PS2H_MS_STAT_TIM_ERROR | PS2H_MS_STAT_READY;
				ps2h_ms_timer_state = PS2H_MS_S_TIM_IDLE;
			}
			else
			{
				timer_start(ps2h_ms_timer);
			}
			break;
		default: ;
	}
#if PS2H_MS_USE_TIM_BENCHMARK_PIN
	PS2H_MS_TIM_BENCHMARK_PIN = 1;
#endif
}

void ps2h_ms_init(UINT8 timer)
{
	ps2h_ms_timer = timer;
	timer_init(ps2h_ms_timer, ps2h_ms_on_timer);
	ps2_p0_rx_callback = ps2h_ms_on_rx;
	ps2_p0_tx_callback = ps2h_ms_on_tx;
	ps2_host_p0_init();
	//need to wait for BAT, send reset command only if no BAT
	ps2h_ms_status = PS2H_MS_STAT_NO_ERROR;
	ps2h_ms_retry_count = 0;
	ps2h_ms_rx_state = PS2H_MS_S_RX_WDATA;	//Only "acknowledge" (0xFA) is processed as a response, all else as DATA
	ps2h_ms_timer_state = PS2H_MS_S_TIM_WBAT;
	ps2h_ms_parse_state = PS2H_MS_S_PRS_WBAT;
	ps2_p0_set_line_state(PS2_S_RX_ENABLED);
	timer_set_period(ps2h_ms_timer, FREQ_SYS / 1000ul);	//period is 1ms, also clears overflow count
	timer_start(ps2h_ms_timer);
	
	ps2h_ms_rx_count = 0;
	ps2h_ms_rx_front = 0;
	ps2h_ms_rx_back = 0;
	
	ps2h_ms_buttons = 0x00;
	ps2h_ms_x_rel = 0x00;
	ps2h_ms_y_rel = 0x00;
	ps2h_ms_z_rel = 0x00;
}

void ps2h_ms_start_tx(void)
{
	ps2_p0_set_line_state(PS2_S_COM_DISABLED);
	ps2h_ms_status &= ~PS2H_MS_STAT_READY;
	timer_set_period(ps2h_ms_timer, FREQ_SYS / 10000ul);	//period is 100us
	timer_start(ps2h_ms_timer);
}

void ps2h_ms_send_1b_command(UINT8 command)
{
	ps2h_ms_timer_state = PS2H_MS_S_TIM_SEND1;
	ps2h_ms_tx_buf[0] = command;
	ps2h_ms_start_tx();
}

void ps2h_ms_send_2b_command(UINT8 command, UINT8 val)
{
	ps2h_ms_timer_state = PS2H_MS_S_TIM_SEND2;
	ps2h_ms_tx_buf[1] = command;
	ps2h_ms_tx_buf[0] = val;
	ps2h_ms_start_tx();
}

//HINT: This is a blocking function that may take considerable time to complete.
//TODO: implement this in a non-blocking way.
void ps2h_ms_enable_scrolling(void)
{
	ps2h_ms_set_sample_rate(PS2H_MS_SAMPLE_RATE_200);
	while(!(ps2h_ms_status & PS2H_MS_STAT_READY));	//wait for the command to complete
	ps2h_ms_set_sample_rate(PS2H_MS_SAMPLE_RATE_100);
	while(!(ps2h_ms_status & PS2H_MS_STAT_READY));
	ps2h_ms_set_sample_rate(PS2H_MS_SAMPLE_RATE_80);
	while(!(ps2h_ms_status & PS2H_MS_STAT_READY));
	//now get the device ID
	ps2h_ms_status &= ~PS2H_MS_STAT_DEVICE_ID;
	ps2h_ms_timer_state = PS2H_MS_S_TIM_SEND1;
	ps2h_ms_parse_state = PS2H_MS_S_PRS_WID;
	ps2h_ms_tx_buf[0] = PS2H_MS_COM_1B_GET_DEVICE_ID;
	ps2h_ms_start_tx();
}

void ps2h_ms_reset(void)
{
	ps2h_ms_status = PS2H_MS_STAT_NO_ERROR;
	ps2h_ms_retry_count = 0;
	
	ps2h_ms_rx_state = PS2H_MS_S_RX_WDATA;
	ps2h_ms_timer_state = PS2H_MS_S_TIM_SEND1;
	ps2h_ms_parse_state = PS2H_MS_S_PRS_WBAT;
	ps2h_ms_tx_buf[0] = PS2H_MS_COM_1B_RESET;
	ps2h_ms_start_tx();
	
	ps2h_ms_rx_count = 0;
	ps2h_ms_rx_front = 0;
	ps2h_ms_rx_back = 0;
	
	ps2h_ms_buttons = 0x00;
	ps2h_ms_x_rel = 0x00;
	ps2h_ms_y_rel = 0x00;
	ps2h_ms_z_rel = 0x00;
}

void ps2h_ms_update(void)
{
	UINT8 temp;
	
	while(ps2h_ms_rx_count)
	{
		temp = ps2h_ms_rx_buf[ps2h_ms_rx_front];
		ps2h_ms_rx_front += 1;
		ps2h_ms_rx_front &= (PS2H_MS_RX_BUF_SIZE - 1);
		E_DIS = 1;
		ps2h_ms_rx_count -= 1;
		E_DIS = 0;
		
		switch(ps2h_ms_parse_state)
		{
			case PS2H_MS_S_PRS_WBAT:
				if(temp == 0xAA)
					ps2h_ms_status |= PS2H_MS_STAT_BAT_PASSED;
				else
					ps2h_ms_status |= PS2H_MS_STAT_PROT_ERROR;
				ps2h_ms_parse_state = PS2H_MS_S_PRS_WID;
				break;
			case PS2H_MS_S_PRS_WID:
				if(temp == 0x00)
					ps2h_ms_status |= PS2H_MS_STAT_DEVICE_ID;
				else if((temp == 0x03) || (temp == 0x04))
					ps2h_ms_status |= PS2H_MS_STAT_DEVICE_ID | PS2H_MS_STAT_SCROLL_EN;
				else
					ps2h_ms_status |= PS2H_MS_STAT_PROT_ERROR;
				ps2h_ms_parse_state = PS2H_MS_S_PRS_BTNS;
				break;
			case PS2H_MS_S_PRS_BTNS:
				ps2h_ms_buttons = temp;
				if(temp & PS2H_MS_BTNS_ALWAYS_1)	//bit 3 should always be 1
					ps2h_ms_parse_state = PS2H_MS_S_PRS_MOVX;
				else
					ps2h_ms_status |= PS2H_MS_STAT_PROT_ERROR;
				break;
			case PS2H_MS_S_PRS_MOVX:
				ps2h_ms_x_rel = temp;
				if(ps2h_ms_buttons & PS2H_MS_BTNS_SIGN_X)	//negative
				{
					if((~ps2h_ms_x_rel & 0x80) | (ps2h_ms_buttons & PS2H_MS_BTNS_OVERFLOW_X))
						ps2h_ms_x_rel = 0x80;	//clip to min val
				}
				else
				{
					if((ps2h_ms_x_rel & 0x80) | (ps2h_ms_buttons & PS2H_MS_BTNS_OVERFLOW_X))
						ps2h_ms_x_rel = 0x7F;	//clip to max val
				}
				ps2h_ms_parse_state = PS2H_MS_S_PRS_MOVY;
				break;
			case PS2H_MS_S_PRS_MOVY:
				ps2h_ms_y_rel = temp;
				if(ps2h_ms_buttons & PS2H_MS_BTNS_SIGN_Y)
				{
					if((~ps2h_ms_y_rel & 0x80) | (ps2h_ms_buttons & PS2H_MS_BTNS_OVERFLOW_Y))
						ps2h_ms_y_rel = 0x80;
				}
				else
				{
					if((ps2h_ms_y_rel & 0x80) | (ps2h_ms_buttons & PS2H_MS_BTNS_OVERFLOW_Y))
						ps2h_ms_y_rel = 0x7F;
				}
				if(ps2h_ms_status & PS2H_MS_STAT_SCROLL_EN)
					ps2h_ms_parse_state = PS2H_MS_S_PRS_MOVZ;
				else
					ps2h_ms_parse_state = PS2H_MS_S_PRS_BTNS;
				break;
			case PS2H_MS_S_PRS_MOVZ:
				ps2h_ms_z_rel = temp;
				ps2h_ms_parse_state = PS2H_MS_S_PRS_BTNS;
				break;
			default: ;
		}
	}
}
