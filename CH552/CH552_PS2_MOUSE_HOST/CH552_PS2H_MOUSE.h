#ifndef _CH552_PS2H_MOUSE_H_
#define _CH552_PS2H_MOUSE_H_

//HINT: Read the VERY IMPORTANT warning in CH552_PS2_HOST.h. This library may conflict with the GPIO library.

//HINT: The RX buffer size must be a power of 2
#define PS2H_MS_RX_BUF_SIZE 	8
#define PS2H_MS_RETRY_LIMIT		2
#define PS2H_MS_USE_RX_BENCHMARK_PIN	1
#define PS2H_MS_USE_TIM_BENCHMARK_PIN	1
#define PS2H_MS_RX_BENCHMARK_PIN		PWM1
#define PS2H_MS_TIM_BENCHMARK_PIN		PWM2

//HINT: user should not manually clear the BAT_PASSED and DEVICE_ID bits before the READY bit is set.
//The SCROLL_EN bit should never be manually cleared.
#define PS2H_MS_STAT_NO_ERROR	0x00
#define PS2H_MS_STAT_DATA_ERROR	0x01
#define PS2H_MS_STAT_PROT_ERROR	0x02
#define PS2H_MS_STAT_TIM_ERROR	0x04
#define PS2H_MS_STAT_BAT_PASSED	0x08
#define PS2H_MS_STAT_DEVICE_ID	0x10
#define PS2H_MS_STAT_SCROLL_EN	0x20
#define PS2H_MS_STAT_READY		0x40

//HINT: sample rate in samples per second
#define PS2H_MS_SAMPLE_RATE_10	10
#define PS2H_MS_SAMPLE_RATE_20	20
#define PS2H_MS_SAMPLE_RATE_40	40
#define PS2H_MS_SAMPLE_RATE_60	60
#define PS2H_MS_SAMPLE_RATE_80	80
#define PS2H_MS_SAMPLE_RATE_100	100
#define PS2H_MS_SAMPLE_RATE_200	200

//HINT: resolution in counts per mm
#define PS2H_MS_RESOLUTION_1	0x00
#define PS2H_MS_RESOLUTION_2	0x01
#define PS2H_MS_RESOLUTION_4	0x02
#define PS2H_MS_RESOLUTION_8	0x03

#define PS2H_MS_BTNS_BTN_LEFT	0x01
#define PS2H_MS_BTNS_BTN_RIGHT	0x02
#define PS2H_MS_BTNS_BTN_CENTER	0x04
#define PS2H_MS_BTNS_ALWAYS_1	0x08
#define PS2H_MS_BTNS_SIGN_X		0x10
#define PS2H_MS_BTNS_SIGN_Y		0x20
#define PS2H_MS_BTNS_OVERFLOW_X	0x40
#define PS2H_MS_BTNS_OVERFLOW_Y	0x80

#define PS2H_MS_COM_1B_RESET				0xFF
#define PS2H_MS_COM_1B_RESEND				0xFE
#define PS2H_MS_COM_1B_SET_DEFAULTS			0xF6
#define PS2H_MS_COM_1B_DISABLE_REPORTING	0xF5
#define PS2H_MS_COM_1B_ENABLE_REPORTING		0xF4
#define PS2H_MS_COM_1B_GET_DEVICE_ID		0xF2
#define PS2H_MS_COM_1B_SET_REMOTE_MODE		0xF0
#define PS2H_MS_COM_1B_SET_WRAP_MODE		0xEE
#define PS2H_MS_COM_1B_RESET_WRAP_MODE		0xEC
#define PS2H_MS_COM_1B_READ_DATA			0xEB
#define PS2H_MS_COM_1B_SET_STREAM_MODE		0xEA
#define PS2H_MS_COM_1B_STATUS_REQUEST		0xE9
#define PS2H_MS_COM_1B_SET_SCALING_2_1		0xE7
#define PS2H_MS_COM_1B_SET_SCALING_1_1		0xE6
#define PS2H_MS_COM_2B_SET_SAMPLE_RATE		0xF3
#define PS2H_MS_COM_2B_SET_RESOLUTION		0xE8

extern volatile UINT8 ps2h_ms_status;
extern volatile UINT8 ps2h_ms_buttons;
extern volatile signed char ps2h_ms_x_rel;
extern volatile signed char ps2h_ms_y_rel;
extern volatile signed char ps2h_ms_z_rel;

#define ps2h_ms_set_defaults() (ps2h_ms_send_1b_command(PS2H_MS_COM_1B_SET_DEFAULTS))
#define ps2h_ms_disable_reporting() (ps2h_ms_send_1b_command(PS2H_MS_COM_1B_DISABLE_REPORTING))
#define ps2h_ms_enable_reporting() (ps2h_ms_send_1b_command(PS2H_MS_COM_1B_ENABLE_REPORTING))
#define ps2h_ms_set_remote_mode() (ps2h_ms_send_1b_command(PS2H_MS_COM_1B_SET_REMOTE_MODE))
#define ps2h_ms_read_data() (ps2h_ms_send_1b_command(PS2H_MS_COM_1B_READ_DATA))
#define ps2h_ms_set_stream_mode() (ps2h_ms_send_1b_command(PS2H_MS_COM_1B_SET_STREAM_MODE))
#define ps2h_ms_set_sample_rate(sample_rate) (ps2h_ms_send_2b_command(PS2H_MS_COM_2B_SET_SAMPLE_RATE, (sample_rate)))
#define ps2h_ms_set_resolution(resolution) (ps2h_ms_send_2b_command(PS2H_MS_COM_2B_SET_RESOLUTION, (resolution)))

void ps2h_ms_init(UINT8 timer);
void ps2h_ms_send_1b_command(UINT8 command);
void ps2h_ms_send_2b_command(UINT8 command, UINT8 val);
void ps2h_ms_enable_scrolling(void);
void ps2h_ms_reset(void);
void ps2h_ms_update(void);

#endif
