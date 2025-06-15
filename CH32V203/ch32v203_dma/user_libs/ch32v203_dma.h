/*
 * ch32v203_dma.h
 *
 *  Created on: Jun 14, 2025
 *      Author: Steve
 */

#ifndef _CH32V203_DMA_H_
#define _CH32V203_DMA_H_

#define DMA_IMPLEMENT_ISR 1

#if DMA_IMPLEMENT_ISR
extern void (*dma1_channel1_callback)(void);
extern void (*dma1_channel2_callback)(void);
extern void (*dma1_channel3_callback)(void);
extern void (*dma1_channel4_callback)(void);
extern void (*dma1_channel5_callback)(void);
extern void (*dma1_channel6_callback)(void);
extern void (*dma1_channel7_callback)(void);
extern void (*dma1_channel8_callback)(void);
#endif

// HINT: Refer to the DMAy_CFGRx register description in the CH32V203 reference manual.
#define DMA_CFG_MEM2MEM_OFF		0x0000
#define DMA_CFG_MEM2MEM_ON		0x4000
#define DMA_CFG_PRI_LOW			0x0000
#define DMA_CFG_PRI_MEDIUM		0x1000
#define DMA_CFG_PRI_HIGH		0x2000
#define DMA_CFG_PRI_MAX			0x3000
#define DMA_CFG_MSIZE_8			0x0000
#define DMA_CFG_MSIZE_16		0x0400
#define DMA_CFG_MSIZE_32		0x0800
#define DMA_CFG_PSIZE_8			0x0000
#define DMA_CFG_PSIZE_16		0x0100
#define DMA_CFG_PSIZE_32		0x0200
#define DMA_CFG_MINC_OFF		0x0000
#define DMA_CFG_MINC_ON			0x0080
#define DMA_CFG_PINC_OFF		0x0000
#define DMA_CFG_PINC_ON			0x0040
#define DMA_CFG_CIRC_OFF		0x0000
#define DMA_CFG_CIRC_ON			0x0020
#define DMA_CFG_PERIPH_TO_MEM	0x0000
#define DMA_CFG_MEM_TO_PERIPH	0x0010
#define DMA_CFG_TEIE_OFF		0x0000
#define DMA_CFG_TEIE_ON			0x0008
#define DMA_CFG_HTIE_OFF		0x0000
#define DMA_CFG_HTIE_ON			0x0004
#define DMA_CFG_TCIE_OFF		0x0000
#define DMA_CFG_TCIE_ON			0x0002
#define DMA_CFG_DISABLE			0x0000
#define DMA_CFG_ENABLE			0x0001

#define dma_channel_config(channel, options) ((channel)->CFGR = (options))
#define dma_get_pending_transfers(channel) ((channel)->CNTR)

void dma_init(void);
void dma_transfer(DMA_Channel_TypeDef* channel, uint32_t mem_addr, uint32_t periph_addr, uint16_t num_words);

#endif /* _CH32V203_DMA_H_ */
