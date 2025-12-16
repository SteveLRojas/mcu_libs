#ifndef _CH559_TM1622_H_
#define _CH559_TM1622_H_

//HINT: Pin options. Use defines from GPIO library.
#define TM1622_PORT_CS		GPIO_PORT_0
#define TM1622_PORT_WR		GPIO_PORT_0
#define TM1622_PORT_DATA	GPIO_PORT_0
#define TM1622_PIN_CS		GPIO_PIN_7
#define TM1622_PIN_WR		GPIO_PIN_6
#define TM1622_PIN_DATA		GPIO_PIN_5

//HINT: Display options
#define TM1622_NUM_DIGITS	10

#define TM1622_CMD_SYS_DIS	0x00
#define TM1622_CMD_SYS_EN	0x01
#define TM1622_CMD_LCD_OFF	0x02
#define TM1622_CMD_LCD_ON	0x03
#define TM1622_CMD_TIM_DIS	0x04
#define TM1622_CMD_WDT_DIS	0x05
#define TM1622_CMD_TIM_EN	0x06
#define TM1622_CMD_WDT_EN	0x07
#define TM1622_CMD_TONE_OFF	0x08
#define TM1622_CMD_TONE_ON	0x09
#define TM1622_CMD_CLR_TIM	0x0D
#define TM1622_CMD_CLR_WDT	0x0F
#define TM1622_CMD_RC_32K	0x18
#define TM1622_CMD_EXT_32K	0x1C
#define TM1622_CMD_TONE_4K	0x40
#define TM1622_CMD_TONE_2K	0x60
#define TM1622_CMD_IRQ_DIS	0x80
#define TM1622_CMD_IRQ_EN	0x81
#define TM1622_CMD_F1		0xA0
#define TM1622_CMD_F2		0xA1
#define TM1622_CMD_F4		0xA2
#define TM1622_CMD_F8		0xA3
#define TM1622_CMD_F16		0xA4
#define TM1622_CMD_F32		0xA5
#define TM1622_CMD_F64		0xA6
#define TM1622_CMD_F128		0xA7

#define TM1622_SEG_A1	0x0008
#define TM1622_SEG_A2	0x0800
#define TM1622_SEG_B	0x0200
#define TM1622_SEG_C	0x2000
#define TM1622_SEG_D1	0x0010
#define TM1622_SEG_D2	0x1000
#define TM1622_SEG_E	0x0020
#define TM1622_SEG_F	0x0002
#define TM1622_SEG_G1	0x0001
#define TM1622_SEG_G2	0x8000
#define TM1622_SEG_H	0x0004
#define TM1622_SEG_I	0x0400
#define TM1622_SEG_J	0x0100
#define TM1622_SEG_K	0x0080
#define TM1622_SEG_L	0x0040
#define TM1622_SEG_M	0x4000

#define TM1622_SEG_DP1	0x0002
#define TM1622_SEG_DP2	0x0004
#define TM1622_SEG_DP3	0x0008
#define TM1622_SEG_DP4	0x0010
#define TM1622_SEG_DP5	0x0020
#define TM1622_SEG_DP6	0x0040
#define TM1622_SEG_DP7	0x0080
#define TM1622_SEG_DP8	0x0100
#define TM1622_SEG_DP9	0x0200

extern UINT16 code tm1622_ascii_tab[128];
extern UINT16 tm1622_buf[TM1622_NUM_DIGITS];
extern UINT16 tm1622_dp_buf;

#define tm1622_set_dp(idx, val) (tm1622_dp_buf = (val) ? (tm1622_dp_buf | (0x0001 << (idx))) : (tm1622_dp_buf & ~(0x0001 << (idx))))
#define tm1622_dp_off(idx) (tm1622_dp_buf = tm1622_dp_buf & ~(0x0001 << (idx)))
#define tm1622_dp_on(idx) (tm1622_dp_buf = tm1622_dp_buf | (0x0001 << (idx)))
#define tm1622_digit_off(idx) (tm1622_buf[idx] = 0x0000)

void tm1622_init(void);
void tm1622_send_command(UINT8 command);
void tm1622_send_data(UINT8 addr, UINT8 val);
void tm1622_write_char(UINT8 idx, char val);
void tm1622_write_string(UINT8 idx, char* str);
void tm1622_set_digit(UINT8 idx, UINT8 val);
void tm1622_update(void);

#endif
