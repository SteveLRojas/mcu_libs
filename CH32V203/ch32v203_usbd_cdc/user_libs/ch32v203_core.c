/*
 * ch32v203_core.c
 *
 *  Created on: Jul 15, 2024
 *      Author: Steve
 */
#include <stdint.h>
#include "ch32v203_core.h"

//HINT: Return the Machine Status Register
uint32_t core_get_mstatus(void)
{
  uint32_t result;

  __asm volatile ( "csrr %0," "mstatus" : "=r" (result) );
  return (result);
}

//HINT: set mstatus value
void core_set_mstatus(uint32_t value)
{
  __asm volatile ("csrw mstatus, %0" : : "r" (value) );
}

//HINT: Return the Machine ISA Register
uint32_t core_get_misa(void)
{
  uint32_t result;

  __asm volatile ( "csrr %0," "misa" : "=r" (result) );
  return (result);
}

//HINT: Set the Machine ISA Register
void core_set_misa(uint32_t value)
{
  __asm volatile ("csrw misa, %0" : : "r" (value) );
}

//HINT: Return the Machine Trap-Vector Base-Address Register
uint32_t core_get_mtvec(void)
{
  uint32_t result;

  __asm volatile ( "csrr %0," "mtvec" : "=r" (result) );
  return (result);
}

//HINT: Set the Machine Trap-Vector Base-Address Register
void core_set_mtvec(uint32_t value)
{
	__asm volatile ("csrw mtvec, %0" : : "r" (value) );
}

//HINT: Return the Machine Scratch Register
uint32_t core_get_mscratch(void)
{
  uint32_t result;

  __asm volatile ( "csrr %0," "mscratch" : "=r" (result) );
  return (result);
}

//HINT: Set the Machine Scratch Register
void core_set_mscratch(uint32_t value)
{
	__asm volatile ("csrw mscratch, %0" : : "r" (value) );
}

//HINT: Return the Machine Exception Program Register
uint32_t core_get_mepc(void)
{
  uint32_t result;

  __asm volatile ( "csrr %0," "mepc" : "=r" (result) );
  return (result);
}

//HINT: Set the Machine Exception Program Register
void core_set_mepc(uint32_t value)
{
	__asm volatile ("csrw mepc, %0" : : "r" (value) );
}

//HINT: Return the Machine Cause Register
uint32_t core_get_mcause(void)
{
  uint32_t result;

  __asm volatile ( "csrr %0," "mcause" : "=r" (result) );
  return (result);
}

//HINT: Set the Machine Cause Register
void core_set_mcause(uint32_t value)
{
	__asm volatile ("csrw mcause, %0" : : "r" (value) );
}

//HINT: Return the Machine Trap Value Register
uint32_t core_get_mtval(void)
{
  uint32_t result;

  __asm volatile ( "csrr %0," "mtval" : "=r" (result) );
  return (result);
}

//HINT: Set the Machine Trap Value Register
void core_set_mtval(uint32_t value)
{
	__asm volatile ("csrw mtval, %0" : : "r" (value) );
}

//HINT: Return Vendor ID Register
uint32_t core_get_mvendorid(void)
{
  uint32_t result;

  __asm volatile ( "csrr %0," "mvendorid" : "=r" (result) );
  return (result);
}

//HINT: Return Machine Architecture ID Register
uint32_t core_get_marchid(void)
{
  uint32_t result;

  __asm volatile ( "csrr %0," "marchid" : "=r" (result) );
  return (result);
}

//HINT: Return Machine Implementation ID Register
uint32_t core_get_mimpid(void)
{
  uint32_t result;

  __asm volatile ( "csrr %0," "mimpid" : "=r" (result) );
  return (result);
}

//HINT: Return Hart ID Register
uint32_t core_get_mhartid(void)
{
  uint32_t result;

  __asm volatile ( "csrr %0," "mhartid" : "=r" (result) );
  return (result);
}

//HINT: Return SP Register
uint32_t core_get_sp(void)
{
  uint32_t result;

  __asm volatile ( "mv %0," "sp" : "=r"(result) : );
  return (result);
}
