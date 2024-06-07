#ifndef _CH559_PWM_H_
#define _CH559_PWM_H_

#define USE_PWM_3 1
#define USE_PWM_FUNCTIONS 0

#define PWM_1 1
#define PWM_2 2
#define PWM_3 3

#define PWM_1_ACTIVE_HIGH 0x08
#define PWM_1_ACTIVE_LOW  0x28
#define PWM_1_DISABLED    0x00
#define PWM_2_ACTIVE_HIGH 0x04
#define PWM_2_ACTIVE_LOW  0x44
#define PWM_2_DISABLED    0x00

#define PWM_3_ACTIVE_HIGH 0x0C
#define PWM_3_ACTIVE_LOW  0x2C
#define PWM_3_DISABLED    0x00

#define PWM_1_2_P24_P25   0x00
#define PWM_1_2_P43_P45   0x80
#define PWM_3_P12         0x00
#define PWM_3_P42         0x40

#define pwm1_enable_output() (PWM_CTRL |= bPWM_OUT_EN)
#define pwm1_disable_output() (PWM_CTRL &= ~bPWM_OUT_EN)
#define pwm1_set_duty_cycle(duty) (PWM_DATA = (duty))

#define pwm2_enable_output() (PWM_CTRL |= bPWM2_OUT_EN)
#define pwm2_disable_output() (PWM_CTRL &= ~bPWM2_OUT_EN)
#define pwm2_set_duty_cycle(duty) (PWM_DATA2 = (duty))

void pwm1_2_init(UINT8 mode, UINT8 clk_div, UINT8 max_count, UINT8 pin_map);
#ifdef USE_PWM_3
#define pwm3_enable_output() (T3_CTRL |= bT3_OUT_EN)
#define pwm3_disable_output() (T3_CTRL &= ~bT3_OUT_EN)
#define pwm3_set_duty_cycle(duty) (T3_FIFO = (duty))
void pwm3_init(UINT8 mode, UINT16 clk_div, UINT16 max_count, UINT8 pin_map);
#endif
#ifdef USE_PWM_FUNCTIONS
void pwm_set_output_enable(UINT8 pwm, UINT8 oe);
void pwm_set_duty_cycle(UINT8 pwm, UINT16 duty);
#endif

#endif
