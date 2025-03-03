#ifndef _CH552_RCC_H_
#define _CH552_RCC_H_

//set this to the intended system speed (used for delay loops)
#define	 FREQ_SYS	24000000ul

//Use for rcc_set_wdog_rst_en and rcc_set_wdog_int_en
#define RCC_WDOG_ENABLED	0x01
#define RCC_WDOG_DISABLED	0x00

//Use RCC_CLK_OSC_EN to enable the external oscillator before using RCC_CLK_SRC_EXT
#define RCC_CLK_SRC_INT		0x80
#define RCC_CLK_OSC_EN		0xC0
#define RCC_CLK_SRC_EXT		0x40

#define RCC_CLK_FREQ_187K5	0x00
#define RCC_CLK_FREQ_750K	0x01
#define RCC_CLK_FREQ_3M		0x02
#define RCC_CLK_FREQ_6M		0x03
#define RCC_CLK_FREQ_12M	0x04
#define RCC_CLK_FREQ_16M	0x05
#define RCC_CLK_FREQ_24M	0x06

#define RCC_RST_TYP_SOFT	0x00
#define RCC_RST_TYP_PWRON	0x10
#define RCC_RST_TYP_WDOG	0x20
#define RCC_RST_TYP_EXTP	0x30

extern UINT8 rcc_wdog_reload_val;
extern UINT16 rcc_wdog_overflow_count;
extern void (*rcc_wdog_callback)(void);

#define rcc_get_rst_typ() (PCON & 0x30)
#define rcc_reload_wdog(count) (WDOG_COUNT = count)
#define rcc_get_wdog_int_en() (IE_WDOG)
#define rcc_set_wdog_int_en(en) (IE_WDOG = en)

void rcc_set_wdog_rst_en(UINT8 rst_en);
void rcc_set_clk_src(UINT8 clk_config);
void rcc_set_clk_freq(UINT8 pll_config);
void rcc_soft_reset(void);
void rcc_delay_us(UINT16 us);
void rcc_delay_ms(UINT16 ms);

#endif
