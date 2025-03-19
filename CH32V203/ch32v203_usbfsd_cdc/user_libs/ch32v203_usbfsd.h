/*
 * ch32v203_usbfsd.h
 *
 *  Created on: Aug 24, 2024
 *      Author: Steve
 */

#ifndef _CH32V203_USBFSD_H_
#define _CH32V203_USBFSD_H_

/* R8_USB_CTRL */
#define USBFS_UC_HOST_MODE          0x80
#define USBFS_UC_LOW_SPEED          0x40
#define USBFS_UC_DEV_PU_EN          0x20
#define USBFS_UC_SYS_CTRL_MASK      0x30
#define USBFS_UC_SYS_CTRL0          0x00
#define USBFS_UC_SYS_CTRL1          0x10
#define USBFS_UC_SYS_CTRL2          0x20
#define USBFS_UC_SYS_CTRL3          0x30
#define USBFS_UC_INT_BUSY           0x08
#define USBFS_UC_RESET_SIE          0x04
#define USBFS_UC_CLR_ALL            0x02
#define USBFS_UC_DMA_EN             0x01

/* R8_UDEV_CTRL */
#define USBFS_UD_PD_DIS             0x80      // disable USB UDP/UDM pulldown resistance: 0=enable pulldown, 1=disable
#define USBFS_UD_DP_PIN             0x20      // ReadOnly: indicate current UDP pin level
#define USBFS_UD_DM_PIN             0x10      // ReadOnly: indicate current UDM pin level
#define USBFS_UD_LOW_SPEED          0x04      // enable USB physical port low speed: 0=full speed, 1=low speed
#define USBFS_UD_GP_BIT             0x02      // general purpose bit
#define USBFS_UD_PORT_EN            0x01      // enable USB physical port I/O: 0=disable, 1=enable

/* R8_USB_DEV_AD */
#define USBFS_UDA_GP_BIT            0x80
#define USBFS_USB_ADDR_MASK         0x7F

/* R8_USB_MIS_ST */
#define USBFS_UMS_SOF_PRES          0x80
#define USBFS_UMS_SOF_ACT           0x40
#define USBFS_UMS_SIE_FREE          0x20
#define USBFS_UMS_R_FIFO_RDY        0x10
#define USBFS_UMS_BUS_RESET         0x08
#define USBFS_UMS_SUSPEND           0x04
#define USBFS_UMS_DM_LEVEL          0x02
#define USBFS_UMS_DEV_ATTACH        0x01

/* R8_USB_INT_FG */
#define USBFS_U_IS_NAK              0x80    // RO, indicate current USB transfer is NAK received
#define USBFS_U_TOG_OK              0x40    // RO, indicate current USB transfer toggle is OK
#define USBFS_U_SIE_FREE            0x20    // RO, indicate USB SIE free status
#define USBFS_UIF_FIFO_OV           0x10    // FIFO overflow interrupt flag for USB, write 1 to clear
#define USBFS_UIF_HST_SOF           0x08    // host SOF timer interrupt flag for USB host, write 1 to clear
#define USBFS_UIF_SUSPEND           0x04    // USB suspend or resume event interrupt flag, write 1 to clear
#define USBFS_UIF_TRANSFER          0x02    // USB transfer completion interrupt flag, write 1 to clear
#define USBFS_UIF_DETECT            0x01    // device detected event interrupt flag for USB host mode, write 1 to clear
#define USBFS_UIF_BUS_RST           0x01    // bus reset event interrupt flag for USB device mode, write 1 to clear

/* R8_USB_INT_ST */
#define USBFS_UIS_IS_NAK            0x80      // RO, indicate current USB transfer is NAK received for USB device mode
#define USBFS_UIS_TOG_OK            0x40      // RO, indicate current USB transfer toggle is OK
#define USBFS_UIS_TOKEN_MASK        0x30      // RO, bit mask of current token PID code received for USB device mode
#define USBFS_UIS_TOKEN_OUT         0x00
#define USBFS_UIS_TOKEN_SOF         0x10
#define USBFS_UIS_TOKEN_IN          0x20
#define USBFS_UIS_TOKEN_SETUP       0x30
// bUIS_TOKEN1 & bUIS_TOKEN0: current token PID code received for USB device mode
//   00: OUT token PID received
//   01: SOF token PID received
//   10: IN token PID received
//   11: SETUP token PID received
#define USBFS_UIS_ENDP_MASK         0x0F      // RO, bit mask of current transfer endpoint number for USB device mode
#define USBFS_UIS_H_RES_MASK        0x0F      // RO, bit mask of current transfer handshake response for USB host mode: 0000=no response, time out from device, others=handshake response PID received

//HINT: User Interface begins below this line.
#ifndef EP_0
#define EP_0 0
#define EP_1 1
#define EP_2 2
#define EP_3 3
#define EP_4 4
#define EP_5 5
#define EP_6 6
#define EP_7 7
#endif

//HINT: The SOF interrupt enable bit is marked as reserved in the reference manual, it may not function.
#define USBFSD_INT_SOF 0x80
#define USBFSD_INT_SUSPEND 0x04
#define USBFSD_INT_TRANSFER 0x02
#define USBFSD_INT_RESET 0x01

#define USBFSD_TOG_0 0x00
#define USBFSD_TOG_1 0x04
#define USBFSD_AUTOTOG_0 0x00
#define USBFSD_AUTOTOG_1 0x08

#define USBFSD_RES_ACK 0x00
#define USBFSD_RES_NONE 0x01
#define USBFSD_RES_NAK 0x02
#define USBFSD_RES_STALL 0x03

#define USBFSD_EP1_RX_EN 0x80
#define USBFSD_EP1_TX_EN 0x40
#define USBFSD_EP1_BUF_SINGLE 0x00
#define USBFSD_EP1_BUF_DOUBLE 0x10
#define USBFSD_EP2_RX_EN 0x08
#define USBFSD_EP2_TX_EN 0x04
#define USBFSD_EP2_BUF_SINGLE 0x00
#define USBFSD_EP2_BUF_DOUBLE 0x01
#define USBFSD_EP3_RX_EN 0x80
#define USBFSD_EP3_TX_EN 0x40
#define USBFSD_EP3_BUF_SINGLE 0x00
#define USBFSD_EP3_BUF_DOUBLE 0x10
#define USBFSD_EP4_RX_EN 0x08
#define USBFSD_EP4_TX_EN 0x04
#define USBFSD_EP4_BUF_SINGLE 0x00
#define USBFSD_EP4_BUF_DOUBLE 0x01
#define USBFSD_EP5_RX_EN 0x08
#define USBFSD_EP5_TX_EN 0x04
#define USBFSD_EP5_BUF_SINGLE 0x00
#define USBFSD_EP5_BUF_DOUBLE 0x01
#define USBFSD_EP6_RX_EN 0x80
#define USBFSD_EP6_TX_EN 0x40
#define USBFSD_EP6_BUF_SINGLE 0x00
#define USBFSD_EP6_BUF_DOUBLE 0x10
#define USBFSD_EP7_RX_EN 0x08
#define USBFSD_EP7_TX_EN 0x04
#define USBFSD_EP7_BUF_SINGLE 0x00
#define USBFSD_EP7_BUF_DOUBLE 0x01

typedef struct _USBFSD_CONFIG {
	uint32_t ep0_buf;
	uint32_t ep1_buf;
	uint32_t ep2_buf;
	uint32_t ep3_buf;
	uint32_t ep4_buf;
	uint32_t ep5_buf;
	uint32_t ep6_buf;
	uint32_t ep7_buf;
	uint8_t ep0_tx_tog_res;
	uint8_t ep0_rx_tog_res;
	uint8_t ep1_tx_tog_res;
	uint8_t ep1_rx_tog_res;
	uint8_t ep2_tx_tog_res;
	uint8_t ep2_rx_tog_res;
	uint8_t ep3_tx_tog_res;
	uint8_t ep3_rx_tog_res;
	uint8_t ep4_tx_tog_res;
	uint8_t ep4_rx_tog_res;
	uint8_t ep5_tx_tog_res;
	uint8_t ep5_rx_tog_res;
	uint8_t ep6_tx_tog_res;
	uint8_t ep6_rx_tog_res;
	uint8_t ep7_tx_tog_res;
	uint8_t ep7_rx_tog_res;
	uint8_t ep4_ep1_mode;
	uint8_t ep2_ep3_mode;
	uint8_t ep5_ep6_mode;
	uint8_t ep7_mode;
	uint8_t int_en;
} usbfsd_config_t;

extern void (*usbfsd_sof_callback)(void);
extern void (*usbfsd_out_callback)(uint8_t);
extern void (*usbfsd_in_callback)(uint8_t);
extern void (*usbfsd_setup_callback)(uint8_t);
extern void (*usbfsd_reset_callback)(void);
extern void (*usbfsd_wakeup_callback)(void);
extern void (*usbfsd_suspend_callback)(void);
extern volatile uint16_t usbfsd_sof_count;

#define usbfsd_enable_interrupts(interrupts) (USBOTG_FS->USB_INT_EN |= (interrupts))
#define usbfsd_disable_interrupts(interrupts) (USBOTG_FS->USB_INT_EN &= ~(interrupts))
#define usbfsd_set_addr(addr) (USBOTG_FS->USB_DEV_AD = (USBOTG_FS->USB_DEV_AD & USBFS_UDA_GP_BIT) | (addr))

#define usbfsd_set_ep0_tx_len(len) (USBOTG_FS->UEP0_TX_LEN = (len))
#define usbfsd_set_ep1_tx_len(len) (USBOTG_FS->UEP1_TX_LEN = (len))
#define usbfsd_set_ep2_tx_len(len) (USBOTG_FS->UEP2_TX_LEN = (len))
#define usbfsd_set_ep3_tx_len(len) (USBOTG_FS->UEP3_TX_LEN = (len))
#define usbfsd_set_ep4_tx_len(len) (USBOTG_FS->UEP4_TX_LEN = (len))
#define usbfsd_set_ep5_tx_len(len) (USBOTG_FS->UEP5_TX_LEN = (len))
#define usbfsd_set_ep6_tx_len(len) (USBOTG_FS->UEP6_TX_LEN = (len))
#define usbfsd_set_ep7_tx_len(len) (USBOTG_FS->UEP7_TX_LEN = (len))
#define usbfsd_get_rx_len() (USBOTG_FS->USB_RX_LEN)

#define usbfsd_set_ep0_out_toggle(tog) (USBOTG_FS->UEP0_RX_CTRL = (USBOTG_FS->UEP0_RX_CTRL & ~USBFSD_TOG_1) | (tog))
#define usbfsd_set_ep0_in_toggle(tog) (USBOTG_FS->UEP0_TX_CTRL = (USBOTG_FS->UEP0_TX_CTRL & ~USBFSD_TOG_1) | (tog))
#define usbfsd_set_ep1_out_toggle(tog) (USBOTG_FS->UEP1_RX_CTRL = (USBOTG_FS->UEP1_RX_CTRL & ~USBFSD_TOG_1) | (tog))
#define usbfsd_set_ep1_in_toggle(tog) (USBOTG_FS->UEP1_TX_CTRL = (USBOTG_FS->UEP1_TX_CTRL & ~USBFSD_TOG_1) | (tog))
#define usbfsd_set_ep2_out_toggle(tog) (USBOTG_FS->UEP2_RX_CTRL = (USBOTG_FS->UEP2_RX_CTRL & ~USBFSD_TOG_1) | (tog))
#define usbfsd_set_ep2_in_toggle(tog) (USBOTG_FS->UEP2_TX_CTRL = (USBOTG_FS->UEP2_TX_CTRL & ~USBFSD_TOG_1) | (tog))
#define usbfsd_set_ep3_out_toggle(tog) (USBOTG_FS->UEP3_RX_CTRL = (USBOTG_FS->UEP3_RX_CTRL & ~USBFSD_TOG_1) | (tog))
#define usbfsd_set_ep3_in_toggle(tog) (USBOTG_FS->UEP3_TX_CTRL = (USBOTG_FS->UEP3_TX_CTRL & ~USBFSD_TOG_1) | (tog))
#define usbfsd_set_ep4_out_toggle(tog) (USBOTG_FS->UEP4_RX_CTRL = (USBOTG_FS->UEP4_RX_CTRL & ~USBFSD_TOG_1) | (tog))
#define usbfsd_set_ep4_in_toggle(tog) (USBOTG_FS->UEP4_TX_CTRL = (USBOTG_FS->UEP4_TX_CTRL & ~USBFSD_TOG_1) | (tog))
#define usbfsd_set_ep5_out_toggle(tog) (USBOTG_FS->UEP5_RX_CTRL = (USBOTG_FS->UEP5_RX_CTRL & ~USBFSD_TOG_1) | (tog))
#define usbfsd_set_ep5_in_toggle(tog) (USBOTG_FS->UEP5_TX_CTRL = (USBOTG_FS->UEP5_TX_CTRL & ~USBFSD_TOG_1) | (tog))
#define usbfsd_set_ep6_out_toggle(tog) (USBOTG_FS->UEP6_RX_CTRL = (USBOTG_FS->UEP6_RX_CTRL & ~USBFSD_TOG_1) | (tog))
#define usbfsd_set_ep6_in_toggle(tog) (USBOTG_FS->UEP6_TX_CTRL = (USBOTG_FS->UEP6_TX_CTRL & ~USBFSD_TOG_1) | (tog))
#define usbfsd_set_ep7_out_toggle(tog) (USBOTG_FS->UEP7_RX_CTRL = (USBOTG_FS->UEP7_RX_CTRL & ~USBFSD_TOG_1) | (tog))
#define usbfsd_set_ep7_in_toggle(tog) (USBOTG_FS->UEP7_TX_CTRL = (USBOTG_FS->UEP7_TX_CTRL & ~USBFSD_TOG_1) | (tog))

#define usbfsd_get_ep0_out_toggle() (USBOTG_FS->UEP0_RX_CTRL & USBFSD_TOG_1)
#define usbfsd_get_ep0_in_toggle() (USBOTG_FS->UEP0_TX_CTRL & USBFSD_TOG_1)
#define usbfsd_get_ep1_out_toggle() (USBOTG_FS->UEP1_RX_CTRL & USBFSD_TOG_1)
#define usbfsd_get_ep1_in_toggle() (USBOTG_FS->UEP1_TX_CTRL & USBFSD_TOG_1)
#define usbfsd_get_ep2_out_toggle() (USBOTG_FS->UEP2_RX_CTRL & USBFSD_TOG_1)
#define usbfsd_get_ep2_in_toggle() (USBOTG_FS->UEP2_TX_CTRL & USBFSD_TOG_1)
#define usbfsd_get_ep3_out_toggle() (USBOTG_FS->UEP3_RX_CTRL & USBFSD_TOG_1)
#define usbfsd_get_ep3_in_toggle() (USBOTG_FS->UEP3_TX_CTRL & USBFSD_TOG_1)
#define usbfsd_get_ep4_out_toggle() (USBOTG_FS->UEP4_RX_CTRL & USBFSD_TOG_1)
#define usbfsd_get_ep4_in_toggle() (USBOTG_FS->UEP4_TX_CTRL & USBFSD_TOG_1)
#define usbfsd_get_ep5_out_toggle() (USBOTG_FS->UEP5_RX_CTRL & USBFSD_TOG_1)
#define usbfsd_get_ep5_in_toggle() (USBOTG_FS->UEP5_TX_CTRL & USBFSD_TOG_1)
#define usbfsd_get_ep6_out_toggle() (USBOTG_FS->UEP6_RX_CTRL & USBFSD_TOG_1)
#define usbfsd_get_ep6_in_toggle() (USBOTG_FS->UEP6_TX_CTRL & USBFSD_TOG_1)
#define usbfsd_get_ep7_out_toggle() (USBOTG_FS->UEP7_RX_CTRL & USBFSD_TOG_1)
#define usbfsd_get_ep7_in_toggle() (USBOTG_FS->UEP7_TX_CTRL & USBFSD_TOG_1)

#define usbfsd_toggle_ep0_out_toggle() (USBOTG_FS->UEP0_RX_CTRL ^= USBFSD_TOG_1)
#define usbfsd_toggle_ep0_in_toggle() (USBOTG_FS->UEP0_TX_CTRL ^= USBFSD_TOG_1)
#define usbfsd_toggle_ep1_out_toggle() (USBOTG_FS->UEP1_RX_CTRL ^= USBFSD_TOG_1)
#define usbfsd_toggle_ep1_in_toggle() (USBOTG_FS->UEP1_TX_CTRL ^= USBFSD_TOG_1)
#define usbfsd_toggle_ep2_out_toggle() (USBOTG_FS->UEP2_RX_CTRL ^= USBFSD_TOG_1)
#define usbfsd_toggle_ep2_in_toggle() (USBOTG_FS->UEP2_TX_CTRL ^= USBFSD_TOG_1)
#define usbfsd_toggle_ep3_out_toggle() (USBOTG_FS->UEP3_RX_CTRL ^= USBFSD_TOG_1)
#define usbfsd_toggle_ep3_in_toggle() (USBOTG_FS->UEP3_TX_CTRL ^= USBFSD_TOG_1)
#define usbfsd_toggle_ep4_out_toggle() (USBOTG_FS->UEP4_RX_CTRL ^= USBFSD_TOG_1)
#define usbfsd_toggle_ep4_in_toggle() (USBOTG_FS->UEP4_TX_CTRL ^= USBFSD_TOG_1)
#define usbfsd_toggle_ep5_out_toggle() (USBOTG_FS->UEP5_RX_CTRL ^= USBFSD_TOG_1)
#define usbfsd_toggle_ep5_in_toggle() (USBOTG_FS->UEP5_TX_CTRL ^= USBFSD_TOG_1)
#define usbfsd_toggle_ep6_out_toggle() (USBOTG_FS->UEP6_RX_CTRL ^= USBFSD_TOG_1)
#define usbfsd_toggle_ep6_in_toggle() (USBOTG_FS->UEP6_TX_CTRL ^= USBFSD_TOG_1)
#define usbfsd_toggle_ep7_out_toggle() (USBOTG_FS->UEP7_RX_CTRL ^= USBFSD_TOG_1)
#define usbfsd_toggle_ep7_in_toggle() (USBOTG_FS->UEP7_TX_CTRL ^= USBFSD_TOG_1)
#define usbfsd_is_toggle_ok() (USBOTG_FS->USB_INT_ST & USBFS_UIS_TOG_OK)

#define usbfsd_set_ep0_out_res(res) (USBOTG_FS->UEP0_RX_CTRL = (USBOTG_FS->UEP0_RX_CTRL & ~USBFSD_RES_STALL) | (res))
#define usbfsd_set_ep0_in_res(res) (USBOTG_FS->UEP0_TX_CTRL = (USBOTG_FS->UEP0_TX_CTRL & ~USBFSD_RES_STALL) | (res))
#define usbfsd_set_ep1_out_res(res) (USBOTG_FS->UEP1_RX_CTRL = (USBOTG_FS->UEP1_RX_CTRL & ~USBFSD_RES_STALL) | (res))
#define usbfsd_set_ep1_in_res(res) (USBOTG_FS->UEP1_TX_CTRL = (USBOTG_FS->UEP1_TX_CTRL & ~USBFSD_RES_STALL) | (res))
#define usbfsd_set_ep2_out_res(res) (USBOTG_FS->UEP2_RX_CTRL = (USBOTG_FS->UEP2_RX_CTRL & ~USBFSD_RES_STALL) | (res))
#define usbfsd_set_ep2_in_res(res) (USBOTG_FS->UEP2_TX_CTRL = (USBOTG_FS->UEP2_TX_CTRL & ~USBFSD_RES_STALL) | (res))
#define usbfsd_set_ep3_out_res(res) (USBOTG_FS->UEP3_RX_CTRL = (USBOTG_FS->UEP3_RX_CTRL & ~USBFSD_RES_STALL) | (res))
#define usbfsd_set_ep3_in_res(res) (USBOTG_FS->UEP3_TX_CTRL = (USBOTG_FS->UEP3_TX_CTRL & ~USBFSD_RES_STALL) | (res))
#define usbfsd_set_ep4_out_res(res) (USBOTG_FS->UEP4_RX_CTRL = (USBOTG_FS->UEP4_RX_CTRL & ~USBFSD_RES_STALL) | (res))
#define usbfsd_set_ep4_in_res(res) (USBOTG_FS->UEP4_TX_CTRL = (USBOTG_FS->UEP4_TX_CTRL & ~USBFSD_RES_STALL) | (res))
#define usbfsd_set_ep5_out_res(res) (USBOTG_FS->UEP5_RX_CTRL = (USBOTG_FS->UEP5_RX_CTRL & ~USBFSD_RES_STALL) | (res))
#define usbfsd_set_ep5_in_res(res) (USBOTG_FS->UEP5_TX_CTRL = (USBOTG_FS->UEP5_TX_CTRL & ~USBFSD_RES_STALL) | (res))
#define usbfsd_set_ep6_out_res(res) (USBOTG_FS->UEP6_RX_CTRL = (USBOTG_FS->UEP6_RX_CTRL & ~USBFSD_RES_STALL) | (res))
#define usbfsd_set_ep6_in_res(res) (USBOTG_FS->UEP6_TX_CTRL = (USBOTG_FS->UEP6_TX_CTRL & ~USBFSD_RES_STALL) | (res))
#define usbfsd_set_ep7_out_res(res) (USBOTG_FS->UEP7_RX_CTRL = (USBOTG_FS->UEP7_RX_CTRL & ~USBFSD_RES_STALL) | (res))
#define usbfsd_set_ep7_in_res(res) (USBOTG_FS->UEP7_TX_CTRL = (USBOTG_FS->UEP7_TX_CTRL & ~USBFSD_RES_STALL) | (res))

#define usbfsd_get_ep0_out_res() (USBOTG_FS->UEP0_RX_CTRL & USBFSD_RES_STALL)
#define usbfsd_get_ep0_in_res() (USBOTG_FS->UEP0_TX_CTRL & USBFSD_RES_STALL)
#define usbfsd_get_ep1_out_res() (USBOTG_FS->UEP1_RX_CTRL & USBFSD_RES_STALL)
#define usbfsd_get_ep1_in_res() (USBOTG_FS->UEP1_TX_CTRL & USBFSD_RES_STALL)
#define usbfsd_get_ep2_out_res() (USBOTG_FS->UEP2_RX_CTRL & USBFSD_RES_STALL)
#define usbfsd_get_ep2_in_res() (USBOTG_FS->UEP2_TX_CTRL & USBFSD_RES_STALL)
#define usbfsd_get_ep3_out_res() (USBOTG_FS->UEP3_RX_CTRL & USBFSD_RES_STALL)
#define usbfsd_get_ep3_in_res() (USBOTG_FS->UEP3_TX_CTRL & USBFSD_RES_STALL)
#define usbfsd_get_ep4_out_res() (USBOTG_FS->UEP4_RX_CTRL & USBFSD_RES_STALL)
#define usbfsd_get_ep4_in_res() (USBOTG_FS->UEP4_TX_CTRL & USBFSD_RES_STALL)
#define usbfsd_get_ep5_out_res() (USBOTG_FS->UEP5_RX_CTRL & USBFSD_RES_STALL)
#define usbfsd_get_ep5_in_res() (USBOTG_FS->UEP5_TX_CTRL & USBFSD_RES_STALL)
#define usbfsd_get_ep6_out_res() (USBOTG_FS->UEP6_RX_CTRL & USBFSD_RES_STALL)
#define usbfsd_get_ep6_in_res() (USBOTG_FS->UEP6_TX_CTRL & USBFSD_RES_STALL)
#define usbfsd_get_ep7_out_res() (USBOTG_FS->UEP7_RX_CTRL & USBFSD_RES_STALL)
#define usbfsd_get_ep7_in_res() (USBOTG_FS->UEP7_TX_CTRL & USBFSD_RES_STALL)

#define usbfsd_set_ep0_out_tog_res(tog_res) (USBOTG_FS->UEP0_RX_CTRL = (tog_res))
#define usbfsd_set_ep0_in_tog_res(tog_res) (USBOTG_FS->UEP0_TX_CTRL = (tog_res))
#define usbfsd_set_ep1_out_tog_res(tog_res) (USBOTG_FS->UEP1_RX_CTRL = (tog_res))
#define usbfsd_set_ep1_in_tog_res(tog_res) (USBOTG_FS->UEP1_TX_CTRL = (tog_res))
#define usbfsd_set_ep2_out_tog_res(tog_res) (USBOTG_FS->UEP2_RX_CTRL = (tog_res))
#define usbfsd_set_ep2_in_tog_res(tog_res) (USBOTG_FS->UEP2_TX_CTRL = (tog_res))
#define usbfsd_set_ep3_out_tog_res(tog_res) (USBOTG_FS->UEP3_RX_CTRL = (tog_res))
#define usbfsd_set_ep3_in_tog_res(tog_res) (USBOTG_FS->UEP3_TX_CTRL = (tog_res))
#define usbfsd_set_ep4_out_tog_res(tog_res) (USBOTG_FS->UEP4_RX_CTRL = (tog_res))
#define usbfsd_set_ep4_in_tog_res(tog_res) (USBOTG_FS->UEP4_TX_CTRL = (tog_res))
#define usbfsd_set_ep5_out_tog_res(tog_res) (USBOTG_FS->UEP5_RX_CTRL = (tog_res))
#define usbfsd_set_ep5_in_tog_res(tog_res) (USBOTG_FS->UEP5_TX_CTRL = (tog_res))
#define usbfsd_set_ep6_out_tog_res(tog_res) (USBOTG_FS->UEP6_RX_CTRL = (tog_res))
#define usbfsd_set_ep6_in_tog_res(tog_res) (USBOTG_FS->UEP6_TX_CTRL = (tog_res))
#define usbfsd_set_ep7_out_tog_res(tog_res) (USBOTG_FS->UEP7_RX_CTRL = (tog_res))
#define usbfsd_set_ep7_in_tog_res(tog_res) (USBOTG_FS->UEP7_TX_CTRL = (tog_res))

void usbfsd_init(const usbfsd_config_t* usb_config);

#endif /* _CH32V203_USBFSD_H_ */
