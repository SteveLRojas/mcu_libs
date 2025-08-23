/*
 * stm32f205_core.c
 *
 *  Created on: Aug 17, 2025
 *      Author: Steve
 */
#include "stm32f205xx.h"
#include "stm32f205_core.h"
#include "stm32f205_rcc.h"

static uint8_t  p_us = 0;
static uint16_t p_ms = 0;

/*********************************************************************
 * @fn      Delay_Init
 *
 * @brief   Initializes Delay Funcation.
 *
 * @return  none
 */
void core_delay_init(void)
{
	core_systick_clk_src_config(SysTick_CLKSource_HCLK_Div8);
	//p_us = rcc_compute_hclk_freq() / 8000000;	//TODO: put this back
	p_us = 96000000UL / 8000000;
	p_ms = (uint16_t)p_us * 1000;
}

void core_delay_us(uint32_t n)
{
	uint32_t i;

	SysTick->LOAD = n * p_us;
	SysTick->VAL = 0x00;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

	do
	{
		i = SysTick->CTRL;
	} while((i & 0x01) && !(i & (1 << 16)));

	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	SysTick->VAL = 0x00;
}

void core_delay_ms(uint16_t n)
{
	uint32_t i;

	SysTick->LOAD = (uint32_t)n * p_ms;
	SysTick->VAL = 0x00;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

	do
	{
		i = SysTick->CTRL;
	} while((i & 0x01) && !(i & (1 << 16)));

	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	SysTick->VAL = 0x00;
}


//HINT: Return the Process Stack Pointer
uint32_t core_get_psp(void) __attribute__( ( naked ) );
uint32_t core_get_psp(void)
{
  uint32_t result=0;

  __asm volatile ("MRS %0, psp\n\t"
                  "MOV r0, %0 \n\t"
                  "BX  lr     \n\t"  : "=r" (result) );
  return(result);
}

//HINT: Assign the value topOfProcStack to the PSP (process stack pointer) Cortex processor register
void core_set_psp(uint32_t topOfProcStack) __attribute__( ( naked ) );
void core_set_psp(uint32_t topOfProcStack)
{
  __asm volatile ("MSR psp, %0\n\t"
                  "BX  lr     \n\t" : : "r" (topOfProcStack) );
}

//HINT: Return the current value of the MSP (main stack pointer) Cortex processor register
uint32_t core_get_msp(void) __attribute__( ( naked ) );
uint32_t core_get_msp(void)
{
  uint32_t result=0;

  __asm volatile ("MRS %0, msp\n\t"
                  "MOV r0, %0 \n\t"
                  "BX  lr     \n\t"  : "=r" (result) );
  return(result);
}

//HINT: Assign the value mainStackPointer to the MSP (main stack pointer) Cortex processor register
void core_set_msp(uint32_t topOfMainStack) __attribute__( ( naked ) );
void core_set_msp(uint32_t topOfMainStack)
{
  __asm volatile ("MSR msp, %0\n\t"
                  "BX  lr     \n\t" : : "r" (topOfMainStack) );
}

//HINT: Return the content of the base priority register
uint32_t core_get_basepri(void)
{
  uint32_t result=0;

  __asm volatile ("MRS %0, basepri_max" : "=r" (result) );
  return(result);
}

//HINT: Set the base priority register
void core_set_basepri(uint32_t value)
{
  __asm volatile ("MSR basepri, %0" : : "r" (value) );
}

//HINT: Return state of the priority mask bit from the priority mask register
uint32_t core_get_primask(void)
{
  uint32_t result=0;

  __asm volatile ("MRS %0, primask" : "=r" (result) );
  return(result);
}

//HINT: Set the priority mask bit in the priority mask register
void core_set_primask(uint32_t priMask)
{
  __asm volatile ("MSR primask, %0" : : "r" (priMask) );
}

//HINT: Return the content of the fault mask register
uint32_t core_get_faultmask(void)
{
  uint32_t result=0;

  __asm volatile ("MRS %0, faultmask" : "=r" (result) );
  return(result);
}

//HINT: Set the fault mask register
void core_set_faultmask(uint32_t faultMask)
{
  __asm volatile ("MSR faultmask, %0" : : "r" (faultMask) );
}

//HINT: Return the content of the control register
uint32_t core_get_control(void)
{
  uint32_t result=0;

  __asm volatile ("MRS %0, control" : "=r" (result) );
  return(result);
}

//HINT: Set the control register
void core_set_control(uint32_t control)
{
  __asm volatile ("MSR control, %0" : : "r" (control) );
}

//HINT: Reverse byte order in integer value
uint32_t core_rev(uint32_t value)
{
  uint32_t result=0;

  __asm volatile ("rev %0, %1" : "=r" (result) : "r" (value) );
  return(result);
}

//HINT: Reverse byte order in unsigned short value
uint32_t core_rev16(uint16_t value)
{
  uint32_t result=0;

  __asm volatile ("rev16 %0, %1" : "=r" (result) : "r" (value) );
  return(result);
}

//HINT: Reverse byte order in signed short value with sign extension to integer
int32_t core_revsh(int16_t value)
{
  uint32_t result=0;

  __asm volatile ("revsh %0, %1" : "=r" (result) : "r" (value) );
  return(result);
}

//HINT: Reverse bit order of value
uint32_t core_rbit(uint32_t value)
{
  uint32_t result=0;

   __asm volatile ("rbit %0, %1" : "=r" (result) : "r" (value) );
   return(result);
}

//HINT: Exclusive LDR command for 8 bit value
uint8_t core_ldrexb(uint8_t *addr)
{
    uint8_t result=0;

   __asm volatile ("ldrexb %0, [%1]" : "=r" (result) : "r" (addr) );
   return(result);
}

//HINT: Exclusive LDR command for 16 bit values
uint16_t core_ldrexh(uint16_t *addr)
{
    uint16_t result=0;

   __asm volatile ("ldrexh %0, [%1]" : "=r" (result) : "r" (addr) );
   return(result);
}

//HINT: Exclusive LDR command for 32 bit values
uint32_t core_ldrexw(uint32_t *addr)
{
    uint32_t result=0;

   __asm volatile ("ldrex %0, [%1]" : "=r" (result) : "r" (addr) );
   return(result);
}

//HINT: Exclusive STR command for 8 bit values
uint32_t core_strexb(uint8_t value, uint8_t *addr)
{
   uint32_t result=0;

   __asm volatile ("strexb %0, %2, [%1]" : "=&r" (result) : "r" (addr), "r" (value) );
   return(result);
}

//HINT: Exclusive STR command for 16 bit values
uint32_t core_strexh(uint16_t value, uint16_t *addr)
{
   uint32_t result=0;

   __asm volatile ("strexh %0, %2, [%1]" : "=&r" (result) : "r" (addr), "r" (value) );
   return(result);
}

//HINT: Exclusive STR command for 32 bit values
uint32_t core_strexw(uint32_t value, uint32_t *addr)
{
   uint32_t result=0;

   __asm volatile ("strex %0, %2, [%1]" : "=r" (result) : "r" (addr), "r" (value) );
   return(result);
}
