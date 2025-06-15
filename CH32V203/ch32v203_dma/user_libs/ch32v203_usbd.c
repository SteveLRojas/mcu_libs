/*
 * ch32v203_usbd.c
 *
 *  Created on: Jul 23, 2024
 *      Author: Steve
 */
#include "ch32v20x.h"
#include "ch32v203_core.h"
#include "ch32v203_usbd.h"

void USB_HP_CAN1_TX_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USB_LP_CAN1_RX0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USBWakeUp_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void (*usbd_sof_callback)(void) = 0;
void (*usbd_esof_callback)(void) = 0;
void (*usbd_out_callback)(uint8_t) = 0;
void (*usbd_in_callback)(uint8_t) = 0;
void (*usbd_setup_callback)(uint8_t) = 0;
void (*usbd_reset_callback)(void) = 0;
void (*usbd_wakeup_callback)(void) = 0;
void (*usbd_suspend_callback)(void) = 0;
volatile uint16_t sof_count;

uint16_t ep_config[8];

void USB_HP_CAN1_TX_IRQHandler(void)
{
	uint8_t ep = (uint8_t)(USBD_COMMON->ISTR & USBD_EP_ID);

	if(USBD_ENDPOINT[ep].EPR & USBD_CTR_RX)
	{
		if(usbd_out_callback)
			usbd_out_callback(ep);
		USBD_ENDPOINT[ep].EPR &= ~(USBD_CTR_RX | USBD_DTOG_RX | USBD_STAT_RX | USBD_DTOG_TX | USBD_STAT_TX);
	}
	if(USBD_ENDPOINT[ep].EPR & USBD_CTR_TX)
	{
		if(usbd_in_callback)
			usbd_in_callback(ep);
		USBD_ENDPOINT[ep].EPR &= ~(USBD_CTR_TX | USBD_DTOG_RX | USBD_STAT_RX | USBD_DTOG_TX | USBD_STAT_TX);
	}

//	uint16_t interrupt_status;
//	uint8_t ep;
//
//	while((interrupt_status = USBD_COMMON->ISTR) & USBD_CTR)
//	{
//		ep = (uint8_t)(interrupt_status & USBD_EP_ID);
//		if(USBD_ENDPOINT[ep].EPR & USBD_CTR_RX)
//		{
//			if(usbd_out_callback)
//				usbd_out_callback(ep);
//			USBD_ENDPOINT[ep].EPR &= ~(USBD_CTR_RX | USBD_DTOG_RX | USBD_STAT_RX | USBD_DTOG_TX | USBD_STAT_TX);
//		}
//		if(USBD_ENDPOINT[ep].EPR & USBD_CTR_TX)
//		{
//			if(usbd_in_callback)
//				usbd_in_callback(ep);
//			USBD_ENDPOINT[ep].EPR &= ~(USBD_CTR_TX | USBD_DTOG_RX | USBD_STAT_RX | USBD_DTOG_TX | USBD_STAT_TX);
//		}
//	}
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
	uint16_t active_interrupts = USBD_COMMON->ISTR & USBD_COMMON->CNTR;

	if(active_interrupts & USBD_CTR)
	{
		uint8_t ep = (uint8_t)(USBD_COMMON->ISTR & USBD_EP_ID);
		//if(USBD_COMMON->ISTR & USBD_DIR)
		if(USBD_ENDPOINT[ep].EPR & USBD_CTR_RX)
		{
			if(USBD_ENDPOINT[ep].EPR & USBD_SETUP)
			{
				if(usbd_setup_callback)
					usbd_setup_callback(ep);
			}
			else
			{
				if(usbd_out_callback)
					usbd_out_callback(ep);
			}
			USBD_ENDPOINT[ep].EPR &= ~(USBD_CTR_RX | USBD_DTOG_RX | USBD_STAT_RX | USBD_DTOG_TX | USBD_STAT_TX);
		}
		//else
		if(USBD_ENDPOINT[ep].EPR & USBD_CTR_TX)
		{
			if(usbd_in_callback)
				usbd_in_callback(ep);
			USBD_ENDPOINT[ep].EPR &= ~(USBD_CTR_TX | USBD_DTOG_RX | USBD_STAT_RX | USBD_DTOG_TX | USBD_STAT_TX);
		}
	}
	if(active_interrupts & USBD_SUSP)
	{
		USBD_COMMON->CNTR |= USBD_FSUSP;

		if(usbd_suspend_callback)
			usbd_suspend_callback();

		USBD_COMMON->ISTR &= ~USBD_SUSP;
		USBD_COMMON->CNTR |= USBD_LPMODE;
	}
	if(active_interrupts & USBD_RESET)
	{
		for(uint16_t d = 0; d < 8; ++d)
		{
			USBD_ENDPOINT[d].EPR = ep_config[d];
		}
		USBD_COMMON->DADDR = USBD_EF;

		if(usbd_reset_callback)
			usbd_reset_callback();

		USBD_COMMON->ISTR = 0;
	}
	if(active_interrupts & USBD_SOF)
	{
		++sof_count;
		if(usbd_sof_callback)
			usbd_sof_callback();
		USBD_COMMON->ISTR &= ~USBD_SOF;
	}
	if(active_interrupts & USBD_ESOF)
	{
		if(usbd_esof_callback)
			usbd_esof_callback();
		USBD_COMMON->ISTR &= ~USBD_ESOF;
	}
	if(active_interrupts & USBD_WKUP)
	{
		USBD_COMMON->CNTR &= ~USBD_FSUSP;
		if(usbd_wakeup_callback)
			usbd_wakeup_callback();
		USBD_COMMON->ISTR &= ~USBD_WKUP;
	}

	USBD_COMMON->ISTR &= ~(USBD_PMAOVR | USBD_ERR);
}

void USBWakeUp_IRQHandler(void)
{
	USBD_COMMON->CNTR &= ~USBD_FSUSP;
	if(usbd_wakeup_callback)
		usbd_wakeup_callback();
	USBD_COMMON->ISTR &= ~USBD_WKUP;
}

//HINT: This function assumes that the clocks are already configured.
void usbd_init(const usbd_config_t* usbd_config)
{
	USBD_COMMON->CNTR = USBD_FRES;
	USBD_COMMON->BTABLE = 0x00;	//place btable at the start of the PMA

	uint16_t buf_offset = USBD_BTABLE_SIZE;	//The buffers are placed after the buffer description table
	uint16_t buf_size;	//size and offset must be even numbers
	for(uint16_t d = 0; d < 8; ++d)
	{
		if((usbd_config->ep_type[d] == USBD_EP_TYPE_BULK_DBL) || (usbd_config->ep_type[d] == USBD_EP_TYPE_ISO))
		{
			//Endpoint is double-buffered
			if(usbd_config->tx_buf_size[d] == 0)
			{
				//Double-buffered OUT
				buf_size = usbd_config->rx_buf_size[d] + 2;	//The USBD module needs 2 extra bytes because it writes the CRC
				if(buf_size < 63)
				{
					buf_size = (buf_size + 1) & 0xFFFE;	//round up to an even number
					USBD_BTABLE[d].COUNT_TX = buf_size << (10 - 1);	//block size in units of 2 at bits [14:10]
					USBD_BTABLE[d].COUNT_RX = buf_size << (10 - 1);
				}
				else
				{
					buf_size = (buf_size + 31) & 0xFFE0;	//round up to multiple of 32
					USBD_BTABLE[d].COUNT_TX = 0x8000 | ((buf_size - 32) << (10 - 5));	//block size - 1 in units of 32 at bits [14:10]
					USBD_BTABLE[d].COUNT_RX = 0x8000 | ((buf_size - 32) << (10 - 5));
				}
				USBD_BTABLE[d].ADDR_TX = buf_offset;
				buf_offset += buf_size;
				USBD_BTABLE[d].ADDR_RX = buf_offset;
				buf_offset += buf_size;
			}
			else
			{
				//Double-buffered IN
				buf_size = usbd_config->tx_buf_size[d];
				buf_size = (buf_size + 1) & 0xFFFE;	//round up to an even number
				USBD_BTABLE[d].COUNT_TX = 0;	//COUNT_TX is the number of bytes ready to send, not the buffer size
				USBD_BTABLE[d].COUNT_RX = 0;
				USBD_BTABLE[d].ADDR_TX = buf_offset;
				buf_offset += buf_size;
				USBD_BTABLE[d].ADDR_RX = buf_offset;
				buf_offset += buf_size;
			}
		}
		else
		{
			USBD_BTABLE[d].ADDR_TX = buf_offset;
			buf_size = usbd_config->tx_buf_size[d];
			buf_size = (buf_size + 1) & 0xFFFE;	//round up to an even number
			buf_offset += buf_size;
			USBD_BTABLE[d].COUNT_TX = 0;	//COUNT_TX is the number of bytes ready to send, not the buffer size

			USBD_BTABLE[d].ADDR_RX = buf_offset;
			buf_size = usbd_config->rx_buf_size[d] + 2;	//The USBD module needs 2 extra bytes because it writes the CRC
			if(buf_size < 63)
			{
				buf_size = (buf_size + 1) & 0xFFFE;	//round up to an even number
				USBD_BTABLE[d].COUNT_RX = buf_size << (10 - 1);	//block size in units of 2 at bits [14:10]
			}
			else
			{
				buf_size = (buf_size + 31) & 0xFFE0;	//round up to multiple of 32
				USBD_BTABLE[d].COUNT_RX = 0x8000 | ((buf_size - 32) << (10 - 5));	//block size - 1 in units of 32 at bits [14:10]
			}
			buf_offset += buf_size;
		}

		ep_config[d] = usbd_config->ep_type[d] | d;
	}

	USBD_COMMON->DADDR = USBD_EF;
	EXTEN->EXTEN_CTR = (EXTEN->EXTEN_CTR | EXTEN_USBD_PU_EN) & ~EXTEN_USBD_LS;	//pull-up enable, full speed
	USBD_COMMON->CNTR &= ~USBD_FRES;
	USBD_COMMON->ISTR = 0;
	USBD_COMMON->CNTR |= usbd_config->int_en;
	core_enable_irq(USB_HP_CAN1_TX_IRQn);
	core_enable_irq(USB_LP_CAN1_RX0_IRQn);
	core_enable_irq(USBWakeUp_IRQn);
}

void usbd_disable(void)
{
	USBD_COMMON->CNTR = USBD_FRES;
	USBD_COMMON->DADDR = 0;
	EXTEN->EXTEN_CTR &= ~EXTEN_USBD_PU_EN;
	USBD_COMMON->ISTR = 0;
	core_disable_irq(USB_HP_CAN1_TX_IRQn);
	core_disable_irq(USB_LP_CAN1_RX0_IRQn);
	core_disable_irq(USBWakeUp_IRQn);
}

void usbd_write_to_pma(uint16_t offset, const uint16_t* source, uint16_t num_words)
{
	uint16_t* pma = (uint16_t*)(USBD_PMA_BASE + (offset << 1));

	while(num_words--)
	{
		*pma = *source;
		pma += 2;
		++source;
	}
}

void usbd_read_from_pma(uint16_t offset, uint16_t* dest, uint16_t num_words)
{
	uint16_t* pma = (uint16_t*)(USBD_PMA_BASE + (offset << 1));

	while(num_words--)
	{
		*dest = *pma;
		pma += 2;
		++dest;
	}
}

void usbd_init_pma_val(uint16_t offset, uint16_t val, uint16_t num_words)
{
	uint16_t* pma = (uint16_t*)(USBD_PMA_BASE + (offset << 1));

	while(num_words--)
	{
		*pma = val;
		pma += 2;
	}
}
