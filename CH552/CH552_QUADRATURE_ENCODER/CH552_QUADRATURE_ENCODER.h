#ifndef _CH552_QUADRATURE_ENCODER_H_
#define _CH552_QUADRATURE_ENCODER_H_

#define QENC_MODE_INTERRUPT 0
#define QENC_MODE_POLLED 1

extern volatile UINT16 qenc_count;
extern volatile UINT16 qenc_raw_count;

void qenc_init(UINT8 qenc_mode);
void qenc_poll(void);

#endif
