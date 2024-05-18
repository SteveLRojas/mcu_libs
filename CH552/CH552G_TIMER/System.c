//#include "Type.h"
#include "CH552.H"
#include "System.h"

/******************************************************************************
* Function Name : CfgFsys( )
* Description: CH559 clock selection and configuration function, the internal crystal oscillator 12MHz is used by default, if FREQ_SYS is defined, it can be
                    According to the configuration of PLL_CFG and CLOCK_CFG, the formula is as follows:
                    Fsys = (Fosc * ( PLL_CFG & MASK_PLL_MULT ))/(CLOCK_CFG & MASK_SYS_CK_DIV);
                    The specific clock needs to be configured by yourself
* Input	:	None
* Output :	None
* Return :	None
*******************************************************************************/
void CfgFsys()	
{
	SAFE_MOD = 0x55;							//Enable safe mode
	SAFE_MOD = 0xAA;												 

	CLOCK_CFG &= ~MASK_SYS_CK_SEL;

	//Configure the system clock to 24MHz
	CLOCK_CFG |= 6; 															  
	
	SAFE_MOD = 0xFF;			//Turn off safe mode
// If you modify the main frequency, you must modify FREQ_SYS at the same time, otherwise the delay function will be inaccurate
}


/*******************************************************************************
* Function Name  : mDelayus(UNIT16 n)
* Description    : us delay function
* Input          : UNIT16 n
* Output         : None
* Return         : None
*******************************************************************************/ 
void mDelayuS( UINT16 n )  // Delay in uS
{
	while ( n ) {  // total = 12~13 Fsys cycles, 1uS @Fsys=12MHz
		++ SAFE_MOD;  // 2 Fsys cycles, for higher Fsys, add operation here
#ifdef	FREQ_SYS
#if		FREQ_SYS >= 14000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 16000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 18000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 20000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 22000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 24000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 26000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 28000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 30000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 32000000
		++ SAFE_MOD;
#endif
#endif
		-- n;
	}
}


/*******************************************************************************
* Function Name  : mDelayms(UNIT16 n)
* Description    : ms delay function
* Input          : UNIT16 n
* Output         : None
* Return         : None
*******************************************************************************/
void	mDelaymS( UINT16 n )                              // Delay in mS
{
	while ( n ) 
	{
		mDelayuS( 1000 );
		-- n;
	}
}

