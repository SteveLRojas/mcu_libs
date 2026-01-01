#ifndef _CH559_TM1637_H_
#define _CH559_TM1637_H_

//HINT: Pin options. Use defines from GPIO library.
#define TM1637_PORT_DIO		GPIO_PORT_0
#define TM1637_PORT_CLK		GPIO_PORT_0
#define TM1637_PIN_DIO		GPIO_PIN_6
#define TM1637_PIN_CLK		GPIO_PIN_7

//HINT: Display options
#define TM1637_NUM_DIGITS	6

#define TM1637_CMD_DATA		0x40
#define TM1637_CMD_CONTROL	0x80
#define TM1637_CMD_ADDRESS	0xC0

// Options for data command
#define TM1637_OPT_WRITE_REG	0x00
#define TM1637_OPT_READ_KEYS	0x02
#define TM1637_OPT_AUTO_INC		0x00
#define TM1637_OPT_FIX_ADDR		0x04
#define TM1637_OPT_NORM_MODE	0x00
#define TM1637_OPT_TEST_MODE	0x08

// Options for control command
#define TM1637_OPT_PW_1_16		0x00
#define TM1637_OPT_PW_2_16		0x01
#define TM1637_OPT_PW_4_16		0x02
#define TM1637_OPT_PW_10_16		0x03
#define TM1637_OPT_PW_11_16		0x04
#define TM1637_OPT_PW_12_16		0x05
#define TM1637_OPT_PW_13_16		0x06
#define TM1637_OPT_PW_14_16		0x07
#define TM1637_OPT_DISP_OFF		0x00
#define TM1637_OPT_DISP_ON		0x08

#define TM1637_SEG_A	0x01
#define TM1637_SEG_B	0x02
#define TM1637_SEG_C	0x04
#define TM1637_SEG_D	0x08
#define TM1637_SEG_E	0x10
#define TM1637_SEG_F	0x20
#define TM1637_SEG_G	0x40
#define TM1637_SEG_DP	0x80

extern UINT8 code tm1637_hex_tab[16];
extern UINT8 tm1637_buf[TM1637_NUM_DIGITS];

#define tm1637_set_digit(idx, val) (tm1637_buf[idx] = tm1637_hex_tab[val])
#define tm1637_set_dp(idx, val) (tm1637_buf[idx] = val ? (tm1637_buf[idx] | TM1637_SEG_DP) : (tm1637_buf[idx] & ~ TM1637_SEG_DP))
#define tm1637_digit_off(idx) (tm1637_buf[idx] = 0x00)

void tm1637_init(UINT8 pulse_width);
void tm1637_start(void);
void tm1637_stop(void);
void tm1637_write(UINT8 val);
UINT8 tm1637_read(void);
void tm1637_set_byte(UINT8 idx, UINT8 val);
void tm1637_update(void);

#endif
