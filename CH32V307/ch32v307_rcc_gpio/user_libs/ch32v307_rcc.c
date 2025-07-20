/*
 * ch32v307_rcc.c
 *
 *  Created on: Jul 19, 2025
 *      Author: Steve
 */
#include "ch32v30x.h"
#include "ch32v307_rcc.h"

void rcc_system_init(void)
{
	rcc_hsi_on();
	RCC->CFGR0 &= ~(RCC_CFGR0_MCO | RCC_USBPRE | RCC_ADCPRE | RCC_PPRE2 | RCC_PPRE1 | RCC_HPRE | RCC_SW);
	RCC->CFGR2 = 0x00000000;
	rcc_pll_off();
	rcc_pll2_off();
	rcc_pll3_off();
	rcc_hsi_pre_div1();
	RCC->INTR = RCC_CSSC | RCC_PLL3RDYC | RCC_PLL2RDYC | RCC_PLLRDYC | RCC_HSERDYC | RCC_HSIRDYC | RCC_LSERDYC | RCC_LSIRDYC;
#if RCC_USE_HSE
	if(rcc_hse_init(0x00))
	{
		rcc_prediv1_src_hse();
		rcc_prediv1_config(RCC_PREDIV1_FREQ_DIV);
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
	rcc_hse_off();
	rcc_hsi_pre_div1();
	rcc_hpre_config(RCC_HCLK_FREQ_DIV);
	rcc_ppre1_config(RCC_PCLK1_FREQ_DIV);
	rcc_ppre2_config(RCC_PCLK2_FREQ_DIV);
	rcc_adcpre_config(RCC_ADCCLK_FREQ_DIV);
	rcc_usbpre_config(RCC_USBCLK_FREQ_DIV);

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
	} while(!(RCC->CTLR & RCC_HSERDY) && (hse_timeout != RCC_HSE_STARTUP_TIMEOUT));

	return (RCC->CTLR & RCC_HSERDY) ? 0x01 : 0x00;
}

void rcc_init_pll(uint32_t pll_options)
{
	rcc_pll_off();
	RCC->CFGR0 &= ~(RCC_PLLMULL | RCC_PLLSRC);
	RCC->CFGR0 |= pll_options;
	rcc_pll_on();
	while(!(RCC->CTLR & RCC_PLLRDY));
	return;
}

void rcc_init_pll2(uint32_t pll2_mul)
{
	rcc_pll2_off();
	rcc_pll2mul_config(pll2_mul);
	while(!(RCC->CTLR & RCC_PLL2RDY));
	return;
}

void rcc_init_pll3(uint32_t pll3_mul)
{
	rcc_pll3_off();
	rcc_pll3mul_config(pll3_mul);
	while(!(RCC->CTLR & RCC_PLL3RDY));
	return;
}

//HINT: the PLL LUT has units of 0.5x to accommodate x.5 settings
static const uint8_t pll_mul_lut[16] = {18 * 2, 3 * 2, 4 * 2, 5 * 2, 6 * 2, 7 * 2, 8 * 2, 9 * 2, 10 * 2, 11 * 2, 12 * 2, 13 * 2, 14 * 2, 13, 15 * 2, 16 * 2};
static const uint8_t pll2_mul_lut[16] = {5, 25, 4 * 2, 5 * 2, 6 * 2, 7 * 2, 8 * 2, 9 * 2, 10 * 2, 11 * 2, 12 * 2, 13 * 2, 14 * 2, 15 * 2, 16 * 2, 20 * 2};
uint32_t rcc_compute_sysclk_freq()
{
	uint32_t pll_src;
	uint32_t pll_clk;
	uint32_t pll_mul;
	uint32_t prediv;

	pll_src = RCC->CFGR0 & RCC_SWS;
	switch(pll_src)
	{
	case RCC_SWS_HSI:
		return 8000000;
	case RCC_SWS_HSE:
		return RCC_HSE_FREQ;
	case RCC_SWS_PLL:
		if(RCC->CFGR0 & RCC_PLLSRC)	//PREDIV1
		{
			if(RCC->CFGR2 & RCC_PREDIV1SRC)	//PLL2
			{
				prediv = (RCC->CFGR2 & RCC_PREDIV2) >> 4;	//prediv2 setting
				pll_clk = RCC_HSE_FREQ;
				pll_clk = pll_clk / (prediv + 1);
				pll_mul = RCC->CFGR2 & RCC_PLL2MUL;
				pll_mul = pll_mul >> 8;
				pll_mul = pll2_mul_lut[pll_mul];
				pll_clk = pll_clk * pll_mul;	//pll_mul is in units of 0.5x
				pll_clk = pll_clk >> 1;	//PLL2 freq
			}
			else	//HSE
			{
				pll_clk = RCC_HSE_FREQ;
			}
			prediv = RCC->CFGR2 & RCC_PREDIV1;
			pll_clk = pll_clk / (prediv +  1);
		}
		else	//HSI
		{
			if(EXTEN->EXTEN_CTR & EXTEN_PLL_HSI_PRE)
				pll_clk = 8000000;
			else
				pll_clk = 4000000;
		}

		pll_mul = RCC->CFGR0 & RCC_PLLMULL;
		pll_mul = pll_mul >> 18;
		pll_mul = pll_mul_lut[pll_mul];

		pll_clk = pll_clk * pll_mul;
		pll_clk = pll_clk >> 1;	//pll_mul is in units of 0.5x
		return pll_clk;
	default:
		return 0;
	}
}

uint32_t rcc_compute_hclk_freq()
{
	uint32_t sysclk = rcc_compute_sysclk_freq();
	uint32_t hclk_div = (RCC->CFGR0 & RCC_HPRE) >> 4;

	if(hclk_div & 0x08)
	{
		hclk_div = hclk_div & 0x07;
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
	uint32_t pclk1_div = (RCC->CFGR0 & RCC_PPRE1) >> 8;

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
	uint32_t pclk1_div = (RCC->CFGR0 & RCC_PPRE1) >> 8;

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
	uint32_t pclk2_div = (RCC->CFGR0 & RCC_PPRE2) >> 11;

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
	uint32_t pclk2_div = (RCC->CFGR0 & RCC_PPRE2) >> 11;

	if(!(pclk2_div & 0x04))
	{
		pclk2_div = 0;
	}
	pclk2_div &= 0x03;

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
