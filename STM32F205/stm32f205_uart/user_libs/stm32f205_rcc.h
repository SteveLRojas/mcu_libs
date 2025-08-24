/*
 * stm32f205_rcc.h
 *
 *  Created on: Aug 11, 2025
 *      Author: Steve
 */

#ifndef _STM32F205_RCC_H_
#define _STM32F205_RCC_H_

#define RCC_SYS_FREQ_DIV2	0x00000000
#define RCC_SYS_FREQ_DIV4	0x00010000
#define RCC_SYS_FREQ_DIV6	0x00020000
#define RCC_SYS_FREQ_DIV8	0x00030000

// HINT: User options for initial clock configuration
#define RCC_USE_HSE 1
#define RCC_HSE_FREQ 16000000
#define RCC_FLASH_LATENCY FLASH_ACR_LATENCY_3WS
//HINT: RCC_PLL_PREDIV has a range of 2 to 63 inclusive
#define RCC_PLL_PREDIV 8
//HINT: RCC_PLL_FREQ_MUL has a range of 192 to 432 inclusive
#define RCC_PLL_FREQ_MUL 192
#define RCC_PLL_SYS_FREQ_DIV RCC_SYS_FREQ_DIV4
//HINT: RCC_PLL_USB_FREQ_DIV has a range of 2 to 15 inclusive
#define RCC_PLL_USB_FREQ_DIV 8
#define RCC_HCLK_FREQ_DIV RCC_CFGR_HPRE_DIV1
#define RCC_PCLK1_FREQ_DIV RCC_CFGR_PPRE1_DIV4
#define RCC_PCLK2_FREQ_DIV RCC_CFGR_PPRE2_DIV2

// Do not modify anything below this line
#define rcc_plli2s_is_ready() (RCC->CR & RCC_CR_PLLI2SRDY)
#define rcc_plli2son() (RCC->CR |= RCC_CR_PLLI2SON)
#define rcc_plli2soff() (RCC->CR &= ~RCC_CR_PLLI2SON)
#define rcc_pll_is_ready() (RCC->CR & RCC_CR_PLLRDY)
#define rcc_pll_on() (RCC->CR |= RCC_CR_PLLON)
#define rcc_pll_off() (RCC->CR &= ~RCC_CR_PLLON)
#define rcc_css_on() (RCC->CR |= RCC_CR_CSSON)
#define rcc_css_off() (RCC->CR &= ~RCC_CR_CSSON)
#define rcc_hsebyp_on() (RCC->CR |= RCC_CR_HSEBYP)
#define rcc_hsebyp_off() (RCC->CR &= ~RCC_CR_HSEBYP)
#define rcc_hse_is_ready() (RCC->CR & RCC_CR_HSERDY)
#define rcc_hse_on() (RCC->CR |= RCC_CR_HSEON)
#define rcc_hse_off() (RCC->CR &= ~RCC_CR_HSEON)
#define rcc_hsi_is_ready() (RCC->CR & RCC_CR_HSIRDY)
#define rcc_hsi_on() (RCC->CR |= RCC_CR_HSION)
#define rcc_hsi_off() (RCC->CR &= ~RCC_CR_HSION)

#define rcc_usb_freq_div_config(pllq_div) (RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLQ) | ((pllq_div) << RCC_PLLCFGR_PLLQ_Pos))
#define rcc_pll_src_hse() (RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC)
#define rcc_pll_src_hsi() (RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC)
#define rcc_sys_freq_div_config(config) (RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLP) | (config))
#define rcc_pll_mul_config(plln_mul) (RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLN) | ((plln_mul) << RCC_PLLCFGR_PLLN_Pos))
#define rcc_pll_prediv_config(pllm_div) (RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLM) | (pllm_div))

#define rcc_rtcpre_config(config) (RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_RTCPRE) | (config))
#define rcc_ppre2_config(config) (RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE2) | (config))
#define rcc_ppre1_config(config) (RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE1) | (config))
#define rcc_hpre_config(config) (RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_HPRE) | (config))
#define rcc_sw_config(config) (RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | (config))
#define rcc_get_sys_clk_src() (RCC->CFGR & RCC_CFGR_SWS)

#define rcc_ahb1_set_reset(reset) (RCC->AHB1RSTR |= (reset))
#define rcc_ahb1_clear_reset(reset) (RCC->AHB1RSTR &= ~(reset))
#define rcc_ahb2_set_reset(reset) (RCC->AHB2RSTR |= (reset))
#define rcc_ahb2_clear_reset(reset) (RCC->AHB2RSTR &= ~(reset))
#define rcc_ahb3_set_reset(reset) (RCC->AHB3RSTR |= (reset))
#define rcc_ahb3_clear_reset(reset) (RCC->AHB3RSTR &= ~(reset))

#define rcc_ahb1_clk_enable(clock) (RCC->AHB1ENR |= (clock))
#define rcc_ahb1_clk_disable(clock) (RCC->AHB1ENR &= ~(clock))
#define rcc_ahb2_clk_enable(clock) (RCC->AHB2ENR |= (clock))
#define rcc_ahb2_clk_disable(clock) (RCC->AHB2ENR &= ~(clock))
#define rcc_ahb3_clk_enable(clock) (RCC->AHB3ENR |= (clock))
#define rcc_ahb3_clk_disable(clock) (RCC->AHB3ENR &= ~(clock))

#define apb1_clk_enable(clock) (RCC->APB1ENR |= (clock))
#define apb1_clk_disable(clock) (RCC->APB1ENR &= ~(clock))
#define apb2_clk_enable(clock) (RCC->APB2ENR |= (clock))
#define apb2_clk_disable(clock) (RCC->APB2ENR &= ~(clock))

void rcc_system_init(void);
uint32_t rcc_compute_sysclk_freq();
uint32_t rcc_compute_hclk_freq();
uint32_t rcc_compute_pclk1_freq();
uint32_t rcc_compute_pclk1_tim_freq();
uint32_t rcc_compute_pclk2_freq();
uint32_t rcc_compute_pclk2_tim_freq();

#endif /* _STM32F205_RCC_H_ */
