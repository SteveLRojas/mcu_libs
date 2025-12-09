#ifndef _CH559_HT1621_H_
#define _CH559_HT1621_H_

//HINT: The glass type defines the order and mapping of the display segments
#define HT1621_GLASS_TYP_STANDARD	0
#define HT1621_GLASS_TYP_GDM0103	1

//HINT: Pin options. Use defines from GPIO library.
#define HT1621_PORT_CS		GPIO_PORT_0
#define HT1621_PORT_WR		GPIO_PORT_0
#define HT1621_PORT_DATA	GPIO_PORT_0
#define HT1621_PIN_CS		GPIO_PIN_5
#define HT1621_PIN_WR		GPIO_PIN_6
#define HT1621_PIN_DATA		GPIO_PIN_7

//HINT: Display options
#define HT1621_GLASS_TYPE	HT1621_GLASS_TYP_GDM0103
#define HT1621_NUM_DIGITS	8

#define HT1621_CMD_SYS_DIS	0x00
#define HT1621_CMD_SYS_EN	0x01
#define HT1621_CMD_LCD_OFF	0x02
#define HT1621_CMD_LCD_ON	0x03
#define HT1621_CMD_TIM_DIS	0x04
#define HT1621_CMD_WDT_DIS	0x05
#define HT1621_CMD_TIM_EN	0x06
#define HT1621_CMD_WDT_EN	0x07
#define HT1621_CMD_TONE_OFF	0x08
#define HT1621_CMD_TONE_ON	0x09
#define HT1621_CMD_CLR_TIM	0x0C
#define HT1621_CMD_CLR_WDT	0x0E
#define HT1621_CMD_XTAL_32K	0x14
#define HT1621_CMD_RC_256K	0x18
#define HT1621_CMD_EXT_256K	0x1C
#define HT1621_CMD_BIAS_COM	0x20
#define HT1621_CMD_TONE_4K	0x40
#define HT1621_CMD_TONE_2K	0x60
#define HT1621_CMD_IRQ_DIS	0x80
#define HT1621_CMD_IRQ_EN	0x81
#define HT1621_CMD_F1		0xA0
#define HT1621_CMD_F2		0xA1
#define HT1621_CMD_F4		0xA2
#define HT1621_CMD_F8		0xA3
#define HT1621_CMD_F16		0xA4
#define HT1621_CMD_F32		0xA5
#define HT1621_CMD_F64		0xA6
#define HT1621_CMD_F128		0xA7

#define HT1621_BIAS_1_2	0x00
#define HT1621_BIAS_1_3	0x01
#define HT1621_COM_2	0x00
#define HT1621_COM_3	0x04
#define HT1621_COM_4	0x08

#if (HT1621_GLASS_TYPE == HT1621_GLASS_TYP_GDM0103)
#define HT1621_SEG_A	0x08
#define HT1621_SEG_B	0x04
#define HT1621_SEG_C	0x02
#define HT1621_SEG_D	0x10
#define HT1621_SEG_E	0x20
#define HT1621_SEG_F	0x80
#define HT1621_SEG_G	0x40
#define HT1621_SEG_DP	0x01
#else
#define HT1621_SEG_A	0x01
#define HT1621_SEG_B	0x02
#define HT1621_SEG_C	0x04
#define HT1621_SEG_D	0x80
#define HT1621_SEG_E	0x40
#define HT1621_SEG_F	0x10
#define HT1621_SEG_G	0x20
#define HT1621_SEG_DP	0x08
#endif

extern UINT8 code ht1621_hex_tab[16];
extern UINT8 ht1621_buf[HT1621_NUM_DIGITS];

#define ht1621_set_digit(idx, val) (ht1621_buf[idx] = ht1621_hex_tab[val])
#define ht1621_set_dp(idx, val) (ht1621_buf[idx] = val ? (ht1621_buf[idx] | HT1621_SEG_DP) : (ht1621_buf[idx] & ~ HT1621_SEG_DP))
#define ht1621_digit_off(idx) (ht1621_buf[idx] = 0x00)

void ht1621_init(void);
void ht1621_send_command(UINT8 command);
void ht1621_send_data(UINT8 addr, UINT8 val);
void ht1621_set_byte(UINT8 idx, UINT8 val);
void ht1621_update(void);

#endif
