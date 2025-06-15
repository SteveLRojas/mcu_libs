/*
 * ch32v203_rcc.h
 *
 *  Created on: Jul 2, 2024
 *      Author: Steve
 */

#ifndef USER_LIBS_CH32V203_RCC_H_
#define USER_LIBS_CH32V203_RCC_H_

#define RCC_USBPRE_DIV1 0x00000000
#define RCC_USBPRE_DIV2 0x00400000
#define RCC_USBPRE_DIV3 0x00800000

// HINT: User options for initial clock configuration
#define RCC_USE_HSE 1
#define RCC_HSE_FREQ 8000000
#define RCC_PLL_FREQ_MUL RCC_PLLMULL12
#define RCC_HCLK_FREQ_DIV RCC_HPRE_DIV1
#define RCC_PCLK1_FREQ_DIV RCC_PPRE1_DIV2
#define RCC_PCLK2_FREQ_DIV RCC_PPRE2_DIV1
#define RCC_USBCLK_FREQ_DIV RCC_USBPRE_DIV2
#define RCC_ADCCLK_FREQ_DIV RCC_ADCPRE_DIV8

// Do not modify anything below this line
#define rcc_hse_on() (RCC->CTLR |= RCC_HSEON)
#define rcc_hse_off() (RCC->CTLR &= ~RCC_HSEON)
#define rcc_hsi_on() (RCC->CTLR |= RCC_HSION)
#define rcc_hsi_off() (RCC->CTLR &= ~RCC_HSION)
#define rcc_pll_on() (RCC->CTLR |= RCC_PLLON)
#define rcc_pll_off() (RCC->CTLR &= ~RCC_PLLON)
#define rcc_css_on() (RCC->CTLR |= RCC_CSSON)
#define rcc_css_off() (RCC->CTLR &= ~RCC_CSSON)
#define rcc_hsebyp_on() (RCC->CTLR |= RCC_HSEBYP)
#define rcc_hsebyp_off() (RCC->CTLR &= ~RCC_HSEBYP)

#define rcc_hse_pre_div1() (RCC->CFGR0 &= ~RCC_PLLXTPRE)
#define rcc_hse_pre_div2() (RCC->CFGR0 |= RCC_PLLXTPRE)
#define rcc_hsi_pre_div1() (EXTEN->EXTEN_CTR |= EXTEN_PLL_HSI_PRE)
#define rcc_hsi_pre_div2() (EXTEN->EXTEN_CTR &= ~EXTEN_PLL_HSI_PRE)
#define rcc_pll_src_hse() (RCC->CFGR0 |= RCC_PLLSRC)
#define rcc_pll_src_hsi() (RCC->CFGR0 &= ~RCC_PLLSRC)

#define rcc_mco_config(config) (RCC->CFGR0 = (RCC->CFGR0 & ~RCC_CFGR0_MCO) | (config))
#define rcc_usbpre_config(config) (RCC->CFGR0 = (RCC->CFGR0 & ~RCC_USBPRE) | (config))
#define rcc_pllmul_config(config) (RCC->CFGR0 = (RCC->CFGR0 & ~RCC_PLLMULL) | (config))
#define rcc_adcpre_config(config) (RCC->CFGR0 = (RCC->CFGR0 & ~RCC_ADCPRE) | (config))
#define rcc_ppre2_config(config) (RCC->CFGR0 = (RCC->CFGR0 & ~RCC_PPRE2) | (config))
#define rcc_ppre1_config(config) (RCC->CFGR0 = (RCC->CFGR0 & ~RCC_PPRE1) | (config))
#define rcc_hpre_config(config) (RCC->CFGR0 = (RCC->CFGR0 & ~RCC_HPRE) | (config))
#define rcc_sw_config(config) (RCC->CFGR0 = (RCC->CFGR0 & ~RCC_SW) | (config))

#define rcc_ahb_clk_enable(clock) (RCC->AHBPCENR |= (clock))
#define rcc_ahb_clk_disable(clock) (RCC->AHBPCENR &= ~(clock))
#define rcc_apb2_clk_enable(clock) (RCC->APB2PCENR |= (clock))
#define rcc_apb2_clk_disable(clock) (RCC->APB2PCENR &= ~(clock))
#define rcc_apb1_clk_enable(clock) (RCC->APB1PCENR |= (clock))
#define rcc_apb1_clk_disable(clock) (RCC->APB1PCENR &= ~(clock))

void rcc_system_init(void);
uint8_t rcc_hse_init(uint32_t hse_options);
void rcc_init_pll(uint32_t pll_options);
uint32_t rcc_compute_sysclk_freq();
uint32_t rcc_compute_hclk_freq();
uint32_t rcc_compute_pclk1_freq();
uint32_t rcc_compute_pclk2_freq();
uint32_t rcc_compute_adcclk();

#endif /* USER_LIBS_CH32V203_RCC_H_ */
