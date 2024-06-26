/********************************** (C) COPYRIGHT *******************************
* File Name          : DEBUG.H
* Author             : WCH
* Version            : V1.9
* Date               : 2021/12/15
* Description        : CH559 DEBUG Interface         				   
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/


#ifndef	__DEBUG_H__
#define __DEBUG_H__

//定义函数返回值
#ifndef  SUCCESS
#define  SUCCESS  1
#endif
#ifndef  FAIL
#define  FAIL    0
#endif

//定义定时器起始
#ifndef  START
#define  START  1
#endif
#ifndef  STOP
#define  STOP    0
#endif

#define	 FREQ_SYS	48000000ul
#define  BAUD_RATE  125000ul

#ifndef MIN
#define MIN(a,b)                (((a) <= (b)) ? (a) : (b))
#endif

void	CfgFsys( );                        //CH559时钟选择和配置
void	mDelayuS( UINT16 n );              // 以uS为单位延时
void	mDelaymS( UINT16 n );              // 以mS为单位延时
void	CH559UART0Alter();                 //CH559串口0引脚映射到P0.2/P0.3
void	mInitSTDIO( );                      //T1作为波特率发生器
UINT8	CH559UART0RcvByte( );              //CH559 UART0查询方式接收一个字节
void	CH559UART0SendByte(UINT8 SendDat); //CH559UART0发送一个字节
#endif
