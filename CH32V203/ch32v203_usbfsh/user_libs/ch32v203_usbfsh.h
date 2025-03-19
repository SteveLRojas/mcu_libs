/*
 * ch32v203_usbfsh.h
 *
 *  Created on: Aug 29, 2024
 *      Author: Steve
 */

#ifndef _CH32V203_USBFSH_H_
#define _CH32V203_USBFSH_H_

/* USB PID */
 #ifndef USB_PID_SETUP
 #define USB_PID_NULL                0x00
 #define USB_PID_SOF                 0x05
 #define USB_PID_SETUP               0x0D
 #define USB_PID_IN                  0x09
 #define USB_PID_OUT                 0x01
 #define USB_PID_NYET                0x06
 #define USB_PID_ACK                 0x02
 #define USB_PID_NAK                 0x0A
 #define USB_PID_STALL               0x0E
 #define USB_PID_DATA0               0x03
 #define USB_PID_DATA1               0x0B
 #define USB_PID_DATA2               0x07
 #define USB_PID_MDATA               0x0F
 #define USB_PID_PRE                 0x0C
 #define USB_PID_SPLIT               0x08
 #define USB_PID_PING                0x04
 #endif

/* Bit Define for USB Request Type */
#ifndef USB_REQ_TYP_MASK
#define USB_REQ_TYP_IN              0x80
#define USB_REQ_TYP_OUT             0x00
#define USB_REQ_TYP_READ            0x80
#define USB_REQ_TYP_WRITE           0x00
#define USB_REQ_TYP_MASK            0x60
#define USB_REQ_TYP_STANDARD        0x00
#define USB_REQ_TYP_CLASS           0x20
#define USB_REQ_TYP_VENDOR          0x40
#define USB_REQ_TYP_RESERVED        0x60
#define USB_REQ_RECIP_MASK          0x1F
#define USB_REQ_RECIP_DEVICE        0x00
#define USB_REQ_RECIP_INTERF        0x01
#define USB_REQ_RECIP_ENDP          0x02
#define USB_REQ_RECIP_OTHER         0x03
#define USB_REQ_FEAT_REMOTE_WAKEUP  0x01
#define USB_REQ_FEAT_ENDP_HALT      0x00
#endif

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

/* R8_USB_INT_EN */
#define USBFS_UIE_DEV_SOF           0x80
#define USBFS_UIE_DEV_NAK           0x40
#define USBFS_UIE_FIFO_OV           0x10
#define USBFS_UIE_HST_SOF           0x08
#define USBFS_UIE_SUSPEND           0x04
#define USBFS_UIE_TRANSFER          0x02
#define USBFS_UIE_DETECT            0x01
#define USBFS_UIE_BUS_RST           0x01

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
#define USBFS_UIF_FIFO_OV           0x10    // FIFO overflow interrupt flag for USB, direct bit address clear or write 1 to clear
#define USBFS_UIF_HST_SOF           0x08    // host SOF timer interrupt flag for USB host, direct bit address clear or write 1 to clear
#define USBFS_UIF_SUSPEND           0x04    // USB suspend or resume event interrupt flag, direct bit address clear or write 1 to clear
#define USBFS_UIF_TRANSFER          0x02    // USB transfer completion interrupt flag, direct bit address clear or write 1 to clear
#define USBFS_UIF_DETECT            0x01    // device detected event interrupt flag for USB host mode, direct bit address clear or write 1 to clear
#define USBFS_UIF_BUS_RST           0x01    // bus reset event interrupt flag for USB device mode, direct bit address clear or write 1 to clear

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

/* R8_UEPn_TX_CTRL */
#define USBFS_UEP_T_AUTO_TOG        0x08      // enable automatic toggle after successful transfer completion on endpoint 1/2/3: 0=manual toggle, 1=automatic toggle
#define USBFS_UEP_T_TOG             0x04      // prepared data toggle flag of USB endpoint X transmittal (IN): 0=DATA0, 1=DATA1
#define USBFS_UEP_T_RES_MASK        0x03      // bit mask of handshake response type for USB endpoint X transmittal (IN)
#define USBFS_UEP_T_RES_ACK         0x00
#define USBFS_UEP_T_RES_NONE        0x01
#define USBFS_UEP_T_RES_NAK         0x02
#define USBFS_UEP_T_RES_STALL       0x03
// bUEP_T_RES1 & bUEP_T_RES0: handshake response type for USB endpoint X transmittal (IN)
//   00: DATA0 or DATA1 then expecting ACK (ready)
//   01: DATA0 or DATA1 then expecting no response, time out from host, for non-zero endpoint isochronous transactions
//   10: NAK (busy)
//   11: STALL (error)
// host aux setup

/* R8_UEPn_RX_CTRL, n=0-7 */
#define USBFS_UEP_R_AUTO_TOG        0x08      // enable automatic toggle after successful transfer completion on endpoint 1/2/3: 0=manual toggle, 1=automatic toggle
#define USBFS_UEP_R_TOG             0x04      // expected data toggle flag of USB endpoint X receiving (OUT): 0=DATA0, 1=DATA1
#define USBFS_UEP_R_RES_MASK        0x03      // bit mask of handshake response type for USB endpoint X receiving (OUT)
#define USBFS_UEP_R_RES_ACK         0x00
#define USBFS_UEP_R_RES_NONE        0x01
#define USBFS_UEP_R_RES_NAK         0x02
#define USBFS_UEP_R_RES_STALL       0x03
// RB_UEP_R_RES1 & RB_UEP_R_RES0: handshake response type for USB endpoint X receiving (OUT)
//   00: ACK (ready)
//   01: no response, time out to host, for non-zero endpoint isochronous transactions
//   10: NAK (busy)
//   11: STALL (error)

/* R8_UHOST_CTRL */
#define USBFS_UH_PD_DIS             0x80      // disable USB UDP/UDM pulldown resistance: 0=enable pulldown, 1=disable
#define USBFS_UH_DP_PIN             0x20      // ReadOnly: indicate current UDP pin level
#define USBFS_UH_DM_PIN             0x10      // ReadOnly: indicate current UDM pin level
#define USBFS_UH_LOW_SPEED          0x04      // enable USB port low speed: 0=full speed, 1=low speed
#define USBFS_UH_BUS_RESET          0x02      // control USB bus reset: 0=normal, 1=force bus reset
#define USBFS_UH_PORT_EN            0x01      // enable USB port: 0=disable, 1=enable port, automatic disabled if USB device detached

/* R8_UH_EP_MOD */
#define USBFS_UH_EP_TX_EN           0x40      // enable USB host OUT endpoint transmittal
#define USBFS_UH_EP_TBUF_MOD        0x10      // buffer mode of USB host OUT endpoint
// bUH_EP_TX_EN & bUH_EP_TBUF_MOD: USB host OUT endpoint buffer mode, buffer start address is UH_TX_DMA
//   0 x:  disable endpoint and disable buffer
//   1 0:  64 bytes buffer for transmittal (OUT endpoint)
//   1 1:  dual 64 bytes buffer by toggle bit bUH_T_TOG selection for transmittal (OUT endpoint), total=128bytes
#define USBFS_UH_EP_RX_EN           0x08      // enable USB host IN endpoint receiving
#define USBFS_UH_EP_RBUF_MOD        0x01      // buffer mode of USB host IN endpoint
// bUH_EP_RX_EN & bUH_EP_RBUF_MOD: USB host IN endpoint buffer mode, buffer start address is UH_RX_DMA
//   0 x:  disable endpoint and disable buffer
//   1 0:  64 bytes buffer for receiving (IN endpoint)
//   1 1:  dual 64 bytes buffer by toggle bit bUH_R_TOG selection for receiving (IN endpoint), total=128bytes

/* R16_UH_SETUP */
#define USBFS_UH_PRE_PID_EN         0x0400      // USB host PRE PID enable for low speed device via hub
#define USBFS_UH_SOF_EN             0x0004      // USB host automatic SOF enable

/* USB Setup Request */
typedef struct __attribute__((packed)) _USB_SETUP_REQ
{
    uint8_t  bRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} USB_SETUP_REQ;

//HINT: User interface begins below this line
//HINT: defines for use with the usbh_configure_port macros
#define USBFSH_PD_DISABLE 0x80
#define USBFSH_LOW_SPEED 0x04
#define USBFSH_FULL_SPEED 0x00
#define USBFSH_PORT_ENABLE 0x01

//HINT: defines to set rx_tog_res and tx_tog_res
#define USBFSH_TOG_0 0x00
#define USBFSH_TOG_1 0x04
#define USBFSH_AUTOTOG 0x08
#define USBFSH_SEND_ACK 0x00
#define USBFSH_SEND_NONE 0x01
#define USBFSH_EXPECT_ACK 0x00
#define USBFSH_EXPECT_NONE 0x01

typedef struct _USBFSH_EP_INFO
{
	uint8_t ep_num;
	uint8_t max_packet_size;
	uint8_t rx_tog_res;
	uint8_t tx_tog_res;
} usbfsh_ep_info_t;

extern uint8_t usbfsh_rx_buf[64];
extern uint8_t usbfsh_tx_buf[64];
extern uint16_t usbfsh_in_transfer_nak_limit;
extern uint16_t usbfsh_out_transfer_nak_limit;

#define usbfsh_setup_req ((USB_SETUP_REQ*)usbfsh_tx_buf)

void usbfsh_init(void);
uint8_t usbfsh_control_transfer(usbfsh_ep_info_t* ep_info, uint8_t* pbuf);
uint8_t usbfsh_in_transfer(usbfsh_ep_info_t* ep_info, uint8_t* dest, uint16_t num_bytes);
uint8_t usbfsh_out_transfer(usbfsh_ep_info_t* ep_info, uint8_t* source, uint16_t num_bytes);
uint8_t usbfsh_transact(uint16_t nak_limit);

#define usbfsh_configure_port(conf) (USBOTG_H_FS->UHOST_CTRL = conf)
#define usbfsh_ctrl_set_low_speed() (USBOTG_H_FS->USB_CTRL |= USBFS_UC_LOW_SPEED)
#define usbfsh_ctrl_set_full_speed() (USBOTG_H_FS->USB_CTRL &= ~USBFS_UC_LOW_SPEED)

#define usbfsh_begin_port_reset() (USBOTG_H_FS->UHOST_CTRL |= USBFS_UH_BUS_RESET)
#define usbfsh_end_port_reset() (USBOTG_H_FS->UHOST_CTRL &= ~USBFS_UH_BUS_RESET)
#define usbfsh_set_address(addr) (USBOTG_H_FS->USB_DEV_AD = (USBOTG_H_FS->USB_DEV_AD & ~USBFS_USB_ADDR_MASK) | (addr))
#define usbfsh_is_toggle_ok() (USBOTG_H_FS->USB_INT_FG & USBFS_U_TOG_OK)

#define usbfsh_port_is_attached() (USBOTG_H_FS->USB_MIS_ST & USBFS_UMS_DEV_ATTACH)
#define usbfsh_port_is_low_speed() (USBOTG_H_FS->USB_MIS_ST & USBFS_UMS_DM_LEVEL)

#endif /* _CH32V203_USBFSH_H_ */
