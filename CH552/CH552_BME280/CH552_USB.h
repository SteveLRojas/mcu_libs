#ifndef _CH552_USB_H_
#define _CH552_USB_H_

#define EP_0 0
#define EP_1 1
#define EP_2 2
#define EP_3 3
#define EP_4 4

#define USB_INT_SOF 0x80
#define USB_INT_NAK 0x40
#define USB_INT_FIFO_OV 0x10
#define USB_INT_SUSPEND 0x04
#define USB_INT_TRANSFER 0x02
#define USB_INT_RST 0x01

#define EP_OUT_TOG_0 0x00
#define EP_OUT_TOG_1 0x80
#define EP_IN_TOG_0 0x00
#define EP_IN_TOG_1 0x40
#define EP_AUTOTOG_0 0x00
#define EP_AUTOTOG_1 0x10

#define USB_OUT_RES_ACK 0x00
#define USB_OUT_RES_NONE 0x04
#define USB_OUT_RES_NAK 0x08
#define USB_OUT_RES_STALL 0x0C
#define USB_IN_RES_EXPECT_ACK 0x00
#define USB_IN_RES_EXPECT_NONE 0x01
#define USB_IN_RES_NAK 0x02
#define USB_IN_RES_STALL 0x03

#define USB_EP1_RX_EN 0x80
#define USB_EP1_TX_EN 0x40
#define USB_EP1_BUF_SINGLE 0x00
#define USB_EP1_BUF_DOUBLE 0x10
#define USB_EP2_RX_EN 0x08
#define USB_EP2_TX_EN 0x04
#define USB_EP2_BUF_SINGLE 0x00
#define USB_EP2_BUF_DOUBLE 0x01
#define USB_EP3_RX_EN 0x80
#define USB_EP3_TX_EN 0x40
#define USB_EP3_BUF_SINGLE 0x00
#define USB_EP3_BUF_DOUBLE 0x10
#define USB_EP4_RX_EN 0x08
#define USB_EP4_TX_EN 0x04

typedef struct _USB_CONFIG {
	UINT16 ep0_ep4_buf;
	UINT16 ep1_buf;
	UINT16 ep2_buf;
	UINT16 ep3_buf;
	UINT8 ep0_tog_res;
	UINT8 ep1_tog_res;
	UINT8 ep2_tog_res;
	UINT8 ep3_tog_res;
	UINT8 ep4_tog_res;
	UINT8 ep4_ep1_mode;
	UINT8 ep2_ep3_mode;
	UINT8 int_en;
} usb_config_t;

extern void (*usb_sof_callback)(void);
extern void (*usb_out_callback)(UINT8);
extern void (*usb_in_callback)(UINT8);
extern void (*usb_setup_callback)(UINT8);
extern void (*usb_rst_callback)(void);
extern void (*usb_suspend_callback)(void);
extern volatile UINT16 sof_count;

#define usb_enable_interrupts(interrupts) (USB_INT_EN |= (interrupts))
#define usb_disable_interrupts(interrupts) (USB_INT_EN &= ~(interrupts))
#define usb_set_addr(addr) (USB_DEV_AD = (USB_DEV_AD & bUDA_GP_BIT) | (addr))

#define usb_set_ep0_tx_len(len) (UEP0_T_LEN = (len))
#define usb_set_ep1_tx_len(len) (UEP1_T_LEN = (len))
#define usb_set_ep2_tx_len(len) (UEP2_T_LEN = (len))
#define usb_set_ep3_tx_len(len) (UEP3_T_LEN = (len))
#define usb_set_ep4_tx_len(len) (UEP4_T_LEN = (len))
#define usb_get_rx_len() (USB_RX_LEN)

#define usb_set_ep0_out_toggle(tog) (UEP0_CTRL = (UEP0_CTRL & ~EP_OUT_TOG_1) | (tog))
#define usb_set_ep0_in_toggle(tog) (UEP0_CTRL = (UEP0_CTRL & ~EP_IN_TOG_1) | (tog))
#define usb_set_ep1_out_toggle(tog) (UEP1_CTRL = (UEP1_CTRL & ~EP_OUT_TOG_1) | (tog))
#define usb_set_ep1_in_toggle(tog) (UEP1_CTRL = (UEP1_CTRL & ~EP_IN_TOG_1) | (tog))
#define usb_set_ep2_out_toggle(tog) (UEP2_CTRL = (UEP2_CTRL & ~EP_OUT_TOG_1) | (tog))
#define usb_set_ep2_in_toggle(tog) (UEP2_CTRL = (UEP2_CTRL & ~EP_IN_TOG_1) | (tog))
#define usb_set_ep3_out_toggle(tog) (UEP3_CTRL = (UEP3_CTRL & ~EP_OUT_TOG_1) | (tog))
#define usb_set_ep3_in_toggle(tog) (UEP3_CTRL = (UEP3_CTRL & ~EP_IN_TOG_1) | (tog))
#define usb_set_ep4_out_toggle(tog) (UEP4_CTRL = (UEP4_CTRL & ~EP_OUT_TOG_1) | (tog))
#define usb_set_ep4_in_toggle(tog) (UEP4_CTRL = (UEP4_CTRL & ~EP_IN_TOG_1) | (tog))

#define usb_get_ep0_out_toggle() (UEP0_CTRL & EP_OUT_TOG_1)
#define usb_get_ep0_in_toggle() (UEP0_CTRL & EP_IN_TOG_1)
#define usb_get_ep1_out_toggle() (UEP1_CTRL & EP_OUT_TOG_1)
#define usb_get_ep1_in_toggle() (UEP1_CTRL & EP_IN_TOG_1)
#define usb_get_ep2_out_toggle() (UEP2_CTRL & EP_OUT_TOG_1)
#define usb_get_ep2_in_toggle() (UEP2_CTRL & EP_IN_TOG_1)
#define usb_get_ep3_out_toggle() (UEP3_CTRL & EP_OUT_TOG_1)
#define usb_get_ep3_in_toggle() (UEP3_CTRL & EP_IN_TOG_1)
#define usb_get_ep4_out_toggle() (UEP4_CTRL & EP_OUT_TOG_1)
#define usb_get_ep4_in_toggle() (UEP4_CTRL & EP_IN_TOG_1)

#define usb_toggle_ep0_out_toggle() (UEP0_CTRL ^= EP_OUT_TOG_1)
#define usb_toggle_ep0_in_toggle() (UEP0_CTRL ^= EP_IN_TOG_1)
#define usb_toggle_ep1_out_toggle() (UEP1_CTRL ^= EP_OUT_TOG_1)
#define usb_toggle_ep1_in_toggle() (UEP1_CTRL ^= EP_IN_TOG_1)
#define usb_toggle_ep2_out_toggle() (UEP2_CTRL ^= EP_OUT_TOG_1)
#define usb_toggle_ep2_in_toggle() (UEP2_CTRL ^= EP_IN_TOG_1)
#define usb_toggle_ep3_out_toggle() (UEP3_CTRL ^= EP_OUT_TOG_1)
#define usb_toggle_ep3_in_toggle() (UEP3_CTRL ^= EP_IN_TOG_1)
#define usb_toggle_ep4_out_toggle() (UEP4_CTRL ^= EP_OUT_TOG_1)
#define usb_toggle_ep4_in_toggle() (UEP4_CTRL ^= EP_IN_TOG_1)
#define usb_is_toggle_ok() (U_TOG_OK)

#define usb_set_ep0_out_res(res) (UEP0_CTRL = (UEP0_CTRL & ~USB_OUT_RES_STALL) | (res))
#define usb_set_ep0_in_res(res) (UEP0_CTRL = (UEP0_CTRL & ~USB_IN_RES_STALL) | (res))
#define usb_set_ep1_out_res(res) (UEP1_CTRL = (UEP1_CTRL & ~USB_OUT_RES_STALL) | (res))
#define usb_set_ep1_in_res(res) (UEP1_CTRL = (UEP1_CTRL & ~USB_IN_RES_STALL) | (res))
#define usb_set_ep2_out_res(res) (UEP2_CTRL = (UEP2_CTRL & ~USB_OUT_RES_STALL) | (res))
#define usb_set_ep2_in_res(res) (UEP2_CTRL = (UEP2_CTRL & ~USB_IN_RES_STALL) | (res))
#define usb_set_ep3_out_res(res) (UEP3_CTRL = (UEP3_CTRL & ~USB_OUT_RES_STALL) | (res))
#define usb_set_ep3_in_res(res) (UEP3_CTRL = (UEP3_CTRL & ~USB_IN_RES_STALL) | (res))
#define usb_set_ep4_out_res(res) (UEP4_CTRL = (UEP4_CTRL & ~USB_OUT_RES_STALL) | (res))
#define usb_set_ep4_in_res(res) (UEP4_CTRL = (UEP4_CTRL & ~USB_IN_RES_STALL) | (res))

#define usb_get_ep0_out_res() (UEP0_CTRL & USB_OUT_RES_STALL)
#define usb_get_ep0_in_res() (UEP0_CTRL & USB_IN_RES_STALL)
#define usb_get_ep1_out_res() (UEP1_CTRL & USB_OUT_RES_STALL)
#define usb_get_ep1_in_res() (UEP1_CTRL & USB_IN_RES_STALL)
#define usb_get_ep2_out_res() (UEP2_CTRL & USB_OUT_RES_STALL)
#define usb_get_ep2_in_res() (UEP2_CTRL & USB_IN_RES_STALL)
#define usb_get_ep3_out_res() (UEP3_CTRL & USB_OUT_RES_STALL)
#define usb_get_ep3_in_res() (UEP3_CTRL & USB_IN_RES_STALL)
#define usb_get_ep4_out_res() (UEP4_CTRL & USB_OUT_RES_STALL)
#define usb_get_ep4_in_res() (UEP4_CTRL & USB_IN_RES_STALL)

#define usb_set_ep0_tog_res(tog_res) (UEP0_CTRL = (tog_res))
#define usb_set_ep1_tog_res(tog_res) (UEP1_CTRL = (tog_res))
#define usb_set_ep2_tog_res(tog_res) (UEP2_CTRL = (tog_res))
#define usb_set_ep3_tog_res(tog_res) (UEP3_CTRL = (tog_res))
#define usb_set_ep4_tog_res(tog_res) (UEP4_CTRL = (tog_res))

void usb_init(usb_config_t* usb_config);

#endif
