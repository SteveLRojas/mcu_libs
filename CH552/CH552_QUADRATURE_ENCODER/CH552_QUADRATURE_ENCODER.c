#include "CH552.H"
#include "CH552_QUADRATURE_ENCODER.h"

volatile UINT16 qenc_count;
volatile UINT16 qenc_raw_count;
UINT8 qenc_prev_event;	//Used to prevent false counting caused by bad contacts

//CW Events:
//	0: Falling A, high B
//	1: Falling B, low A
//	2: Rising A, low B
//	3: Rising B, high A
//CCW Events:
//	4: Falling A, low B
//	5: Rising B, low A
//	6: Rising A, high B
//	7: Falling B, high A

// Interrupt triggered on the falling edge of A
void int0_isr(void) interrupt INT_NO_INT0
{
	if(INT1)
	{
		if(qenc_prev_event != 0)
			++qenc_count;
		++qenc_raw_count;
		qenc_prev_event = 0;
	}
	else
	{
		if(qenc_prev_event != 4)
			--qenc_count;
		--qenc_raw_count;
		qenc_prev_event = 4;
	}
}

// Interrupt triggered on the falling edge of B
void int1_isr(void) interrupt INT_NO_INT1
{
	if(INT0)
	{
		if(qenc_prev_event != 7)
			--qenc_count;
		--qenc_raw_count;
		qenc_prev_event = 7;
	}
	else
	{
		if(qenc_prev_event != 1)
			++qenc_count;
		++qenc_raw_count;
		qenc_prev_event = 1;
	}
}

void qenc_init()
{
	qenc_count = 0;
	qenc_raw_count = 0;
	IT0 = 1;
	IT1 = 1;
	//Enable interrupts
	EX0 = 1;
	EX1 = 1;
}
