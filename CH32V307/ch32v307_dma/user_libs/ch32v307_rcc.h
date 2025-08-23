/*
 * ch32v307_rcc.h
 *
 *  Created on: Jul 19, 2025
 *      Author: Steve
 */

#ifndef _CH32V307_RCC_H_
#define _CH32V307_RCC_H_

//HINT: Refer to these defines for setting the user options below
#define RCC_PREDIV1_DIV1	0x00000000
#define RCC_PREDIV1_DIV2	0x00000001
#define RCC_PREDIV1_DIV3	0x00000002
#define RCC_PREDIV1_DIV4	0x00000003
#define RCC_PREDIV1_DIV5	0x00000004
#define RCC_PREDIV1_DIV6	0x00000005
#define RCC_PREDIV1_DIV7	0x00000006
#define RCC_PREDIV1_DIV8	0x00000007
#define RCC_PREDIV1_DIV9	0x00000008
#define RCC_PREDIV1_DIV10	0x00000009
#define RCC_PREDIV1_DIV11	0x0000000A
#define RCC_PREDIV1_DIV12	0x0000000B
#define RCC_PREDIV1_DIV13	0x0000000C
#define RCC_PREDIV1_DIV14	0x0000000D
#define RCC_PREDIV1_DIV15	0x0000000E
#define RCC_PREDIV1_DIV16	0x0000000F

#define  RCC_PLLMULL18	((uint32_t)0x00000000)	/* PLL input clock*18 */
#define  RCC_PLLMULL3	((uint32_t)0x00040000)	/* PLL input clock*3 */
#define  RCC_PLLMULL4	((uint32_t)0x00080000)	/* PLL input clock*4 */
#define  RCC_PLLMULL5	((uint32_t)0x000C0000)	/* PLL input clock*5 */
#define  RCC_PLLMULL6	((uint32_t)0x00100000)	/* PLL input clock*6 */
#define  RCC_PLLMULL7	((uint32_t)0x00140000)	/* PLL input clock*7 */
#define  RCC_PLLMULL8	((uint32_t)0x00180000)	/* PLL input clock*8 */
#define  RCC_PLLMULL9	((uint32_t)0x001C0000)	/* PLL input clock*9 */
#define  RCC_PLLMULL10	((uint32_t)0x00200000)	/* PLL input clock10 */
#define  RCC_PLLMULL11	((uint32_t)0x00240000)	/* PLL input clock*11 */
#define  RCC_PLLMULL12	((uint32_t)0x00280000)	/* PLL input clock*12 */
#define  RCC_PLLMULL13	((uint32_t)0x002C0000)	/* PLL input clock*13 */
#define  RCC_PLLMULL14	((uint32_t)0x00300000)	/* PLL input clock*14 */
#define  RCC_PLLMULL6_5	((uint32_t)0x00340000)	/* PLL input clock*6.5 */
#define  RCC_PLLMULL15	((uint32_t)0x00380000)	/* PLL input clock*15 */
#define  RCC_PLLMULL16	((uint32_t)0x003C0000)	/* PLL input clock*16 */

#define RCC_USBPRE_DIV1	0x00000000
#define RCC_USBPRE_DIV2	0x00400000
#define RCC_USBPRE_DIV3	0x00800000

// HINT: User options for initial clock configuration
#define RCC_USE_HSE		1
#define RCC_HSE_FREQ	8000000
#define RCC_HSE_STARTUP_TIMEOUT	((uint16_t)0x1000) /* Time out for HSE start up */
#define RCC_PREDIV1_FREQ_DIV	RCC_PREDIV1_DIV1
#define RCC_PLL_FREQ_MUL	RCC_PLLMULL12
#define RCC_HCLK_FREQ_DIV	RCC_HPRE_DIV1
#define RCC_PCLK1_FREQ_DIV	RCC_PPRE1_DIV2
#define RCC_PCLK2_FREQ_DIV	RCC_PPRE2_DIV1
#define RCC_USBCLK_FREQ_DIV	RCC_USBPRE_DIV2
#define RCC_ADCCLK_FREQ_DIV	RCC_ADCPRE_DIV8

// Do not modify anything below this line
#define rcc_hse_on() (RCC->CTLR |= RCC_HSEON)
#define rcc_hse_off() (RCC->CTLR &= ~RCC_HSEON)
#define rcc_hsi_on() (RCC->CTLR |= RCC_HSION)
#define rcc_hsi_off() (RCC->CTLR &= ~RCC_HSION)
#define rcc_pll_on() (RCC->CTLR |= RCC_PLLON)
#define rcc_pll_off() (RCC->CTLR &= ~RCC_PLLON)
#define rcc_pll2_on() (RCC->CTLR |= RCC_PLL2ON)
#define rcc_pll2_off() (RCC->CTLR &= ~RCC_PLL2ON)
#define rcc_pll3_on() (RCC->CTLR |= RCC_PLL3ON)
#define rcc_pll3_off() (RCC->CTLR &= ~RCC_PLL3ON)
#define rcc_css_on() (RCC->CTLR |= RCC_CSSON)
#define rcc_css_off() (RCC->CTLR &= ~RCC_CSSON)
#define rcc_hsebyp_on() (RCC->CTLR |= RCC_HSEBYP)
#define rcc_hsebyp_off() (RCC->CTLR &= ~RCC_HSEBYP)

#define rcc_hsi_pre_div1() (EXTEN->EXTEN_CTR |= EXTEN_PLL_HSI_PRE)
#define rcc_hsi_pre_div2() (EXTEN->EXTEN_CTR &= ~EXTEN_PLL_HSI_PRE)
#define rcc_pll_src_hse() (RCC->CFGR0 |= RCC_PLLSRC)
#define rcc_pll_src_hsi() (RCC->CFGR0 &= ~RCC_PLLSRC)
#define rcc_prediv1_src_pll2() (RCC->CFGR2 |= RCC_PREDIV1SRC)
#define rcc_prediv1_src_hse() (RCC->CFGR2 &= ~RCC_PREDIV1SRC)

#define rcc_mco_config(config) (RCC->CFGR0 = (RCC->CFGR0 & ~RCC_CFGR0_MCO) | (config))
#define rcc_usbpre_config(config) (RCC->CFGR0 = (RCC->CFGR0 & ~RCC_USBPRE) | (config))
#define rcc_pllmul_config(config) (RCC->CFGR0 = (RCC->CFGR0 & ~RCC_PLLMULL) | (config))
#define rcc_adcpre_config(config) (RCC->CFGR0 = (RCC->CFGR0 & ~RCC_ADCPRE) | (config))
#define rcc_ppre2_config(config) (RCC->CFGR0 = (RCC->CFGR0 & ~RCC_PPRE2) | (config))
#define rcc_ppre1_config(config) (RCC->CFGR0 = (RCC->CFGR0 & ~RCC_PPRE1) | (config))
#define rcc_hpre_config(config) (RCC->CFGR0 = (RCC->CFGR0 & ~RCC_HPRE) | (config))
#define rcc_sw_config(config) (RCC->CFGR0 = (RCC->CFGR0 & ~RCC_SW) | (config))

#define rcc_prediv1_config(config) (RCC->CFGR2 = (RCC->CFGR2 & ~RCC_PREDIV1) | (config))
#define rcc_prediv2_config(config) (RCC->CFGR2 = (RCC->CFGR2 & ~RCC_PREDIV2) | (config))
#define rcc_pll2mul_config(config) (RCC->CFGR2 = (RCC->CFGR2 & ~RCC_PLL2MUL) | (config))
#define rcc_pll3mul_config(config) (RCC->CFGR2 = (RCC->CFGR2 & ~RCC_PLL3MUL) | (config))

#define rcc_ahb_clk_enable(clock) (RCC->AHBPCENR |= (clock))
#define rcc_ahb_clk_disable(clock) (RCC->AHBPCENR &= ~(clock))
#define rcc_apb2_clk_enable(clock) (RCC->APB2PCENR |= (clock))
#define rcc_apb2_clk_disable(clock) (RCC->APB2PCENR &= ~(clock))
#define rcc_apb1_clk_enable(clock) (RCC->APB1PCENR |= (clock))
#define rcc_apb1_clk_disable(clock) (RCC->APB1PCENR &= ~(clock))

void rcc_system_init(void);
uint8_t rcc_hse_init(uint32_t hse_options);
void rcc_init_pll(uint32_t pll_options);
void rcc_init_pll2(uint32_t pll2_mul);
void rcc_init_pll3(uint32_t pll3_mul);
uint32_t rcc_compute_sysclk_freq();
uint32_t rcc_compute_hclk_freq();
uint32_t rcc_compute_pclk1_freq();
uint32_t rcc_compute_pclk1_tim_freq();
uint32_t rcc_compute_pclk2_freq();
uint32_t rcc_compute_pclk2_tim_freq();
uint32_t rcc_compute_adcclk();

#endif /* _CH32V307_RCC_H_ */
