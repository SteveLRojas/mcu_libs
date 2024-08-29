/*
 * ch32v203_rcc.c
 *
 *  Created on: Jul 2, 2024
 *      Author: Steve
 */
#include "ch32v20x.h"
#include "ch32v203_rcc.h"

void rcc_system_init(void)
{
	rcc_hsi_on();
	RCC->CFGR0 &= ~(RCC_CFGR0_MCO | RCC_USBPRE | RCC_PPRE2 | RCC_PPRE1 | RCC_HPRE | RCC_SW);
	rcc_pll_off();
	rcc_hsi_pre_div1();
	RCC->INTR = RCC_PLLRDYC | RCC_HSERDYC | RCC_HSIRDYC | RCC_LSERDYC | RCC_LSIRDYC;
#if RCC_USE_HSE
	if(rcc_hse_init(0x00))
	{
		rcc_hpre_config(RCC_HCLK_FREQ_DIV);
		rcc_ppre1_config(RCC_PCLK1_FREQ_DIV);
		rcc_ppre2_config(RCC_PCLK2_FREQ_DIV);
		rcc_adcpre_config(RCC_ADCCLK_FREQ_DIV);
		rcc_usbpre_config(RCC_USBCLK_FREQ_DIV);

		rcc_init_pll(RCC_PLL_FREQ_MUL | RCC_PLLSRC);
		rcc_sw_config(RCC_SW_PLL);
		while ((RCC->CFGR0 & RCC_SWS) != RCC_SWS_PLL);
	}
#else
	rcc_hpre_config(RCC_HCLK_FREQ_DIV);
	rcc_ppre1_config(RCC_PCLK1_FREQ_DIV);
	rcc_ppre2_config(RCC_PCLK2_FREQ_DIV);

	rcc_init_pll(RCC_PLL_FREQ_MUL);
	rcc_sw_config(RCC_SW_PLL);
	while ((RCC->CFGR0 & RCC_SWS) != RCC_SWS_PLL);
#endif
}

uint8_t rcc_hse_init(uint32_t hse_options)
{
	rcc_hse_off();
	RCC->CTLR &= ~(RCC_CSSON | RCC_HSEBYP);
	RCC->CTLR |= hse_options;
	rcc_hse_on();

	uint16_t hse_timeout = 0;
	do
	{
		++hse_timeout;
	} while(!(RCC->CTLR & RCC_HSERDY) && (hse_timeout != HSE_STARTUP_TIMEOUT));

	return (RCC->CTLR & RCC_HSERDY) ? 0x01 : 0x00;
}

void rcc_init_pll(uint32_t pll_options)
{
	rcc_pll_off();
	RCC->CFGR0 &= ~(RCC_PLLMULL | RCC_PLLXTPRE | RCC_PLLSRC);
	RCC->CFGR0 |= pll_options;
	rcc_pll_on();
	while(!(RCC->CTLR & RCC_PLLRDY));
	return;
}

uint32_t rcc_compute_sysclk_freq()
{
	uint32_t pll_src;
	uint32_t pll_clk;
	uint32_t pll_mul;

	pll_src = RCC->CFGR0 & RCC_SWS;
	switch(pll_src)
	{
	case RCC_SWS_HSI:
		return 8000000;
	case RCC_SWS_HSE:
		return RCC_HSE_FREQ;
	case RCC_SWS_PLL:
		pll_mul = RCC->CFGR0 & RCC_PLLMULL;
		pll_mul = pll_mul >> 18;
		pll_mul += 2;
		if(pll_mul == 17)
			pll_mul = 18;

		if(RCC->CFGR0 & RCC_PLLSRC)
		{
			pll_clk = RCC_HSE_FREQ;
			if(RCC->CFGR0 & RCC_PLLXTPRE)
				pll_clk = pll_clk >> 1;
		}
		else
		{
			if(EXTEN->EXTEN_CTR & EXTEN_PLL_HSI_PRE)
				pll_clk = 8000000;
			else
				pll_clk = 4000000;
		}

		return pll_mul * pll_clk;
	default:
		return 0;
	}
}

uint32_t rcc_compute_hclk_freq()
{
	uint32_t sysclk = rcc_compute_sysclk_freq();
	uint32_t hclk_div = RCC->CFGR0 & RCC_HPRE;

	if(hclk_div & RCC_HPRE_3)
	{
		// [7:4] HPRE[3:0]
		hclk_div = ((hclk_div & ~RCC_HPRE_3) >> 4) + 1; // # times to div sysclk by 2
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
	uint32_t pclk1_div = RCC->CFGR0 & RCC_PPRE1;

	if(pclk1_div & RCC_PPRE1_2)
	{
		// [10:8] PPRE1[2:0]
		pclk1_div = ((pclk1_div & ~RCC_PPRE1_2) >> 8) + 1; // # times to div hclk by 2
	}
	else
	{
		pclk1_div = 0;
	}

	return (hclk >> pclk1_div);
}

uint32_t rcc_compute_pclk2_freq()
{
	uint32_t hclk = rcc_compute_hclk_freq();
	uint32_t pclk2_div = RCC->CFGR0 & RCC_PPRE2;

	if(pclk2_div & RCC_PPRE2_2)
	{
		// [13:11] PPRE2[2:0]
		pclk2_div = ((pclk2_div & ~RCC_PPRE2_2) >> 11) + 1; // # times to div hclk by 2
	}
	else
	{
		pclk2_div = 0;
	}

	return (hclk >> pclk2_div);
}

uint32_t rcc_compute_adcclk()
{
	uint32_t pclk2 = rcc_compute_pclk2_freq();
	uint32_t addclk_div = RCC->CFGR0 & RCC_ADCPRE;

	// [15:14] ADCPRE[1:0]
	addclk_div = (addclk_div >> 13) + 2; // ADCPRE * 2 + 2

	return (pclk2 / addclk_div);
}
