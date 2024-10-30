
#ifndef	__DEBUG_H__
#define __DEBUG_H__

//Define the return value of the function
#ifndef  SUCCESS
#define  SUCCESS  1
#endif
#ifndef  FAIL
#define  FAIL    0
#endif

//define the timer start
#ifndef  START
#define  START  1
#endif
#ifndef  STOP
#define  STOP    0
#endif

#define	 FREQ_SYS	24000000ul	  //The main frequency of the system is 24MHz
#define  BAUD_RATE  125000ul

void CfgFsys(void);                   //CH559 clock selection and configuration
void mDelayuS(UINT16 n);              // Delay in uS
void mDelaymS(UINT16 n);              // Delay in mS

#endif

