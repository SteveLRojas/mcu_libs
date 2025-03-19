/*
 * ch32v203_usbfsh.c
 *
 *  Created on: Aug 29, 2024
 *      Author: Steve
 */
#include "ch32v20x.h"
#include "ch32v203_core.h"
#include "ch32v203_usbfsh.h"

uint8_t usbfsh_rx_buf[64] __attribute__ ((aligned(4)));
uint8_t usbfsh_tx_buf[64] __attribute__ ((aligned(4)));
uint8_t usbfsh_ep_pid;
uint16_t usbfsh_in_transfer_nak_limit;
uint16_t usbfsh_out_transfer_nak_limit;

void usbfsh_init(void)
{
	core_disable_irq(USBFS_IRQn);
	core_disable_irq(USBFSWakeUp_IRQn);
	USBOTG_H_FS->USB_CTRL = USBFS_UC_HOST_MODE;
	USBOTG_H_FS->USB_DEV_AD &= ~USBFS_USB_ADDR_MASK;
	USBOTG_H_FS->UH_RX_DMA = (uint32_t)usbfsh_rx_buf;
	USBOTG_H_FS->UH_TX_DMA = (uint32_t)usbfsh_tx_buf;
	USBOTG_H_FS->UH_EP_MOD = USBFS_UH_EP_TX_EN | USBFS_UH_EP_RX_EN;
	USBOTG_H_FS->UH_RX_CTRL = 0x00;
	USBOTG_H_FS->UH_TX_CTRL = 0x00;
	USBOTG_H_FS->USB_CTRL = USBFS_UC_HOST_MODE | USBFS_UC_INT_BUSY | USBFS_UC_DMA_EN;
	USBOTG_H_FS->UHOST_CTRL = 0x00;

	USBOTG_H_FS->UH_SETUP = USBFS_UH_SOF_EN;
	USBOTG_H_FS->UH_EP_PID = 0x00;
	USBOTG_H_FS->UH_TX_LEN = 0x00;
	USBOTG_H_FS->USB_INT_EN = 0x00;
	USBOTG_H_FS->USB_INT_FG = 0xFF;
}

//HINT: copy control request to tx_buf before calling this function
// For control write transfers check U_TOG_OK, for control read transfers check the return value.
uint8_t usbfsh_control_transfer(usbfsh_ep_info_t* ep_info, uint8_t* pbuf)
{
	uint8_t response;
	uint16_t rem_len;
	uint8_t bytes_to_copy;

	// setup stage
	USBOTG_H_FS->UH_TX_CTRL = 0x00;
	USBOTG_H_FS->UH_TX_LEN = 0x08;
	usbfsh_ep_pid = (USB_PID_SETUP << 4) | ep_info->ep_num;
	response = usbfsh_transact(0xFFFF);
	if(response != USB_PID_ACK)
		return response;

	// data stage
	USBOTG_H_FS->UH_RX_CTRL = USBFS_UEP_R_TOG | USBFS_UEP_R_AUTO_TOG;
	USBOTG_H_FS->UH_TX_CTRL = USBFS_UEP_T_TOG | USBFS_UEP_T_AUTO_TOG;
	rem_len = usbfsh_setup_req->wLength;
	if(rem_len && pbuf)
	{
		if(usbfsh_setup_req->bRequestType & USB_REQ_TYP_IN)
		{
			usbfsh_ep_pid = (USB_PID_IN << 4) | ep_info->ep_num;
			do
			{
				response = usbfsh_transact(0xFFFF);
				// U_TOG_OK is not cleared if no response is received?
				if(!(USBOTG_H_FS->USB_INT_FG & USBFS_U_TOG_OK) || !response)
					return response;

				bytes_to_copy = (uint8_t)USBOTG_H_FS->USB_RX_LEN;
				if(bytes_to_copy > rem_len)
					bytes_to_copy = rem_len;
				rem_len -= bytes_to_copy;

				for(uint8_t idx = 0; idx < bytes_to_copy; ++idx)
				{
					*pbuf = usbfsh_rx_buf[idx];
					++pbuf;
				}

				if((uint8_t)USBOTG_H_FS->USB_RX_LEN != ep_info->max_packet_size)	//handle short or zero length packet
					break;
			} while(rem_len);

			// status stage (OUT)
			USBOTG_H_FS->UH_TX_LEN = 0x00;
			usbfsh_ep_pid = (USB_PID_OUT << 4) | ep_info->ep_num;
			response = usbfsh_transact(0xFFFF);
			ep_info->rx_tog_res = USBOTG_H_FS->UH_RX_CTRL;
			ep_info->tx_tog_res = USBOTG_H_FS->UH_TX_CTRL;
			return response;
		}
		else
		{
			usbfsh_ep_pid = (USB_PID_OUT << 4) | ep_info->ep_num;
			do
			{
				bytes_to_copy = (uint8_t)rem_len;
				if(rem_len > ep_info->max_packet_size)
					bytes_to_copy = ep_info->max_packet_size;
				USBOTG_H_FS->UH_TX_LEN = bytes_to_copy;

				for(uint8_t idx = 0; idx < bytes_to_copy; ++idx)
				{
					usbfsh_tx_buf[idx] = *pbuf;
					++pbuf;
				}

				response = usbfsh_transact(0xFFFF);
				if(response != USB_PID_ACK)
					return response;

				rem_len -= bytes_to_copy;
			} while(rem_len);
		}
	}

	// status stage (IN)
	usbfsh_ep_pid = (USB_PID_IN << 4) | ep_info->ep_num;
	response = usbfsh_transact(0xFFFF);
	ep_info->rx_tog_res = USBOTG_H_FS->UH_RX_CTRL;
	ep_info->tx_tog_res = USBOTG_H_FS->UH_TX_CTRL;
	return response;
}

uint8_t usbfsh_in_transfer(usbfsh_ep_info_t* ep_info, uint8_t* dest, uint16_t num_bytes)
{
	uint8_t response;
	uint8_t bytes_to_copy;

	USBOTG_H_FS->UH_RX_CTRL = ep_info->rx_tog_res | USBFS_UEP_R_AUTO_TOG;
	usbfsh_ep_pid = (USB_PID_IN << 4) | ep_info->ep_num;

	do
	{
		response = usbfsh_transact(usbfsh_in_transfer_nak_limit);
		if(!(USBOTG_H_FS->USB_INT_FG & USBFS_U_TOG_OK) || !response)
		{
			ep_info->rx_tog_res = USBOTG_H_FS->UH_RX_CTRL;
			return response;
		}

		bytes_to_copy = (uint8_t)USBOTG_H_FS->USB_RX_LEN;
		if(bytes_to_copy > num_bytes)
			bytes_to_copy = num_bytes;
		num_bytes -= bytes_to_copy;

		for(uint8_t idx = 0; idx < bytes_to_copy; ++idx)
		{
			*dest = usbfsh_rx_buf[idx];
			++dest;
		}

		if((uint8_t)USBOTG_H_FS->USB_RX_LEN != ep_info->max_packet_size)	//handle short or zero length packet
			break;
	} while(num_bytes);

	ep_info->rx_tog_res = USBOTG_H_FS->UH_RX_CTRL;
	return response;
}

uint8_t usbfsh_out_transfer(usbfsh_ep_info_t* ep_info, uint8_t* source, uint16_t num_bytes)
{
	uint8_t response;
	uint8_t bytes_to_copy;

	USBOTG_H_FS->UH_TX_CTRL = ep_info->tx_tog_res | USBFS_UEP_T_AUTO_TOG;
	usbfsh_ep_pid = (USB_PID_OUT << 4) | ep_info->ep_num;

	do
	{
		bytes_to_copy = (uint8_t)num_bytes;
		if(num_bytes > ep_info->max_packet_size)
			bytes_to_copy = ep_info->max_packet_size;
		USBOTG_H_FS->UH_TX_LEN = bytes_to_copy;

		for(uint8_t idx = 0; idx < bytes_to_copy; ++idx)
		{
			usbfsh_tx_buf[idx] = *source;
			++source;
		}

		response = usbfsh_transact(usbfsh_out_transfer_nak_limit);
		if(response != USB_PID_ACK)
		{
			ep_info->tx_tog_res = USBOTG_H_FS->UH_TX_CTRL;
			return response;
		}

		num_bytes -= bytes_to_copy;
	} while(num_bytes);

	ep_info->tx_tog_res = USBOTG_H_FS->UH_TX_CTRL;
	return response;
}

//HINT: set nak_limit to 0xFFFF for unlimited retries
uint8_t usbfsh_transact(uint16_t nak_limit)
{
	uint8_t sof_count;
	uint16_t nak_count = 0;
	uint8_t response;

	do
	{
		USBOTG_H_FS->UH_EP_PID = usbfsh_ep_pid;
		USBOTG_H_FS->USB_INT_FG = USBFS_UIF_TRANSFER;
		sof_count = 0;
		USBOTG_H_FS->USB_INT_FG = USBFS_UIF_HST_SOF;
		do
		{
			if(USBOTG_H_FS->USB_INT_FG & USBFS_UIF_HST_SOF)
			{
				++sof_count;
				USBOTG_H_FS->USB_INT_FG = USBFS_UIF_HST_SOF;
			}
		} while(!(USBOTG_H_FS->USB_INT_FG & USBFS_UIF_TRANSFER) && (sof_count != 2));
		USBOTG_H_FS->UH_EP_PID = USB_PID_NULL;

		// MASK_UIS_H_RES is not set to 0 by the hardware!
		if(!(USBOTG_H_FS->USB_INT_FG & USBFS_UIF_TRANSFER))
			return 0;

		response = USBOTG_H_FS->USB_INT_ST & USBFS_UIS_H_RES_MASK;
		if(response != USB_PID_NAK)
			return response;

		++nak_count;
	} while(nak_count <= nak_limit);

	return response;
}
