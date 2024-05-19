#ifndef _CH552_FTDI_SERIAL_H_
#define _CH552_FTDI_SERIAL_H_

extern UINT8 xdata Ep1Buffer[MAX_PACKET_SIZE];		//Endpoint 1 IN send buffer
extern UINT8 xdata Ep2Buffer[MAX_PACKET_SIZE];		//Endpoint 2 OUT receive buffer
extern UINT8 UsbConfig;
extern volatile UINT8 idata USBReceived;
extern volatile UINT8 idata UpPoint1_Busy;
extern volatile UINT8 idata Latency_Timer;
/* Download control */
extern volatile UINT8 idata USBOutLength;
extern volatile UINT8 idata USBOutPtr;
extern volatile UINT8 idata USBReceived;
/* upload control */
extern volatile UINT8 idata UpPoint1_Busy;   //Is the upload endpoint busy flag

void ftdi_init(void);

#endif
