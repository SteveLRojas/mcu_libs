#ifndef _CH552_PS2H_KB_H_
#define _CH552_PS2H_KB_H_

//HINT: Read the VERY IMPORTANT warning in CH552_PS2_HOST.h. This library may conflict with the GPIO library.

//HINT: The RX buffer size must be a power of 2
#define PS2H_KB_RX_BUF_SIZE 	8
#define PS2H_KB_MAX_NUM_PRESSED 6
#define PS2H_KB_RETRY_LIMIT		2
#define PS2H_KB_USE_RX_BENCHMARK_PIN	1
#define PS2H_KB_USE_TIM_BENCHMARK_PIN	1
#define PS2H_KB_RX_BENCHMARK_PIN		PWM1
#define PS2H_KB_TIM_BENCHMARK_PIN		PWM2

//HINT: user should not manually clear the BAT_PASSED bit before the READY bit is set.
#define PS2H_KB_STAT_NO_ERROR	0x00
#define PS2H_KB_STAT_DATA_ERROR	0x01
#define PS2H_KB_STAT_PROT_ERROR	0x02
#define PS2H_KB_STAT_TIM_ERROR	0x04
#define PS2H_KB_STAT_ROLLOVER	0x08
#define PS2H_KB_STAT_PAUSE_KEY	0x10
#define PS2H_KB_STAT_BAT_PASSED	0x20
#define PS2H_KB_STAT_READY		0x40

//HINT: Typematic rate in characters per second. P stands for decimal point.
#define PS2H_KB_RATE_30P0		0x00
#define PS2H_KB_RATE_26P7		0x01
#define PS2H_KB_RATE_24P0		0x02
#define PS2H_KB_RATE_21P8		0x03
#define PS2H_KB_RATE_20P7		0x04
#define PS2H_KB_RATE_18P5		0x05
#define PS2H_KB_RATE_17P1		0x06
#define PS2H_KB_RATE_16P0		0x07
#define PS2H_KB_RATE_15P0		0x08
#define PS2H_KB_RATE_13P3		0x09
#define PS2H_KB_RATE_12P0		0x0A
#define PS2H_KB_RATE_10P9		0x0B
#define PS2H_KB_RATE_10P0		0x0C
#define PS2H_KB_RATE_9P2		0x0D
#define PS2H_KB_RATE_8P6		0x0E
#define PS2H_KB_RATE_8P0		0x0F
#define PS2H_KB_RATE_7P5		0x10
#define PS2H_KB_RATE_6P7		0x11
#define PS2H_KB_RATE_6P0		0x12
#define PS2H_KB_RATE_5P5		0x13
#define PS2H_KB_RATE_5P0		0x14
#define PS2H_KB_RATE_4P6		0x15
#define PS2H_KB_RATE_4P3		0x16
#define PS2H_KB_RATE_4P0		0x17
#define PS2H_KB_RATE_3P7		0x18
#define PS2H_KB_RATE_3P3		0x19
#define PS2H_KB_RATE_3P0		0x1A
#define PS2H_KB_RATE_2P7		0x1B
#define PS2H_KB_RATE_2P5		0x1C
#define PS2H_KB_RATE_2P3		0x1D
#define PS2H_KB_RATE_2P1		0x1E
#define PS2H_KB_RATE_2P0		0x1F

//HINT:	Typematic delay in seconds. P stands for decimal point
#define PS2H_KB_DELAY_0P25		0x00
#define PS2H_KB_DELAY_0P50		0x20
#define PS2H_KB_DELAY_0P75		0x40
#define PS2H_KB_DELAY_1P00		0x60

#define PS2H_KB_LED_SCROLL_LOCK	0x01
#define PS2H_KB_LED_NUM_LOCK	0x02
#define PS2H_KB_LED_CAPS_LOCK	0x04

#define PS2H_KB_SCAN_CODE_SET_1	0x01
#define PS2H_KB_SCAN_CODE_SET_2	0x02
#define PS2H_KB_SCAN_CODE_SET_3	0x03

//HINT: PS/2 keyboard commands
#define PS2H_KB_COM_1B_RESET					0xFF
#define PS2H_KB_COM_1B_RESEND					0xFE
#define PS2H_KB_COM_1B_SET_KEY_TYPE_MAKE		0xFD
#define PS2H_KB_COM_1B_SET_KEY_TYPE_MAKE_BREAK	0xFC
#define PS2H_KB_COM_1B_SET_KEY_TYPE_TYPEMATIC	0xFB
#define PS2H_KB_COM_1B_SET_ALL_KEYS_MAKE		0xF9
#define PS2H_KB_COM_1B_SET_ALL_KEYS_MAKE_BREAK	0xF8
#define PS2H_KB_COM_1B_SET_ALL_KEYS_TYPEMATIC	0xF7
#define PS2H_KB_COM_1B_SET_DEFAULT				0xF6
#define PS2H_KB_COM_1B_DISABLE					0xF5
#define PS2H_KB_COM_1B_ENABLE					0xF4
#define PS2H_KB_COM_1B_READ_ID					0xF2
#define PS2H_KB_COM_1B_ECHO						0xEE
#define PS2H_KB_COM_2B_SET_TYPEMATIC_RATE_DELAY	0xF3
#define PS2H_KB_COM_2B_SET_SCAN_CODE_SET		0xF0
#define PS2H_KB_COM_2B_SET_LED					0xED

extern volatile UINT16 ps2h_kb_pressed_keys[PS2H_KB_MAX_NUM_PRESSED];
extern volatile UINT8 ps2h_kb_status;

#define ps2h_kb_set_default() (ps2h_kb_send_1b_command(PS2H_KB_COM_1B_SET_DEFAULT))
#define ps2h_kb_disable() (ps2h_kb_send_1b_command(PS2H_KB_COM_1B_DISABLE))
#define ps2h_kb_enable() (ps2h_kb_send_1b_command(PS2H_KB_COM_1B_ENABLE))
#define ps2h_kb_set_typematic_rate_delay(rate_delay) (ps2h_kb_send_2b_command(PS2H_KB_COM_2B_SET_TYPEMATIC_RATE_DELAY, (rate_delay)))
#define ps2h_kb_set_scan_code_set(scan_code_set) (ps2h_kb_send_2b_command(PS2H_KB_COM_2B_SET_SCAN_CODE_SET, (scan_code_set)))
#define ps2h_kb_set_led(indicators) (ps2h_kb_send_2b_command(PS2H_KB_COM_2B_SET_LED, (indicators)))

void ps2h_kb_init(UINT8 timer);
void ps2h_kb_send_1b_command(UINT8 command);
void ps2h_kb_send_2b_command(UINT8 command, UINT8 val);
void ps2h_kb_reset(void);
void ps2h_kb_update_keys(void);

#endif
