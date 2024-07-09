#ifndef _CH552_QUADRATURE_ENCODER_H_
#define _CH552_QUADRATURE_ENCODER_H_

extern volatile UINT16 qenc_count;
extern volatile UINT16 qenc_raw_count;

void qenc_init();

#endif
