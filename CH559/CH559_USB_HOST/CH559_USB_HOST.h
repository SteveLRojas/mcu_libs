#ifndef _CH559_USB_HOST_H_
#define _CH559_USB_HOST_H_

#define USBH_TRANSACT_TIMEOUT 800

//HINT: defines for use with the usbh_configure_port macros
#define USBH_DP_PD_DISABLE 0x20
#define USBH_DM_PD_DISABLE 0x10
#define USBH_LOW_SPEED 0x04
#define USBH_FULL_SPEED 0x00
#define USBH_PORT_ENABLE 0x01

//HINT: defines to set rx_tog_res and tx_tog_res
#define USBH_IN_TOG_0 0x00
#define USBH_IN_TOG_1 0x80
#define USBH_IN_AUTOTOG 0x10
#define USBH_IN_SEND_ACK 0x00
#define USBH_IN_SEND_NONE 0x04
#define USBH_OUT_TOG_0 0x00
#define USBH_OUT_TOG_1 0x40
#define USBH_OUT_AUTOTOG 0x10
#define USBH_OUT_EXPECT_ACK 0x00
#define USBH_OUT_EXPECT_NONE 0x01

typedef struct _USBH_EP_INFO {
	UINT8 ep_num;
	UINT8 max_packet_size;
	UINT8 rx_tog_res;
	UINT8 tx_tog_res;
} usbh_ep_info_t;

extern UINT8 xdata usbh_rx_buf[64];
extern UINT8 xdata usbh_tx_buf[64];
extern UINT8 usbh_in_transfer_nak_limit;
extern UINT8 usbh_out_transfer_nak_limit;


#define usbh_setup_req ((USB_SETUP_REQ xdata*)usbh_tx_buf)

void usbh_init(void);
UINT8 usbh_control_transfer(usbh_ep_info_t* ep_info, UINT8* pbuf);
UINT8 usbh_in_transfer(usbh_ep_info_t* ep_info, UINT8* dest, UINT16 num_bytes);
UINT8 usbh_out_transfer(usbh_ep_info_t* ep_info, UINT8* source, UINT16 num_bytes);
UINT8 usbh_transact(UINT8 nak_limit);

#define usbh_configure_port0(conf) (UHUB0_CTRL = conf)
#define usbh_configure_port1(conf) (UHUB1_CTRL = conf)
#define usbh_ctrl_set_low_speed() (USB_CTRL |= bUC_LOW_SPEED)
#define usbh_ctrl_set_full_speed() (USB_CTRL &= ~bUC_LOW_SPEED)
#define usbh_begin_port0_reset() (UHUB0_CTRL |= bUH_BUS_RESET)
#define usbh_end_port0_reset() (UHUB0_CTRL &= ~bUH_BUS_RESET)
#define usbh_begin_port1_reset() (UHUB1_CTRL |= bUH_BUS_RESET)
#define usbh_end_port1_reset() (UHUB1_CTRL &= ~bUH_BUS_RESET)
#define usbh_set_address(addr) (USB_DEV_AD = (USB_DEV_AD & ~MASK_USB_ADDR) | (addr))

#define usbh_port0_is_attached() (USB_HUB_ST & bUHS_H0_ATTACH)
#define usbh_port0_is_low_speed() (USB_HUB_ST & bUHS_DM_LEVEL)
#define usbh_port1_is_attached() (USB_HUB_ST & bUHS_H1_ATTACH)
#define usbh_port1_is_low_speed() (USB_HUB_ST & bUHS_HM_LEVEL)

#endif