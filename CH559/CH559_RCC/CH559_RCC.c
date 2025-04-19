#define NO_XSFR_DEFINE
#include "CH559.H"
#include "CH559_RCC.h"

UINT8 rcc_wdog_reload_val = 0;
UINT16 rcc_wdog_overflow_count = 0;
void (*rcc_wdog_callback)(void) = NULL;

//HINT: The watchdog counter counts up every 262144 system clock cycles.
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
	if(pll_config == 0x86)
	{
		//56 MHz system clock, Fpll = 336 MHz, Kusb = 7
		SAFE_MOD = 0x55;
		SAFE_MOD = 0xAA;
		PLL_CFG = 0xFC;
		CLOCK_CFG = 0x06 | (CLOCK_CFG & ~MASK_SYS_CK_DIV);
		SAFE_MOD = 0xFF;
	}
	else
	{
		//Fpll = 288 MHz, Kusb = 6
		SAFE_MOD = 0x55;
		SAFE_MOD = 0xAA;
		PLL_CFG = 0xD8;
		CLOCK_CFG = pll_config | (CLOCK_CFG & ~MASK_SYS_CK_DIV);
		SAFE_MOD = 0xFF;
	}
}

void rcc_enable_periph_clk(UINT8 periphs)
{
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	SLEEP_CTRL &= ~periphs;
	SAFE_MOD = 0xFF;
}

void rcc_disable_periph_clk(UINT8 periphs)
{
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	SLEEP_CTRL |= periphs;
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
#if		FREQ_SYS >= 34000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 36000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 38000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 40000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 42000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 44000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 46000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 48000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 50000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 52000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 54000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 56000000
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
