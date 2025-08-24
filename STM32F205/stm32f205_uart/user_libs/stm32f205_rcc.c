/*
 * stm32f205_rcc.c
 *
 *  Created on: Aug 11, 2025
 *      Author: Steve
 */
#include "stm32f205xx.h"
#include "stm32f205_rcc.h"

void rcc_system_init(void)
{
	FLASH->ACR = FLASH_ACR_DCEN | FLASH_ACR_ICEN | FLASH_ACR_PRFTEN | RCC_FLASH_LATENCY;
	while((FLASH->ACR & FLASH_ACR_LATENCY) != RCC_FLASH_LATENCY);
	rcc_hsi_on();
	RCC->CFGR = 0;
	rcc_pll_off();
	rcc_plli2soff();
	RCC->CIR = RCC_CIR_CSSC | RCC_CIR_PLLI2SRDYC | RCC_CIR_PLLRDYC | RCC_CIR_HSERDYC | RCC_CIR_HSIRDYC | RCC_CIR_LSERDYC | RCC_CIR_LSIRDYC;
#if RCC_USE_HSE
	rcc_hse_on();
	while(!rcc_hse_is_ready());

	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLQ | RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLP | RCC_PLLCFGR_PLLN | RCC_PLLCFGR_PLLM);
	RCC->PLLCFGR |= (RCC_PLL_USB_FREQ_DIV << RCC_PLLCFGR_PLLQ_Pos) | RCC_PLLCFGR_PLLSRC_HSE | (RCC_PLL_SYS_FREQ_DIV) | (RCC_PLL_FREQ_MUL << RCC_PLLCFGR_PLLN_Pos) | RCC_PLL_PREDIV;
	rcc_pll_on();
	while(!rcc_pll_is_ready());

	rcc_hpre_config(RCC_HCLK_FREQ_DIV);
	rcc_ppre1_config(RCC_PCLK1_FREQ_DIV);
	rcc_ppre2_config(RCC_PCLK2_FREQ_DIV);
	rcc_sw_config(RCC_CFGR_SW_PLL);
	while(rcc_get_sys_clk_src() != RCC_CFGR_SWS_PLL);
	rcc_hsi_off();
#else
	while(!rcc_hsi_is_ready());

	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLQ | RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLP | RCC_PLLCFGR_PLLN | RCC_PLLCFGR_PLLM);
	RCC->PLLCFGR |= (RCC_PLL_USB_FREQ_DIV << RCC_PLLCFGR_PLLQ_Pos) | RCC_PLLCFGR_PLLSRC_HSI | (RCC_PLL_SYS_FREQ_DIV) | (RCC_PLL_FREQ_MUL << RCC_PLLCFGR_PLLN_Pos) | RCC_PLL_PREDIV;
	rcc_pll_on();
	while(!rcc_pll_is_ready());

	rcc_hpre_config(RCC_HCLK_FREQ_DIV);
	rcc_ppre1_config(RCC_PCLK1_FREQ_DIV);
	rcc_ppre2_config(RCC_PCLK2_FREQ_DIV);
	rcc_sw_config(RCC_CFGR_SWS_PLL);
	while(rcc_get_sys_clk_src() != RCC_CFGR_SWS_PLL);
	rcc_hse_off();
#endif
}

uint32_t rcc_compute_sysclk_freq()
{
	uint32_t pll_src;
	uint32_t pll_prediv;
	uint32_t pll_mul;
	uint32_t pll_div;
	uint32_t pll_clk;

	pll_src = rcc_get_sys_clk_src();
	switch(pll_src)
	{
	case RCC_CFGR_SWS_HSI:
		return 16000000;
	case RCC_CFGR_SWS_HSE:
		return RCC_HSE_FREQ;
	case RCC_CFGR_SWS_PLL:
		pll_prediv = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;
		pll_mul = (RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> RCC_PLLCFGR_PLLN_Pos;
		pll_div = 2 << ((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >> RCC_PLLCFGR_PLLP_Pos);

		pll_clk = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) ? RCC_HSE_FREQ : 16000000;
		pll_clk = pll_clk / pll_prediv;
		pll_clk = pll_clk * pll_mul;
		pll_clk = pll_clk / pll_div;
		return pll_clk;
	default:
		return 0;
	}
}

uint32_t rcc_compute_hclk_freq()
{
	uint32_t sysclk = rcc_compute_sysclk_freq();
	uint32_t hclk_div = (RCC->CFGR & RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos;

	if(hclk_div & 0x08)
	{
		hclk_div &= 0x07;
		if(hclk_div & 0x04)
			hclk_div += 2; // # times to div sysclk by 2
		else
			hclk_div += 1; // # times to div sysclk by 2
	}
	else
	{
		hclk_div = 0;
	}

	return (sysclk >> hclk_div);
}

uint32_t rcc_compute_pclk1_freq()
{
	uint32_t hclk = rcc_compute_hclk_freq();
	uint32_t pclk1_div = (RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos;

	if(pclk1_div & 0x04)
	{
		pclk1_div &= 0x03;
		pclk1_div += 1; // # times to div hclk by 2
	}
	else
	{
		pclk1_div = 0;
	}

	return (hclk >> pclk1_div);
}
uint32_t rcc_compute_pclk1_tim_freq()
{
	uint32_t hclk = rcc_compute_hclk_freq();
	uint32_t pclk1_div = (RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos;

	if(!(pclk1_div & 0x04))
	{
		pclk1_div = 0;
	}
	pclk1_div &= 0x03;

	return (hclk >> pclk1_div);
}

uint32_t rcc_compute_pclk2_freq()
{
	uint32_t hclk = rcc_compute_hclk_freq();
	uint32_t pclk2_div = (RCC->CFGR & RCC_CFGR_PPRE2) >> RCC_CFGR_PPRE2_Pos;

	if(pclk2_div & 0x04)
	{
		pclk2_div &= 0x03;
		pclk2_div += 1; // # times to div hclk by 2
	}
	else
	{
		pclk2_div = 0;
	}

	return (hclk >> pclk2_div);
}

uint32_t rcc_compute_pclk2_tim_freq()
{
	uint32_t hclk = rcc_compute_hclk_freq();
	uint32_t pclk2_div = (RCC->CFGR & RCC_CFGR_PPRE2) >> RCC_CFGR_PPRE2_Pos;

	if(!(pclk2_div & 0x04))
	{
		pclk2_div = 0;
	}
	pclk2_div &= 0x03;

	return (hclk >> pclk2_div);
}
