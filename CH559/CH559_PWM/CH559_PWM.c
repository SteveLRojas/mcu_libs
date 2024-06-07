#define NO_XSFR_DEFINE
#include "CH559.H"
#include "CH559_PWM.h"

//The CH559 datsheet does not say how to set the clock divider... Nor does it say which values are valid. Here are my findings:
// Setting clk_div to 0 results in a PWM frequency of FREQ_SYS / (256 * max_count)
// Setting clk_div to 1 results in a PWM frequency of FREQ_SYS / max_count
// FREQ_PWM = FREQ_SYS / (clk_div * max_count). if clk_div = 0 then FREQ_PWM = FREQ_SYS / (256 * max_count). if max_count = 0 then FREQ_PWM = FREQ_SYS / (clk_div * 256)
// The counter counts up to max_count - 1, if max_count is 0 the counter counts up to 255.
void pwm1_2_init(UINT8 mode, UINT8 clk_div, UINT8 max_count, UINT8 pin_map)
{
	PWM_CTRL = mode;
	PWM_CK_SE = clk_div;
	PWM_CYCLE = max_count;
	PIN_FUNC &= ~bPWM1_PIN_X;
	PIN_FUNC |= pin_map;
}

#ifdef USE_PWM_3
void pwm3_init(UINT8 mode, UINT16 clk_div, UINT16 max_count, UINT8 pin_map)
{
	T3_SETUP = bT3_EN_CK_SE;
	T3_CK_SE = clk_div;
	T3_SETUP &= ~bT3_EN_CK_SE;
	T3_END = max_count;
	T3_CTRL = mode;
	PIN_FUNC &= ~bTMR3_PIN_X;
	PIN_FUNC |= pin_map;
}
#endif

#ifdef USE_PWM_FUNCTIONS
void pwm_set_output_enable(UINT8 pwm, UINT8 oe)
{
	switch(pwm)
	{
		case PWM_1:
			if(oe)
				PWM_CTRL |= bPWM_OUT_EN;
			else
				PWM_CTRL &= ~bPWM_OUT_EN;
			break;
		case PWM_2:
			if(oe)
				PWM_CTRL |= bPWM2_OUT_EN;
			else
				PWM_CTRL &= ~bPWM2_OUT_EN;
			break;
		case PWM_3:
			if(oe)
				T3_CTRL |= bT3_OUT_EN;
			else
				T3_CTRL &= ~bT3_OUT_EN;
			break;
		default:
			return;
	}
}

void pwm_set_duty_cycle(UINT8 pwm, UINT16 duty)
{
	switch(pwm)
	{
		case PWM_1:
			PWM_DATA = (UINT8)duty;
			break;
		case PWM_2:
			PWM_DATA2 = (UINT8)duty;
			break;
		case PWM_3:
			T3_FIFO = duty;
			break;
		default:
			return;
	}
}
#endif
