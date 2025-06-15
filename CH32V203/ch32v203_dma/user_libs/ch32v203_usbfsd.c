/*
 * ch32v203_usbfsd.c
 *
 *  Created on: Aug 24, 2024
 *      Author: Steve
 */
#include "ch32v20x.h"
#include "ch32v203_core.h"
#include "ch32v203_usbfsd.h"

void USBFS_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USBFSWakeUp_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void (*usbfsd_sof_callback)(void) = 0;
void (*usbfsd_out_callback)(uint8_t) = 0;
void (*usbfsd_in_callback)(uint8_t) = 0;
void (*usbfsd_setup_callback)(uint8_t) = 0;
void (*usbfsd_reset_callback)(void) = 0;
void (*usbfsd_wakeup_callback)(void) = 0;
void (*usbfsd_suspend_callback)(void) = 0;
volatile uint16_t usbfsd_sof_count;

void USBFS_IRQHandler(void)
{
	uint8_t int_flags = USBOTG_FS->USB_INT_FG;
	uint8_t int_status = USBOTG_FS->USB_INT_ST;

	if(int_flags & USBFS_UIF_TRANSFER)
	{
		switch(int_status & USBFS_UIS_TOKEN_MASK)
		{
			case USBFS_UIS_TOKEN_OUT:
				if(usbfsd_out_callback)
					usbfsd_out_callback(int_status & USBFS_UIS_ENDP_MASK);
				break;
			case USBFS_UIS_TOKEN_SOF:
				++usbfsd_sof_count;
				if(usbfsd_sof_callback)
					usbfsd_sof_callback();
				break;
			case USBFS_UIS_TOKEN_IN:
				if(usbfsd_in_callback)
					usbfsd_in_callback(int_status & USBFS_UIS_ENDP_MASK);
				break;
			case USBFS_UIS_TOKEN_SETUP:
				if(usbfsd_setup_callback)
					usbfsd_setup_callback(0);	//HW gives wrong endpoint numbers for SETUP tokens
				break;
			default:
				break;
		}
		USBOTG_FS->USB_INT_FG = USBFS_UIF_TRANSFER;
	}
	if(int_flags & USBFS_UIF_BUS_RST)
	{
		if(usbfsd_reset_callback)
			usbfsd_reset_callback();
		USBOTG_FS->USB_DEV_AD = 0x00;
		USBOTG_FS->USB_INT_FG = USBFS_UIF_SUSPEND | USBFS_UIF_TRANSFER | USBFS_UIF_BUS_RST;
	}
	if(int_flags & USBFS_UIF_SUSPEND)
	{
		USBOTG_FS->USB_INT_FG = USBFS_UIF_SUSPEND;
		if(USBOTG_FS->USB_MIS_ST & USBFS_UMS_SUSPEND)
		{
			if(usbfsd_suspend_callback)
				usbfsd_suspend_callback();
		}
		else
		{
			if(usbfsd_wakeup_callback)
				usbfsd_wakeup_callback();
		}
	}

	USBOTG_FS->USB_INT_FG = USBFS_UIF_FIFO_OV | USBFS_UIF_HST_SOF | USBFS_UIF_DETECT;
}

void USBFSWakeUp_IRQHandler(void)
{
	if(USBOTG_FS->USB_INT_FG & USBFS_UIF_SUSPEND)
	{
		USBOTG_FS->USB_INT_FG = USBFS_UIF_SUSPEND;
		if(USBOTG_FS->USB_MIS_ST & USBFS_UMS_SUSPEND)
		{
			if(usbfsd_suspend_callback)
				usbfsd_suspend_callback();
		}
		else
		{
			if(usbfsd_wakeup_callback)
				usbfsd_wakeup_callback();
		}
	}

	USBOTG_FS->USB_INT_FG = USBFS_UIF_FIFO_OV | USBFS_UIF_HST_SOF | USBFS_UIF_DETECT;
}

void usbfsd_init(const usbfsd_config_t* usb_config)
{
	usbfsd_sof_count = 0;
	USBOTG_FS->USB_CTRL = USBFS_UC_DEV_PU_EN | USBFS_UC_INT_BUSY | USBFS_UC_DMA_EN;
	USBOTG_FS->USB_DEV_AD = 0x00;
	USBOTG_FS->UDEV_CTRL = USBFS_UD_PD_DIS | USBFS_UD_PORT_EN;

	USBOTG_FS->UEP0_DMA = usb_config->ep0_buf;
	USBOTG_FS->UEP1_DMA = usb_config->ep1_buf;
	USBOTG_FS->UEP2_DMA = usb_config->ep2_buf;
	USBOTG_FS->UEP3_DMA = usb_config->ep3_buf;
	USBOTG_FS->UEP4_DMA = usb_config->ep4_buf;
	USBOTG_FS->UEP5_DMA = usb_config->ep5_buf;
	USBOTG_FS->UEP6_DMA = usb_config->ep6_buf;
	USBOTG_FS->UEP7_DMA = usb_config->ep7_buf;

	USBOTG_FS->UEP0_TX_CTRL = usb_config->ep0_tx_tog_res;
	USBOTG_FS->UEP0_RX_CTRL = usb_config->ep0_rx_tog_res;
	USBOTG_FS->UEP1_TX_CTRL = usb_config->ep1_tx_tog_res;
	USBOTG_FS->UEP1_RX_CTRL = usb_config->ep1_rx_tog_res;
	USBOTG_FS->UEP2_TX_CTRL = usb_config->ep2_tx_tog_res;
	USBOTG_FS->UEP2_RX_CTRL = usb_config->ep2_rx_tog_res;
	USBOTG_FS->UEP3_TX_CTRL = usb_config->ep3_tx_tog_res;
	USBOTG_FS->UEP3_RX_CTRL = usb_config->ep3_rx_tog_res;
	USBOTG_FS->UEP4_TX_CTRL = usb_config->ep4_tx_tog_res;
	USBOTG_FS->UEP4_RX_CTRL = usb_config->ep4_rx_tog_res;
	USBOTG_FS->UEP5_TX_CTRL = usb_config->ep5_tx_tog_res;
	USBOTG_FS->UEP5_RX_CTRL = usb_config->ep5_rx_tog_res;
	USBOTG_FS->UEP6_TX_CTRL = usb_config->ep6_tx_tog_res;
	USBOTG_FS->UEP6_RX_CTRL = usb_config->ep6_rx_tog_res;
	USBOTG_FS->UEP7_TX_CTRL = usb_config->ep7_tx_tog_res;
	USBOTG_FS->UEP7_RX_CTRL = usb_config->ep7_rx_tog_res;

	USBOTG_FS->UEP4_1_MOD = usb_config->ep4_ep1_mode;
	USBOTG_FS->UEP2_3_MOD = usb_config->ep2_ep3_mode;
	USBOTG_FS->UEP5_6_MOD = usb_config->ep5_ep6_mode;

	USBOTG_FS->UEP0_TX_LEN = 0;
	USBOTG_FS->UEP1_TX_LEN = 0;
	USBOTG_FS->UEP2_TX_LEN = 0;
	USBOTG_FS->UEP3_TX_LEN = 0;
	USBOTG_FS->UEP4_TX_LEN = 0;
	USBOTG_FS->UEP5_TX_LEN = 0;
	USBOTG_FS->UEP6_TX_LEN = 0;
	USBOTG_FS->UEP7_TX_LEN = 0;

	USBOTG_FS->USB_INT_EN = usb_config->int_en;
	USBOTG_FS->USB_INT_FG = 0xFF;
	core_enable_irq(USBFS_IRQn);
	core_enable_irq(USBFSWakeUp_IRQn);
}
