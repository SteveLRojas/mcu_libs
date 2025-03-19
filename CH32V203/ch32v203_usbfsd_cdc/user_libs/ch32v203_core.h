/*
 * ch32v203_core.h
 *
 *  Created on: Jul 15, 2024
 *      Author: Steve
 */

#ifndef _LIBS_CH32V203_CORE_H_
#define _LIBS_CH32V203_CORE_H_

#ifndef __IO
#define __I     volatile const          /*  defines 'read only' permissions     */
#define __O     volatile                /*  defines 'write only' permissions    */
#define __IO    volatile                /*  defines 'read / write' permissions  */
#endif

/* memory mapped structure for Program Fast Interrupt Controller (PFIC) */
typedef struct{
  __I  uint32_t ISR[8];
  __I  uint32_t IPR[8];
  __IO uint32_t ITHRESDR;
  __IO uint32_t RESERVED;
  __IO uint32_t CFGR;
  __I  uint32_t GISR;
  __IO uint8_t VTFIDR[4];
  uint8_t RESERVED0[12];
  __IO uint32_t VTFADDR[4];
  uint8_t RESERVED1[0x90];
  __O  uint32_t IENR[8];
  uint8_t RESERVED2[0x60];
  __O  uint32_t IRER[8];
  uint8_t RESERVED3[0x60];
  __O  uint32_t IPSR[8];
  uint8_t RESERVED4[0x60];
  __O  uint32_t IPRR[8];
  uint8_t RESERVED5[0x60];
  __IO uint32_t IACTR[8];
  uint8_t RESERVED6[0xE0];
  __IO uint8_t IPRIOR[256];
  uint8_t RESERVED7[0x810];
  __IO uint32_t SCTLR;
}PFIC_Type;

/* memory mapped structure for SysTick */
typedef struct
{
    __IO uint32_t CTLR;
    __IO uint32_t SR;
    __IO uint64_t CNT;
    __IO uint64_t CMP;
}SysTick_Type;

#endif /* _CH32V203_CORE_H_ */

#define PFIC            ((PFIC_Type *) 0xE000E000 )
#define SysTick         ((SysTick_Type *) 0xE000F000)

#define PFIC_KEY1       ((uint32_t)0xFA050000)
#define	PFIC_KEY2		((uint32_t)0xBCAF0000)
#define	PFIC_KEY3		((uint32_t)0xBEEF0000)

#define core_enable_irq(irq) (PFIC->IENR[(uint32_t)(irq) >> 5] = 1 << ((uint32_t)(irq) & 0x1F))
#define core_disable_irq(irq) (PFIC->IRER[(uint32_t)(irq) >> 5] = 1 << ((uint32_t)(irq) & 0x1F))
#define core_get_irq_status(irq) ((PFIC->ISR[(uint32_t)(irq) >> 5] & (1 << ((uint32_t)(irq) & 0x1F))) ? 1 : 0)
#define core_get_irq_pending(irq) ((PFIC->IPR[(uint32_t)(irq) >> 5] & (1 << ((uint32_t)(irq) & 0x1F))) ? 1 : 0)
#define core_set_irq_pending(irq) (PFIC->IPSR[(uint32_t)(irq) >> 5] = 1 << ((uint32_t)(irq) & 0x1F))
#define core_clear_pending_irq(irq) (PFIC->IPRR[(uint32_t)(irq) >> 5] = 1 << ((uint32_t)(irq) & 0x1F))
#define core_get_irq_active(irq) ((PFIC->IACTR[(uint32_t)(irq) >> 5] & (1 << (uint32_t)(irq) & 0x1F)) ? 1 : 0)
#define core_set_irq_priority(irq, priority) (PFIC->IPRIOR[(uint32_t)(irq)] = (priority))
#define core_set_event() (PFIC->SCTLR |= (1 << 5))
#define core_set_vtf_irq_num(vtf_idx, irq) (PFIC->VTFIDR[(vtf_idx)] = (UINT32_t)(irq))
#define core_set_vtf_irq_addr(vtf_idx, addr) (PFIC-VTFADDR[(vtf_idx)] = (uint32_t)(addr))
#define core_vtf_irq_enable(vtf_idx) (PFIC->VTFADDR[(vtf_idx)] |= 0x01)
#define core_vtf_irq_disable(vtf_idx) (PFIC->VTFADDR[(vtf_idx)] &= 0xFFFFFFFE)

uint32_t core_get_mstatus(void);
void core_set_mstatus(uint32_t value);
uint32_t core_get_misa(void);
void core_set_misa(uint32_t value);
uint32_t core_get_mtvec(void);
void core_set_mtvec(uint32_t value);
uint32_t core_get_mscratch(void);
void core_set_mscratch(uint32_t value);
uint32_t core_get_mepc(void);
void core_set_mepc(uint32_t value);
uint32_t core_get_mcause(void);
void core_set_mcause(uint32_t value);
uint32_t core_get_mtval(void);
void core_set_mtval(uint32_t value);
uint32_t core_get_mvendorid(void);
uint32_t core_get_marchid(void);
uint32_t core_get_mimpid(void);
uint32_t core_get_mhartid(void);
uint32_t core_get_sp(void);

//HINT: Enable Global Interrupt
__attribute__( ( always_inline ) ) static inline void core_enable_global_irq()
{
  __asm volatile ("csrs 0x800, %0" : : "r" (0x88) );
}

//HINT: Disable Global Interrupt
__attribute__( ( always_inline ) ) static inline void core_disable_global_irq()
{
  __asm volatile ("csrc 0x800, %0" : : "r" (0x88) );
}

__attribute__( ( always_inline ) ) static inline void core_nop()
{
  __asm volatile ("nop");
}

__attribute__( ( always_inline ) ) static inline void core_wait_for_interrupt(void)
{
  PFIC->SCTLR &= ~(1<<3);	// wfi
  asm volatile ("wfi");
}

__attribute__( ( always_inline ) ) static inline void core_wait_for_event(void)
{
    uint32_t tmp= PFIC->SCTLR;
    tmp &= ~(1 << 5);
    tmp |= (1 << 3);
    PFIC->SCTLR = tmp;
    asm volatile ("wfi");
}

__attribute__( ( always_inline ) ) static inline void core_system_reset(void)
{
  PFIC->CFGR = PFIC_KEY3|(1<<7);
}

//HINT: Atomically ADD 32bit value with value in memory using amoadd.w.
__attribute__( ( always_inline ) ) static inline int32_t core_amoadd(volatile int32_t* addr, int32_t value)
{
    int32_t result;

    __asm volatile ("amoadd.w %0, %2, %1" : \
            "=r"(result), "+A"(*addr) : "r"(value) : "memory");
    return *addr;
}

//HINT: Atomically AND 32bit value with value in memory using amoand.w.
__attribute__( ( always_inline ) ) static inline int32_t core_amoand(volatile int32_t* addr, int32_t value)
{
    int32_t result;

    __asm volatile ("amoand.w %0, %2, %1" : \
            "=r"(result), "+A"(*addr) : "r"(value) : "memory");
    return *addr;
}

//HINT: Atomically signed max compare 32bit value with value in memory using amomax.w.
__attribute__( ( always_inline ) ) static inline int32_t core_amomax(volatile int32_t* addr, int32_t value)
{
    int32_t result;

    __asm volatile ("amomax.w %0, %2, %1" : \
            "=r"(result), "+A"(*addr) : "r"(value) : "memory");
    return *addr;
}

//HINT: Atomically unsigned max compare 32bit value with value in memory using amomaxu.w.
__attribute__( ( always_inline ) ) static inline uint32_t core_amomaxu(volatile uint32_t* addr, uint32_t value)
{
    uint32_t result;

    __asm volatile ("amomaxu.w %0, %2, %1" : \
            "=r"(result), "+A"(*addr) : "r"(value) : "memory");
    return *addr;
}

//HINT: Atomically signed min compare 32bit value with value in memory using amomin.w.
__attribute__( ( always_inline ) ) static inline int32_t core_amomin(volatile int32_t* addr, int32_t value)
{
    int32_t result;

    __asm volatile ("amomin.w %0, %2, %1" : \
            "=r"(result), "+A"(*addr) : "r"(value) : "memory");
    return *addr;
}

//HINT: Atomically unsigned min compare 32bit value with value in memory using amominu.w.
__attribute__( ( always_inline ) ) static inline uint32_t core_amominu(volatile uint32_t* addr, uint32_t value)
{
    uint32_t result;

    __asm volatile ("amominu.w %0, %2, %1" : \
            "=r"(result), "+A"(*addr) : "r"(value) : "memory");
    return *addr;
}

//HINT: Atomically OR 32bit value with value in memory using amoor.w.
__attribute__( ( always_inline ) ) static inline int32_t core_amoor(volatile int32_t* addr, int32_t value)
{
    int32_t result;

    __asm volatile ("amoor.w %0, %2, %1" : \
            "=r"(result), "+A"(*addr) : "r"(value) : "memory");
    return *addr;
}


//HINT: Atomically swap new 32bit value into memory using amoswap.w.
__attribute__( ( always_inline ) ) static inline uint32_t core_amoswap(volatile uint32_t* addr, uint32_t newval)
{
    uint32_t result;

    __asm volatile ("amoswap.w %0, %2, %1" : \
            "=r"(result), "+A"(*addr) : "r"(newval) : "memory");
    return result;
}

//HINT: Atomically XOR 32bit value with value in memory using amoxor.w.
__attribute__( ( always_inline ) ) static inline int32_t core_amoxor(volatile int32_t* addr, int32_t value)
{
    int32_t result;

    __asm volatile ("amoxor.w %0, %2, %1" : \
            "=r"(result), "+A"(*addr) : "r"(value) : "memory");
    return *addr;
}
