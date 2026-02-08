#ifndef _CH552_PWM_H_
#define _CH552_PWM_H_

#define PWM_1 1
#define PWM_2 2

#define PWM_ACTIVE_HIGH 0
#define PWM_ACTIVE_LOW 1

#define PWM_1_P15 0x00
#define PWM_1_P30 0x04
#define PWM_2_P34 0x00
#define PWM_2_P31 0x08

void pwm_init(UINT8 pwm, UINT8 pol, UINT8 clk_div, UINT8 pin_map);
void pwm_set_output_enable(UINT8 pwm, UINT8 oe);
void pwm_set_duty_cycle(UINT8 pwm, UINT8 duty);

#endif
