#define NO_XSFR_DEFINE
#include "CH559.H"
#include "CH559_USB_HOST.h"

UINT8 xdata usbh_rx_buf[64] _at_ 0x0000;
UINT8 xdata usbh_tx_buf[64] _at_ 0x0040;
UINT8 usbh_ep_pid;
UINT8 usbh_in_transfer_nak_limit;
UINT8 usbh_out_transfer_nak_limit;

void usbh_init(void)
{
	IE_USB = 0;
	USB_CTRL = bUC_HOST_MODE;
	USB_DEV_AD &= ~MASK_USB_ADDR;
	UH_RX_DMA = (UINT16)usbh_rx_buf;
	UH_TX_DMA = (UINT16)usbh_tx_buf;
	UH_EP_MOD = bUH_EP_TX_EN | bUH_EP_RX_EN;
	UH_RX_CTRL = 0x00;
	UH_TX_CTRL = 0x00;
	USB_CTRL = bUC_HOST_MODE | bUC_INT_BUSY | bUC_DMA_EN;
	UHUB0_CTRL = 0x00;
	UHUB1_CTRL = 0x00;
	
	UH_SETUP = bUH_SOF_EN;
	UH_EP_PID = 0x00;
	UH_TX_LEN = 0x00;
	USB_INT_EN = bUIE_TRANSFER | bUIE_DETECT;
	USB_INT_FG = 0xFF;
}

//HINT: copy control request to tx_buf before calling this function
// For control write transfers check U_TOG_OK, for control read transfers check the return value.
UINT8 usbh_control_transfer(usbh_ep_info_t* ep_info, UINT8* pbuf)
{
	UINT8 response;
	UINT16 rem_len;
	UINT8 idx;
	UINT8 bytes_to_copy;
	
	// setup stage
	UH_TX_CTRL = 0x00;
	UH_TX_LEN = 0x08;
	usbh_ep_pid = (USB_PID_SETUP << 4) | ep_info->ep_num;
	response = usbh_transact(0xFF);
	if(response != USB_PID_ACK)
		return response;
	
	// data stage
	UH_RX_CTRL = bUH_R_TOG | bUH_R_AUTO_TOG;
	UH_TX_CTRL = bUH_T_TOG | bUH_T_AUTO_TOG;
	rem_len = ((UINT16)(usbh_setup_req->wLengthH) << 8) | usbh_setup_req->wLengthL;
	if(rem_len && pbuf)
	{
		if(usbh_setup_req->bRequestType & USB_REQ_TYP_IN)
		{
			usbh_ep_pid = (USB_PID_IN << 4) | ep_info->ep_num;
			do
			{
				response = usbh_transact(0xFF);
				// U_TOG_OK is not cleared if no response is received?
				if(!U_TOG_OK || !response)
					return response;
				
				bytes_to_copy = USB_RX_LEN;
				if(bytes_to_copy > rem_len)
					bytes_to_copy = rem_len;
				rem_len -= bytes_to_copy;
				
				for(idx = 0; idx < bytes_to_copy; ++idx)
				{
					*pbuf = usbh_rx_buf[idx];
					++pbuf;
				}
				
				if(USB_RX_LEN != ep_info->max_packet_size)	//handle short or zero length packet
					break;
			} while(rem_len);
			
			// status stage (OUT)
			UH_TX_LEN = 0x00;
			usbh_ep_pid = (USB_PID_OUT << 4) | ep_info->ep_num;
			response = usbh_transact(0xFF);
			ep_info->rx_tog_res = UH_RX_CTRL;
			ep_info->tx_tog_res = UH_TX_CTRL;
			return response;
		}
		else
		{
			usbh_ep_pid = (USB_PID_OUT << 4) | ep_info->ep_num;
			do
			{
				bytes_to_copy = (UINT8)rem_len;
				if(rem_len > ep_info->max_packet_size)
					bytes_to_copy = ep_info->max_packet_size;
				UH_TX_LEN = bytes_to_copy;
				
				for(idx = 0; idx < bytes_to_copy; ++idx)
				{
					usbh_tx_buf[idx] = *pbuf;
					++pbuf;
				}
				
				response = usbh_transact(0xFF);
				if(response != USB_PID_ACK)
					return response;
				
				rem_len -= bytes_to_copy;
			} while(rem_len);
		}
	}
	
	// status stage (IN)
	usbh_ep_pid = (USB_PID_IN << 4) | ep_info->ep_num;
	response = usbh_transact(0xFF);
	ep_info->rx_tog_res = UH_RX_CTRL;
	ep_info->tx_tog_res = UH_TX_CTRL;
	return response;
}

UINT8 usbh_in_transfer(usbh_ep_info_t* ep_info, UINT8* dest, UINT16 num_bytes)
{
	UINT8 response;
	UINT8 idx;
	UINT8 bytes_to_copy;
	
	UH_RX_CTRL = ep_info->rx_tog_res | bUH_R_AUTO_TOG;
	usbh_ep_pid = (USB_PID_IN << 4) | ep_info->ep_num;
	
	do
	{
		response = usbh_transact(usbh_in_transfer_nak_limit);
		if(!U_TOG_OK || !response)
		{
			ep_info->rx_tog_res = UH_RX_CTRL;
			return response;
		}
		
		bytes_to_copy = USB_RX_LEN;
		if(bytes_to_copy > num_bytes)
			bytes_to_copy = num_bytes;
		num_bytes -= bytes_to_copy;
		
		for(idx = 0; idx < bytes_to_copy; ++idx)
		{
			*dest = usbh_rx_buf[idx];
			++dest;
		}
		
		if(USB_RX_LEN != ep_info->max_packet_size)	//handle short or zero length packet
			break;
	} while(num_bytes);
	
	ep_info->rx_tog_res = UH_RX_CTRL;
	return response;
}

UINT8 usbh_out_transfer(usbh_ep_info_t* ep_info, UINT8* source, UINT16 num_bytes)
{
	UINT8 response;
	UINT8 idx;
	UINT8 bytes_to_copy;
	
	UH_TX_CTRL = ep_info->tx_tog_res | bUH_T_AUTO_TOG;
	usbh_ep_pid = (USB_PID_OUT << 4) | ep_info->ep_num;
	
	do
	{
		bytes_to_copy = (UINT8)num_bytes;
		if(num_bytes > ep_info->max_packet_size)
			bytes_to_copy = ep_info->max_packet_size;
		UH_TX_LEN = bytes_to_copy;
		
		for(idx = 0; idx < bytes_to_copy; ++idx)
		{
			usbh_tx_buf[idx] = *source;
			++source;
		}
				
		response = usbh_transact(usbh_out_transfer_nak_limit);
		if(response != USB_PID_ACK)
		{
			ep_info->tx_tog_res = UH_TX_CTRL;
			return response;
		}
		
		num_bytes -= bytes_to_copy;
	} while(num_bytes);
	
	ep_info->tx_tog_res = UH_TX_CTRL;
	return response;
}

//HINT: set nak_limit to 0xFF for unlimited retries
//TODO: use U_SIE_FREE?
UINT8 usbh_transact(UINT8 nak_limit)
{
	UINT16 timeout_count;
	UINT8 nak_count = 0;
	UINT8 response;
	
	do
	{
		UH_EP_PID = usbh_ep_pid;
		UIF_TRANSFER = 0;
		timeout_count = USBH_TRANSACT_TIMEOUT;
		do
		{
			--timeout_count;
		} while (!UIF_TRANSFER && timeout_count);
		UH_EP_PID = USB_PID_NULL;
		
		// MASK_UIS_H_RES is not set to 0 by the hardware!
		if(!UIF_TRANSFER)
			return 0;
		
		response = USB_INT_ST & MASK_UIS_H_RES;
		if(response != USB_PID_NAK)
			return response;
		
		++nak_count;
	} while(nak_count <= nak_limit);
	
	return response;
}
