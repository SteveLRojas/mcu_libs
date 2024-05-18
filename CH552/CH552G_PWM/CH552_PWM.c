#include "CH552.H"
#include "System.h"
#include "CH552_PWM.h"

//The CH552 datsheet does not say how to set the clock divider... Nor does it say which values are valid. Here are my findings:
// Setting clk_div to 0 results in a PWM frequency of FREQ_SYS / 65536
// Setting clk_div to 1 results in a PWM frequency of FREQ_SYS / 256
// FREQ_PWM = FREQ_SYS / (256 * clk_div), if clk_div = 0 then FREQ_PWM = FREQ_SYS / (256 * 256)
void pwm_init(UINT8 pwm, UINT8 pol, UINT8 clk_div, UINT8 pin_map)
{
	if(pwm & 0x01)
	{
		PWM_CTRL &= ~(bPWM_IE_END | bPWM1_POLAR | bPWM_IF_END | bPWM1_OUT_EN | bPWM_CLR_ALL);
		if(pol)
			PWM_CTRL |= bPWM1_POLAR;
		PIN_FUNC &= ~bPWM1_PIN_X;
		PIN_FUNC |= pin_map;
		PWM_CTRL |= bPWM1_OUT_EN;
	}
	if(pwm & 0x02)
	{
		PWM_CTRL &= ~(bPWM_IE_END | bPWM2_POLAR | bPWM_IF_END | bPWM2_OUT_EN | bPWM_CLR_ALL);
		if(pol)
			PWM_CTRL |= bPWM2_POLAR;
		PIN_FUNC &= ~bPWM2_PIN_X;
		PIN_FUNC |= pin_map;
		PWM_CTRL |= bPWM2_OUT_EN;
	}
	PWM_CK_SE = clk_div;
}

//HINT: pwm_init enables the output
void pwm_set_output_enable(UINT8 pwm, UINT8 oe)
{
	if(pwm & 0x01)
	{
		if(oe)
			PWM_CTRL |= bPWM1_OUT_EN;
		else
			PWM_CTRL &= ~bPWM1_OUT_EN;
	}
	if(pwm & 0x02)
	{
		if(oe)
			PWM_CTRL |= bPWM2_OUT_EN;
		else
			PWM_CTRL &= ~bPWM2_OUT_EN;
	}
}

void pwm_set_duty_cycle(UINT8 pwm, UINT8 duty)
{
	if(pwm & 0x01)
	{
		PWM_DATA1 = duty;
	}
	if(pwm & 0x02)
	{
		PWM_DATA2 = duty;
	}
}
