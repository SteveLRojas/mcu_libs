#include "CH552.H"
#include "CH552_TIMER.h"

UINT16 timer0_initial_count;
UINT16 timer1_initial_count;

//HINT: user can directly read and write the overflow counts
volatile UINT16 timer_overflow_counts[3];

void (*timer0_callback)(void) = NULL;
void (*timer1_callback)(void) = NULL;
void (*timer2_callback)(void) = NULL;

void timer0_isr(void) interrupt INT_NO_TMR0
{
	TL0 = (UINT8)timer0_initial_count;
	TH0 = timer0_initial_count >> 8;
	++timer_overflow_counts[0];
	if(timer0_callback)
		timer0_callback();
}

void timer1_isr(void) interrupt INT_NO_TMR1
{
	TL1 = (UINT8)timer1_initial_count;
	TH1 = timer1_initial_count >> 8;
	++timer_overflow_counts[1];
	if(timer1_callback)
		timer1_callback();
}

void timer2_isr(void) interrupt INT_NO_TMR2
{
	++timer_overflow_counts[2];
	if(timer2_callback)
		timer2_callback();
	TF2 = 0;
}

//HINT: timer_init stops the timer
void timer_init(UINT8 timer, void (*callback)(void))
{
	switch(timer)
	{
		case TIMER_0:
			TR0 = 0;
			TMOD &= 0xF0;
			TMOD |= bT0_M0;
			T2MOD |= bTMR_CLK | bT0_CLK;
			timer_overflow_counts[0] = 0;
			timer0_callback = callback;
			ET0 = 1;	//enable interrupts
			break;
		case TIMER_1:
			TR1 = 0;
			TMOD &= 0x0F;
			TMOD |= bT1_M0;
			T2MOD |= bTMR_CLK | bT1_CLK;
			timer_overflow_counts[1] = 0;
			timer1_callback = callback;
			ET1 = 1;	//enable interrupts
			break;
		case TIMER_2:
			T2CON = 0;
			T2MOD &= 0xF0;
			T2MOD |= bTMR_CLK | bT2_CLK;
			timer_overflow_counts[2] = 0;
			timer2_callback = callback;
			ET2 = 1;	//enable interrupts
			break;
		default:
			return;
	}
}

//HINT: if period is set to 0 the timer will count with a period of 65536
void timer_set_period(UINT8 timer, UINT16 period)
{
	UINT16 initial_count = ~period + 1;
	switch(timer)
	{
		case TIMER_0:
			timer0_initial_count = initial_count;
			TL0 = (UINT8)initial_count;
			TH0 = initial_count >> 8;
			timer_overflow_counts[0] = 0;
			break;
		case TIMER_1:
			timer1_initial_count = initial_count;
			TL1 = (UINT8)initial_count;
			TH1 = initial_count >> 8;
			timer_overflow_counts[1] = 0;
			break;
		case TIMER_2:
			RCAP2 = initial_count;
			T2COUNT = initial_count;
			timer_overflow_counts[2] = 0;
			break;
		default:
			return;
	}
}

void timer_set_interrupt_enable(UINT8 timer, UINT8 ie)
{
	switch(timer)
	{
		case TIMER_0:
			ET0 = ie ? 1 : 0;
			break;
		case TIMER_1:
			ET1 = ie ? 1 : 0;
			break;
		case TIMER_2:
			ET2 = ie ? 1 : 0;
			break;
		default:
			return;
	}
}

void timer_start(UINT8 timer)
{
	switch(timer)
	{
		case TIMER_0:
			TR0 = 1;
			break;
		case TIMER_1:
			TR1 = 1;
			break;
		case TIMER_2:
			TR2 = 1;
			break;
		default:
			return;
	}
}

void timer_stop(UINT8 timer)
{
	switch(timer)
	{
		case TIMER_0:
			TR0 = 0;
			break;
		case TIMER_1:
			TR1 = 0;
			break;
		case TIMER_2:
			TR2 = 0;
			break;
		default:
			return;
	}
}

UINT16 timer_get_ticks(UINT8 timer)
{
	UINT16 num_ticks;
	switch(timer)
	{
		case TIMER_0:
			num_ticks = ((UINT16)TH0) << 8;
			num_ticks |= TL0;
			num_ticks -= timer0_initial_count;
			break;
		case TIMER_1:
			num_ticks = ((UINT16)TH1) << 8;
			num_ticks |= TL1;
			num_ticks -= timer1_initial_count;
			break;
		case TIMER_2:
			num_ticks = T2COUNT;
			num_ticks -= RCAP2;
			break;
		default:
			return 0;
	}
	return num_ticks;
}

//HINT: stop the timer before using the delay functions
//This function has an overhead of about 2.5us @ 24MHz
void timer_short_delay(UINT8 timer, UINT16 num_ticks)
{
	UINT16 initial_count = ~num_ticks + 1;
	switch(timer)
	{
		case TIMER_0:
			TL0 = (UINT8)initial_count;
			TH0 = initial_count >> 8;
			TF0 = 0;
			TR0 = 1;
			while(!TF0) {}
			TR0 = 0;
			return;
		case TIMER_1:
			TL1 = (UINT8)initial_count;
			TH1 = initial_count >> 8;
			TF1 = 0;
			TR1 = 1;
			while (!TF1) {}
			TR1 = 0;
			return;
		case TIMER_2:
			T2COUNT = initial_count;
			TF2 = 0;
			TR2 = 1;
			while(!TF2) {}
			TR2 = 0;
			return;
		default:
			return;
	}
}

//HINT: use timer_set_period to specify the time between overflows
void timer_long_delay(UINT8 timer, UINT16 num_overflows)
{
	timer_overflow_counts[timer] = 0;
	timer_start(timer);
	while(timer_overflow_counts[timer] < num_overflows) {}
	timer_stop(timer);
}
