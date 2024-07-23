#include "CH552.H"
#include "CH552_USB.h"

void (*usb_sof_callback)(void) = NULL;
void (*usb_out_callback)(UINT8) = NULL;
void (*usb_in_callback)(UINT8) = NULL;
void (*usb_setup_callback)(UINT8) = NULL;
void (*usb_rst_callback)(void) = NULL;
void (*usb_suspend_callback)(void) = NULL;
volatile UINT16 sof_count;

// HINT: Enabling SOF interrupts makes SOF tokens trigger UIF_TRANSFER. This breaks interrupt transfers, and any other transfers that happen immediately after a SOF
void usb_isr(void) interrupt INT_NO_USB
{
	if(UIF_TRANSFER)
	{
		switch(USB_INT_ST & MASK_UIS_TOKEN)
		{
			case UIS_TOKEN_OUT:
				if(usb_out_callback)
					usb_out_callback(USB_INT_ST & MASK_UIS_ENDP);
				break;
			case UIS_TOKEN_SOF:
				++sof_count;
				if(usb_sof_callback)
					usb_sof_callback();
				break;
			case UIS_TOKEN_IN:
				if(usb_in_callback)
					usb_in_callback(USB_INT_ST & MASK_UIS_ENDP);
				break;
			case UIS_TOKEN_SETUP:
				if(usb_setup_callback)
					usb_setup_callback(USB_INT_ST & MASK_UIS_ENDP);
				break;
			default:
				break;
		}
		UIF_TRANSFER = 0;
	}
	if(UIF_BUS_RST)
	{
		if(usb_rst_callback)
			usb_rst_callback();
		USB_DEV_AD = 0x00;
		UIF_SUSPEND = 0;
		UIF_TRANSFER = 0;
		UIF_BUS_RST = 0;
	}
	if(UIF_SUSPEND)
	{
		UIF_SUSPEND = 0;
		if(usb_suspend_callback && (USB_MIS_ST & bUMS_SUSPEND))
			usb_suspend_callback();
	}
	else
	{
		USB_INT_FG = 0xFF;
	}
}

void usb_init(usb_config_t* usb_config)
{
	sof_count = 0;
	USB_CTRL = bUC_DEV_PU_EN | bUC_SYS_CTRL1 | bUC_INT_BUSY | bUC_DMA_EN;
	USB_DEV_AD = 0x00;
	UDEV_CTRL = bUD_PD_DIS | bUD_PORT_EN;
	
	UEP0_DMA = usb_config->ep0_ep4_buf;
	UEP1_DMA = usb_config->ep1_buf;
	UEP2_DMA = usb_config->ep2_buf;
	UEP3_DMA = usb_config->ep3_buf;
	UEP0_CTRL = usb_config->ep0_tog_res;
	UEP1_CTRL = usb_config->ep1_tog_res;
	UEP2_CTRL = usb_config->ep2_tog_res;
	UEP3_CTRL = usb_config->ep3_tog_res;
	UEP4_CTRL = usb_config->ep4_tog_res;
	UEP4_1_MOD = usb_config->ep4_ep1_mode;
	UEP2_3_MOD = usb_config->ep2_ep3_mode;
	UEP0_T_LEN = 0;
	UEP1_T_LEN = 0;
	UEP2_T_LEN = 0;
	UEP3_T_LEN = 0;
	UEP4_T_LEN = 0;
	
	USB_INT_EN = usb_config->int_en;
	USB_INT_FG = 0xFF;
	IE_USB = 1;
}
