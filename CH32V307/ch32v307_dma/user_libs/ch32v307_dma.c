/*
 * ch32v307_dma.c
 *
 *  Created on: Aug 23, 2025
 *      Author: Steve
 */

#include "ch32v30x.h"
#include "ch32v307_core.h"
#include "ch32v307_dma.h"

#if DMA1_IMPLEMENT_ISR
void DMA1_Channel1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel5_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel6_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel7_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel8_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void (*dma1_channel1_callback)(void) = 0;
void (*dma1_channel2_callback)(void) = 0;
void (*dma1_channel3_callback)(void) = 0;
void (*dma1_channel4_callback)(void) = 0;
void (*dma1_channel5_callback)(void) = 0;
void (*dma1_channel6_callback)(void) = 0;
void (*dma1_channel7_callback)(void) = 0;
void (*dma1_channel8_callback)(void) = 0;

void DMA1_Channel1_IRQHandler(void)
{
	if(dma1_channel1_callback)
		dma1_channel1_callback();
	DMA1->INTFCR = 0x0000000F;
}

void DMA1_Channel2_IRQHandler(void)
{
	if(dma1_channel2_callback)
		dma1_channel2_callback();
	DMA1->INTFCR = 0x000000F0;
}

void DMA1_Channel3_IRQHandler(void)
{
	if(dma1_channel3_callback)
		dma1_channel3_callback();
	DMA1->INTFCR = 0x00000F00;
}

void DMA1_Channel4_IRQHandler(void)
{
	if(dma1_channel4_callback)
		dma1_channel4_callback();
	DMA1->INTFCR = 0x0000F000;
}

void DMA1_Channel5_IRQHandler(void)
{
	if(dma1_channel5_callback)
		dma1_channel5_callback();
	DMA1->INTFCR = 0x000F0000;
}

void DMA1_Channel6_IRQHandler(void)
{
	if(dma1_channel6_callback)
		dma1_channel6_callback();
	DMA1->INTFCR = 0x00F00000;
}

void DMA1_Channel7_IRQHandler(void)
{
	if(dma1_channel7_callback)
		dma1_channel7_callback();
	DMA1->INTFCR = 0x0F000000;
}

void DMA1_Channel8_IRQHandler(void)
{
	if(dma1_channel8_callback)
		dma1_channel8_callback();
	DMA1->INTFCR = 0xF0000000;
}
#endif

#if DMA2_IMPLEMENT_ISR
void DMA2_Channel1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel5_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel6_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel7_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel8_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel9_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel10_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel11_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void (*dma2_channel1_callback)(void) = 0;
void (*dma2_channel2_callback)(void) = 0;
void (*dma2_channel3_callback)(void) = 0;
void (*dma2_channel4_callback)(void) = 0;
void (*dma2_channel5_callback)(void) = 0;
void (*dma2_channel6_callback)(void) = 0;
void (*dma2_channel7_callback)(void) = 0;
void (*dma2_channel8_callback)(void) = 0;
void (*dma2_channel9_callback)(void) = 0;
void (*dma2_channel10_callback)(void) = 0;
void (*dma2_channel11_callback)(void) = 0;

void DMA2_Channel1_IRQHandler(void)
{
	if(dma2_channel1_callback)
		dma2_channel1_callback();
	DMA2->INTFCR = 0x0000000F;
}

void DMA2_Channel2_IRQHandler(void)
{
	if(dma2_channel2_callback)
		dma2_channel2_callback();
	DMA2->INTFCR = 0x000000F0;
}

void DMA2_Channel3_IRQHandler(void)
{
	if(dma2_channel3_callback)
		dma2_channel3_callback();
	DMA2->INTFCR = 0x00000F00;
}

void DMA2_Channel4_IRQHandler(void)
{
	if(dma2_channel4_callback)
		dma2_channel4_callback();
	DMA2->INTFCR = 0x0000F000;
}

void DMA2_Channel5_IRQHandler(void)
{
	if(dma2_channel5_callback)
		dma2_channel5_callback();
	DMA2->INTFCR = 0x000F0000;
}

void DMA2_Channel6_IRQHandler(void)
{
	if(dma2_channel6_callback)
		dma2_channel6_callback();
	DMA2->INTFCR = 0x00F00000;
}

void DMA2_Channel7_IRQHandler(void)
{
	if(dma2_channel7_callback)
		dma2_channel7_callback();
	DMA2->INTFCR = 0x0F000000;
}

void DMA2_Channel8_IRQHandler(void)
{
	if(dma2_channel8_callback)
		dma2_channel8_callback();
	DMA2_EXTEN->INTFCR = 0x0000000F;
}

void DMA2_Channel9_IRQHandler(void)
{
	if(dma2_channel9_callback)
		dma2_channel9_callback();
	DMA2_EXTEN->INTFCR = 0x000000F0;
}

void DMA2_Channel10_IRQHandler(void)
{
	if(dma2_channel10_callback)
		dma2_channel10_callback();
	DMA2_EXTEN->INTFCR = 0x00000F00;
}

void DMA2_Channel11_IRQHandler(void)
{
	if(dma2_channel11_callback)
		dma2_channel11_callback();
	DMA2_EXTEN->INTFCR = 0x0000F000;
}
#endif

//HINT: initializes both DMA controllers
void dma_init(void)
{
	DMA1->INTFCR = 0xFFFFFFFF;
	core_enable_irq(DMA1_Channel1_IRQn);
	core_enable_irq(DMA1_Channel2_IRQn);
	core_enable_irq(DMA1_Channel3_IRQn);
	core_enable_irq(DMA1_Channel4_IRQn);
	core_enable_irq(DMA1_Channel5_IRQn);
	core_enable_irq(DMA1_Channel6_IRQn);
	core_enable_irq(DMA1_Channel7_IRQn);

	DMA2->INTFCR = 0xFFFFFFFF;
	core_enable_irq(DMA2_Channel1_IRQn);
	core_enable_irq(DMA2_Channel2_IRQn);
	core_enable_irq(DMA2_Channel3_IRQn);
	core_enable_irq(DMA2_Channel4_IRQn);
	core_enable_irq(DMA2_Channel5_IRQn);
	core_enable_irq(DMA2_Channel6_IRQn);
	core_enable_irq(DMA2_Channel7_IRQn);
	core_enable_irq(DMA2_Channel8_IRQn);
	core_enable_irq(DMA2_Channel9_IRQn);
	core_enable_irq(DMA2_Channel10_IRQn);
	core_enable_irq(DMA2_Channel11_IRQn);
}

void dma_transfer(DMA_Channel_TypeDef* channel, uint32_t mem_addr, uint32_t periph_addr, uint16_t num_words)
{
	channel->CFGR &= ~DMA_CFG_ENABLE;
	channel->MADDR = mem_addr;
	channel->PADDR = periph_addr;
	channel->CNTR = num_words;
	channel->CFGR |= DMA_CFG_ENABLE;
}
