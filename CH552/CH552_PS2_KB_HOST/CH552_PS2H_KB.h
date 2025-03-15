#ifndef _CH552_PS2H_KB_H_
#define _CH552_PS2H_KB_H_

//HINT: Read the VERY IMPORTANT warning in CH552_PS2_HOST.h. This library may conflict with the GPIO library.

//HINT: The RX buffer size must be a power of 2
#define PS2H_KB_RX_BUF_SIZE 	8
#define PS2H_KB_MAX_NUM_PRESSED 6
#define PS2H_KB_RETRY_LIMIT		2

#define PS2H_KB_STAT_NO_ERROR	0x00
#define PS2H_KB_STAT_DATA_ERROR	0x01
#define PS2H_KB_STAT_PROT_ERROR	0x02
#define PS2H_KB_STAT_TIM_ERROR	0x04
#define PS2H_KB_STAT_ROLLOVER	0x08
#define PS2H_KB_STAT_PAUSE_KEY	0x10
#define PS2H_KB_STAT_BAT_PASSED	0x20

extern volatile UINT16 ps2h_kb_pressed_keys[PS2H_KB_MAX_NUM_PRESSED];
extern volatile UINT8 ps2h_kb_status;

void ps2h_kb_init(UINT8 timer);
void ps2h_kb_update_keys(void);
void ps2h_kb_set_led(UINT8 led_status);
void ps2h_kb_reset(void);
#endif
