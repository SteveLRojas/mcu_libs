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
#define RCC_AHB_RST_ETHMAC	0x00004000
#define RCC_AHB_RST_DVP		0x00002000
#define RCC_AHB_RST_USBHD	0x00001000

#define RCC_APB1_RST_DAC	0x20000000
#define RCC_APB1_RST_PWR	0x10000000
#define RCC_APB1_RST_BKP	0x08000000
#define RCC_APB1_RST_CAN2	0x04000000
#define RCC_APB1_RST_CAN1	0x02000000
#define RCC_APB1_RST_USBD	0x00800000
#define RCC_APB1_RST_I2C2	0x00400000
#define RCC_APB1_RST_I2C1	0x00200000
#define RCC_APB1_RST_USART5	0x00100000
#define RCC_APB1_RST_USART4	0x00080000
#define RCC_APB1_RST_USART3	0x00040000
#define RCC_APB1_RST_USART2	0x00020000
#define RCC_APB1_RST_SPI3	0x00008000
#define RCC_APB1_RST_SPI2	0x00004000
#define RCC_APB1_RST_WWDG	0x00000800
#define RCC_APB1_RST_USART8	0x00000100
#define RCC_APB1_RST_USART7	0x00000080
#define RCC_APB1_RST_USART6	0x00000040
#define RCC_APB1_RST_TIM7	0x00000020
#define RCC_APB1_RST_TIM6	0x00000010
#define RCC_APB1_RST_TIM5	0x00000008
#define RCC_APB1_RST_TIM4	0x00000004
#define RCC_APB1_RST_TIM3	0x00000002
#define RCC_APB1_RST_TIM2	0x00000001

#define RCC_APB2_RST_TIM10	0x00100000
#define RCC_APB2_RST_TIM9	0x00080000
#define RCC_APB2_RST_USART1	0x00004000
#define RCC_APB2_RST_TIM8	0x00002000
#define RCC_APB2_RST_SPI1	0x00001000
#define RCC_APB2_RST_TIM1	0x00000800
#define RCC_APB2_RST_ADC2	0x00000400
#define RCC_APB2_RST_ADC1	0x00000200
#define RCC_APB2_RST_IOPE	0x00000040
#define RCC_APB2_RST_IOPD	0x00000020
#define RCC_APB2_RST_IOPC	0x00000010
#define RCC_APB2_RST_IOPB	0x00000008
#define RCC_APB2_RST_IOPA	0x00000004
#define RCC_APB2_RST_AFIO	0x00000001

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

#define rcc_ahb_reset_begin(periph) (RCC->AHBRSTR |= (periph))
#define rcc_ahb_reset_end(periph) (RCC->AHBRSTR &= ~(periph))
#define rcc_apb1_reset_begin(periph) (RCC->APB1PRSTR |= (periph))
#define rcc_apb1_reset_end(periph) (RCC->APB1PRSTR &= ~(periph))
#define rcc_apb2_reset_begin(periph) (RCC->APB2PRSTR |= (periph))
#define rcc_apb2_reset_end(periph) (RCC->APB2PRSTR &= ~(periph))

void rcc_system_init(void);
uint8_t rcc_hse_init(uint32_t hse_options);
void rcc_init_pll(uint32_t pll_options);
uint32_t rcc_compute_sysclk_freq();
uint32_t rcc_compute_hclk_freq();
uint32_t rcc_compute_pclk1_freq();
uint32_t rcc_compute_pclk1_tim_freq();
uint32_t rcc_compute_pclk2_freq();
uint32_t rcc_compute_pclk2_tim_freq();
uint32_t rcc_compute_adcclk();

#endif /* USER_LIBS_CH32V203_RCC_H_ */
