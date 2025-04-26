#ifndef _CH559_RCC_H_
#define _CH559_RCC_H_

//set this to the intended system speed (used for delay loops)
#define	 FREQ_SYS	12000000ul

//Use for rcc_set_wdog_rst_en and rcc_set_wdog_int_en
#define RCC_WDOG_ENABLED	0x01
#define RCC_WDOG_DISABLED	0x00

//Use RCC_CLK_OSC_EN to enable the external oscillator before using RCC_CLK_SRC_EXT
#define RCC_CLK_SRC_INT		0x80
#define RCC_CLK_OSC_EN		0xC0
#define RCC_CLK_SRC_EXT		0x40

//HINT: Datasheet says "Priority-use-of lower Fsys to reduce dynamic power dissipation and get wider Working temperature"...
// The MCU might not work at high temperature if a high clock speed is selected.
// No minimum system clock speed is specified for USB operation, but CH552 datasheet specifies at least 6MHz for USB full-speed.
#define RCC_CLK_FREQ_9M		0x00
#define RCC_CLK_FREQ_9M6	0x1E
#define RCC_CLK_FREQ_12M	0x18
#define RCC_CLK_FREQ_18M	0x10
#define RCC_CLK_FREQ_24M	0x0C
#define RCC_CLK_FREQ_28M8	0x0A
#define RCC_CLK_FREQ_36M	0x08
#define RCC_CLK_FREQ_48M	0x06
#define RCC_CLK_FREQ_56M	0x86

#define RCC_PERIPH_USB		0x80
#define RCC_PERIPH_ADC		0x40
#define RCC_PERIPH_UART1	0x20
#define RCC_PERIPH_PWM1SPI1	0x10
#define RCC_PERIPH_SPI0		0x08
#define RCC_PERIPH_TIM3		0x04
#define RCC_PERIPH_LED		0x02
#define RCC_PERIPH_XRAM		0x01

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
void rcc_enable_periph_clk(UINT8 periphs);
void rcc_disable_periph_clk(UINT8 periphs);
void rcc_soft_reset(void);
void rcc_delay_us(UINT16 us);
void rcc_delay_ms(UINT16 ms);

#endif
