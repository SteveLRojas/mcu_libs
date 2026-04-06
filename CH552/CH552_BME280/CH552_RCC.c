#include "CH552.H"
#include "CH552_RCC.h"

UINT8 rcc_wdog_reload_val = 0;
UINT16 rcc_wdog_overflow_count = 0;
void (*rcc_wdog_callback)(void) = NULL;

void wdog_isr(void) interrupt INT_NO_WDOG
{
	rcc_reload_wdog(rcc_wdog_reload_val);
	++rcc_wdog_overflow_count;
	if(rcc_wdog_callback)
		rcc_wdog_callback();
}

void rcc_set_wdog_rst_en(UINT8 rst_en)
{
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~bWDOG_EN;
	GLOBAL_CFG |= rst_en;
	SAFE_MOD = 0xFF;
}

void rcc_set_clk_src(UINT8 clk_config)
{
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	CLOCK_CFG = clk_config | (CLOCK_CFG & ~(bOSC_EN_INT | bOSC_EN_XT));
	SAFE_MOD = 0xFF;
}

void rcc_set_clk_freq(UINT8 pll_config)
{
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	CLOCK_CFG &= ~MASK_SYS_CK_SEL;
	CLOCK_CFG |= pll_config;
	SAFE_MOD = 0xFF;
}

void rcc_soft_reset(void)
{
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= bSW_RESET;
	SAFE_MOD = 0xFF;
}

void rcc_delay_us(UINT16 us)
{
	while(us)
	{  // total = 12~13 Fsys cycles, 1uS @Fsys=12MHz
		++ SAFE_MOD;  // 2 Fsys cycles, for higher Fsys, add operation here
#ifdef	FREQ_SYS
#if		FREQ_SYS >= 14000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 16000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 18000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 20000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 22000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 24000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 26000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 28000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 30000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 32000000
		++ SAFE_MOD;
#endif
#endif
		--us;
	}
}

void rcc_delay_ms(UINT16 ms)
{
	while (ms) 
	{
		rcc_delay_us(1000);
		--ms;
	}
}
