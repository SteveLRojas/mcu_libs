#ifndef _CH547_PWM_H_
#define _CH547_PWM_H_

#define PWM_0 1
#define PWM_1 2
#define PWM_2 4
#define PWM_3 8

#define PWM_0_MODE_ACTIVE_HIGH	0x00
#define PWM_0_MODE_ACTIVE_LOW	0x20
#define PWM_1_MODE_ACTIVE_HIGH	0x00
#define PWM_1_MODE_ACTIVE_LOW	0x40
#define PWM_MODE_8_BIT	0x00
#define PWM_MODE_6_BIT	0x01

#define PWM_0_MAP_P15	0x80
#define PWM_0_MAP_P25	0x00

#define pwm_0_remap(map) (PIN_FUNC = (PIN_FUNC & ~bPWM0_PIN_X) | (map))
#define pwm_set_clk_div(clk_div) (PWM_CTRL = (clk_div))

void pwm_init(UINT8 pwm_chans, UINT8 mode, UINT8 clk_div);
void pwm_set_output_enable(UINT8 pwm_chans, UINT8 oe);
void pwm_set_duty_cycle(UINT8 pwm_chans, UINT8 duty);

#endif
