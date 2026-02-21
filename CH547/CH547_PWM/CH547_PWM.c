#include "CH547.H"
#include "CH547_PWM.h"

//The CH552 datsheet does not say how to set the clock divider... Nor does it say which values are valid. Here are my findings:
// Setting clk_div to 0 results in a PWM frequency of FREQ_SYS / 65536
// Setting clk_div to 1 results in a PWM frequency of FREQ_SYS / 256
// FREQ_PWM = FREQ_SYS / (256 * clk_div), if clk_div = 0 then FREQ_PWM = FREQ_SYS / (256 * 256)
void pwm_init(UINT8 pwm_chans, UINT8 mode, UINT8 clk_div)
{
	if(pwm_chans & 0x01)
	{
		PWM_CTRL &= ~(bPWM0_POLAR | bPWM_IF_END | bPWM0_OUT_EN | bPWM_CLR_ALL | bPWM_MOD_6BIT);
		PWM_CTRL |= bPWM0_POLAR & mode;
	}
	if(pwm_chans & 0x02)
	{
		PWM_CTRL &= ~(bPWM1_POLAR | bPWM_IF_END | bPWM1_OUT_EN | bPWM_CLR_ALL | bPWM_MOD_6BIT);
		PWM_CTRL |= bPWM1_POLAR & mode;
	}
	//Channels 2 and 3 do not have configurable polarity
	PWM_CK_SE = clk_div;
	PWM_CTRL = (PWM_CTRL & ~bPWM_MOD_6BIT) | (mode & bPWM_MOD_6BIT);
}

//HINT: pwm_init does not enable PWM outputs
void pwm_set_output_enable(UINT8 pwm_chans, UINT8 oe)
{
	if(pwm_chans & 0x01)
	{
		if(oe)
			PWM_CTRL |= bPWM0_OUT_EN;
		else
			PWM_CTRL &= ~bPWM0_OUT_EN;
	}
	if(pwm_chans & 0x02)
	{
		if(oe)
			PWM_CTRL |= bPWM1_OUT_EN;
		else
			PWM_CTRL &= ~bPWM1_OUT_EN;
	}
	if(pwm_chans & 0x04)
	{
		if(oe)
			PWM_CTRL2 |= bPWM2_OUT_EN;
		else
			PWM_CTRL2 &= ~bPWM2_OUT_EN;
	}
	if(pwm_chans & 0x08)
	{
		if(oe)
			PWM_CTRL2 |= bPWM3_OUT_EN;
		else
			PWM_CTRL2 &= ~bPWM3_OUT_EN;
	}
}

void pwm_set_duty_cycle(UINT8 pwm_chans, UINT8 duty)
{
	if(pwm_chans & 0x01)
	{
		PWM_DATA0 = duty;
	}
	if(pwm_chans & 0x02)
	{
		PWM_DATA1 = duty;
	}
	if(pwm_chans & 0x04)
	{
		PWM_DATA2 = duty;
	}
	if(pwm_chans & 0x08)
	{
		PWM_DATA3 = duty;
	}
}
