#ifndef _CH552_TKEY_H_
#define _CH552_TKEY_H_

#define TKEY_TIN0_P10 0x01
#define TKEY_TIN1_P11 0x02
#define TKEY_TIN2_P14 0x03
#define TKEY_TIN3_P15 0x04
#define TKEY_TIN4_P16 0x05
#define TKEY_TIN5_P17 0x06

#define TKEY_SLOW 0x10
#define TKEY_FAST 0x00

#define TKEY_DEFAULT_THRESHOLD 0x20

extern volatile UINT16 tkey_results_raw[6];
extern volatile UINT8 tkey_results[6];
extern UINT8 tkey_schedule[6];
extern UINT8 tkey_schedule_len;
extern volatile UINT8 tkey_pending_samples;
extern signed short tkey_threshold;

void tkey_init(UINT8 tkey_speed);
void tkey_init_schedule(void);
void tkey_calibrate(void);
void tkey_run_schedule(UINT8 num_samples);
void tkey_start_single(UINT8 channel);
UINT8 tkey_finish_single(void);
UINT8 tkey_read_single(UINT8 channel);
UINT16 tkey_finish_single_raw(void);
UINT16 tkey_read_single_raw(UINT8 channel);

#endif
