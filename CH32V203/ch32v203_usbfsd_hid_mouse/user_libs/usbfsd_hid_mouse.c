/*
 * usbfsd_hid_mouse.c
 *
 *  Created on: Aug 25, 2024
 *      Author: Steve
 */

#include "ch32v20x.h"
#include "ch32v203_usbfsd.h"
#include "usbfsd_hid_mouse.h"

#define HID_DEV_DESCR_SIZE 18
#define HID_CONF_DESCR_SIZE 34
#define HID_REPORT_DESCR_SIZE 52
#define HID_MOUSE_REPORT_SIZE 4

/* USB standard device request code */
#ifndef USB_GET_DESCRIPTOR
#define USB_GET_STATUS          0x00
#define USB_CLEAR_FEATURE       0x01
#define USB_SET_FEATURE         0x03
#define USB_SET_ADDRESS         0x05
#define USB_GET_DESCRIPTOR      0x06
#define USB_SET_DESCRIPTOR      0x07
#define USB_GET_CONFIGURATION   0x08
#define USB_SET_CONFIGURATION   0x09
#define USB_GET_INTERFACE       0x0A
#define USB_SET_INTERFACE       0x0B
#define USB_SYNCH_FRAME         0x0C
#endif

/* USB HID class request code */
#ifndef HID_GET_REPORT
#define HID_GET_REPORT          0x01
#define HID_GET_IDLE            0x02
#define HID_GET_PROTOCOL        0x03
#define HID_SET_REPORT          0x09
#define HID_SET_IDLE            0x0A
#define HID_SET_PROTOCOL        0x0B
#endif

/* Bit define for USB request type */
#ifndef USB_REQ_TYP_MASK
#define USB_REQ_TYP_IN          0x80            /* control IN, device to host */
#define USB_REQ_TYP_OUT         0x00            /* control OUT, host to device */
#define USB_REQ_TYP_READ        0x80            /* control read, device to host */
#define USB_REQ_TYP_WRITE       0x00            /* control write, host to device */
#define USB_REQ_TYP_MASK        0x60            /* bit mask of request type */
#define USB_REQ_TYP_STANDARD    0x00
#define USB_REQ_TYP_CLASS       0x20
#define USB_REQ_TYP_VENDOR      0x40
#define USB_REQ_TYP_RESERVED    0x60
#define USB_REQ_RECIP_MASK      0x1F            /* bit mask of request recipient */
#define USB_REQ_RECIP_DEVICE    0x00
#define USB_REQ_RECIP_INTERF    0x01
#define USB_REQ_RECIP_ENDP      0x02
#define USB_REQ_RECIP_OTHER     0x03
#endif

/* USB descriptor type */
#ifndef USB_DESCR_TYP_DEVICE
#define USB_DESCR_TYP_DEVICE    0x01
#define USB_DESCR_TYP_CONFIG    0x02
#define USB_DESCR_TYP_STRING    0x03
#define USB_DESCR_TYP_INTERF    0x04
#define USB_DESCR_TYP_ENDP      0x05
#define USB_DESCR_TYP_QUALIF    0x06
#define USB_DESCR_TYP_SPEED     0x07
#define USB_DESCR_TYP_OTG       0x09
#define USB_DESCR_TYP_HID       0x21
#define USB_DESCR_TYP_REPORT    0x22
#define USB_DESCR_TYP_PHYSIC    0x23
#define USB_DESCR_TYP_CS_INTF   0x24
#define USB_DESCR_TYP_CS_ENDP   0x25
#define USB_DESCR_TYP_HUB       0x29
#endif

typedef struct _USB_SETUP_REQ
{
    uint8_t bRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} usb_setup_req_t;

typedef struct _HID_MOUSE_REPORT
{
	uint8_t buttons;
	uint8_t x_rel;
	uint8_t y_rel;
	uint8_t scroll_rel;
} hid_mouse_report_t;

volatile uint8_t ep0_buffer[HID_ENDP0_BUF_SIZE] __attribute__ ((aligned(4)));
volatile uint8_t ep1_buffer[HID_ENDP1_SIZE] __attribute__ ((aligned(4)));

#define hid_setup_buf ((usb_setup_req_t*)ep0_buffer)
#define hid_mouse_report ((hid_mouse_report_t*)ep1_buffer)

uint8_t hid_setup_req;
uint8_t hid_setup_type;
uint16_t hid_setup_len;

const uint8_t* descriptor_ptr;
uint8_t hid_address;
uint8_t hid_config;

volatile uint8_t hid_report_pending;
volatile uint8_t hid_idle_rate;
uint8_t hid_protocol;

/* USB Device Descriptors */
static const uint8_t hid_device_descriptor[] __attribute__ ((aligned(2))) =
{
    HID_DEV_DESCR_SIZE,				// bLength
    0x01,                           // bDescriptorType
    0x10, 0x01,                     // bcdUSB
    0x00,                           // bDeviceClass
    0x00,                           // bDeviceSubClass
    0x00,                           // bDeviceProtocol
    HID_ENDP0_SIZE,					// bMaxPacketSize0
    0x86, 0x1A,                     // idVendor
    0x0C, 0xFE,                     // idProduct
    0x00, 0x01,                     // bcdDevice
    0x01,                           // iManufacturer
    0x02,                           // iProduct
    0x03,                           // iSerialNumber
    0x01,                           // bNumConfigurations
};

/* USB Configuration Descriptors */
static const uint8_t hid_config_descriptor[] __attribute__ ((aligned(2))) =
{
    /* Configuration Descriptor */
    0x09,                           // bLength
    0x02,                           // bDescriptorType
    HID_CONF_DESCR_SIZE & 0xFF, HID_CONF_DESCR_SIZE >> 8, // wTotalLength
    0x01,                           // bNumInterfaces
    0x01,                           // bConfigurationValue
    0x00,                           // iConfiguration
    0x80,                           // bmAttributes: bus powered, no wakeup
    0x32,                           // MaxPower: 100mA

    /* Interface Descriptor (Mouse) */
    0x09,                           // bLength
    0x04,                           // bDescriptorType
    0x00,                           // bInterfaceNumber
    0x00,                           // bAlternateSetting
    0x01,                           // bNumEndpoints
    0x03,                           // bInterfaceClass
    0x01,                           // bInterfaceSubClass
    0x02,                           // bInterfaceProtocol: Mouse
    0x04,                           // iInterface

    /* HID Descriptor (Mouse) */
    0x09,                           // bLength
    0x21,                           // bDescriptorType
    0x10, 0x01,                     // bcdHID
    0x00,                           // bCountryCode
    0x01,                           // bNumDescriptors
    0x22,                           // bDescriptorType
    HID_REPORT_DESCR_SIZE & 0xFF, HID_REPORT_DESCR_SIZE >> 8, // wDescriptorLength

    /* Endpoint Descriptor (Mouse) */
    0x07,                           // bLength
    0x05,                           // bDescriptorType
    0x81,                           // bEndpointAddress: IN Endpoint 1
    0x03,                           // bmAttributes: Interrupt
    HID_ENDP1_SIZE & 0xFF, HID_ENDP1_SIZE >> 8, // wMaxPacketSize
    0x01                            // bInterval: 1mS
};

/* HID Report Descriptor */
static const uint8_t hid_report_descriptor[] __attribute__ ((aligned(2))) =
{
    0x05, 0x01,                     // Usage Page (Generic Desktop)
    0x09, 0x02,                     // Usage (Mouse)
    0xA1, 0x01,                     // Collection (Application)
    0x09, 0x01,                     // Usage (Pointer)
    0xA1, 0x00,                     // Collection (Physical)
    0x05, 0x09,                     // Usage Page (Button)
    0x19, 0x01,                     // Usage Minimum (Button 1)
    0x29, 0x03,                     // Usage Maximum (Button 3)
    0x15, 0x00,                     // Logical Minimum (0)
    0x25, 0x01,                     // Logical Maximum (1)
    0x75, 0x01,                     // Report Size (1)
    0x95, 0x03,                     // Report Count (3)
    0x81, 0x02,                     // Input (Data,Variable,Absolute)
    0x75, 0x05,                     // Report Size (5)
    0x95, 0x01,                     // Report Count (1)
    0x81, 0x03,                     // Input (Constant,Variable,Absolute)
    0x05, 0x01,                     // Usage Page (Generic Desktop)
    0x09, 0x30,                     // Usage (X)
    0x09, 0x31,                     // Usage (Y)
    0x09, 0x38,                     // Usage (Wheel)
    0x15, 0x81,                     // Logical Minimum (-127)
    0x25, 0x7F,                     // Logical Maximum (127)
    0x75, 0x08,                     // Report Size (8)
    0x95, 0x03,                     // Report Count (3)
    0x81, 0x06,                     // Input (Data,Variable,Relative)
    0xC0,                           // End Collection
    0xC0                            // End Collection
};

/* USB String Descriptors */
static const uint8_t hid_string_lang_id[] __attribute__ ((aligned(2))) =
{
	0x04,
	0x03,
	0x09,
	0x04
};

/* USB Device String Vendor */
static const uint8_t hid_string_vendor[] __attribute__ ((aligned(2))) =
{
	14,
	0x03,
	'w',0,'c',0,'h',0,'.',0,'c',0,'n',0
};

/* USB Device String Product */
static const uint8_t hid_string_product[] __attribute__ ((aligned(2))) =
{
    20,
    0x03,
    'U', 0, 'S', 0, 'B', 0, ' ', 0, 'M', 0, 'o', 0, 'u', 0, 's', 0, 'e', 0
};

/* USB Device String Serial */
static const uint8_t hid_string_serial[] __attribute__ ((aligned(2))) =
{
	22,
	0x03,
	'D', 0, 'E', 0, 'A', 0, 'D', 0, 'B', 0, 'E', 0 , 'E', 0, 'F', 0, '0', 0, '2', 0
};

void hid_copy_descriptor(uint8_t len)
{
	volatile uint8_t* dest = ep0_buffer;
	while(len)
	{
		*dest = *descriptor_ptr;
		++dest;
		++descriptor_ptr;
		--len;
	}
}

void hid_on_out(uint8_t ep)
{
	if(ep == EP_0)
	{
		usbfsd_set_ep0_tx_len(0);
		usbfsd_set_ep0_out_tog_res(USBFSD_TOG_1 | USBFSD_RES_ACK);
		usbfsd_set_ep0_in_tog_res(USBFSD_TOG_1 | USBFSD_RES_ACK);
	}
}

void hid_on_in(uint8_t ep)
{
	uint8_t len;

	if(ep == EP_0)
	{
		if((hid_setup_type & USB_REQ_TYP_MASK) == USB_REQ_TYP_STANDARD)
		{
			switch(hid_setup_req)
			{
				case USB_GET_DESCRIPTOR:
					len = hid_setup_len >= HID_ENDP0_SIZE ? HID_ENDP0_SIZE : hid_setup_len;
					hid_copy_descriptor(len);
					hid_setup_len -= len;
					usbfsd_set_ep0_tx_len(len);
					usbfsd_toggle_ep0_in_toggle();
					break;
				case USB_SET_ADDRESS:
					usbfsd_set_addr(hid_address);
					usbfsd_set_ep0_in_res(USBFSD_RES_NAK);
					break;
				default:
					usbfsd_set_ep0_tx_len(0);
					usbfsd_set_ep0_in_res(USBFSD_RES_NAK);
					break;
			}
		}
		else
		{
			usbfsd_set_ep0_tx_len(0);
			usbfsd_set_ep0_in_res(USBFSD_RES_NAK);
		}
	}

	if(ep == EP_1)
	{
		usbfsd_set_ep1_tx_len(0);
		usbfsd_set_ep1_in_res(USBFSD_RES_NAK);

		hid_mouse_report->x_rel = 0;
		hid_mouse_report->y_rel = 0;
		hid_mouse_report->scroll_rel = 0;
		hid_report_pending = 0;
	}
}

void hid_on_setup(uint8_t ep)
{
	uint8_t len;
	uint8_t idx;

	if(ep == EP_0)
	{
		hid_setup_len = hid_setup_buf->wLength;
		hid_setup_req = hid_setup_buf->bRequest;
		hid_setup_type = hid_setup_buf->bRequestType;
		descriptor_ptr = 0;
		len = 0;

		if((hid_setup_type & USB_REQ_TYP_MASK) == USB_REQ_TYP_STANDARD)
		{
			switch(hid_setup_req)
			{
			case USB_GET_STATUS:
				ep0_buffer[0] = 0x00;
				ep0_buffer[1] = 0x00;
				len = 2;
				break;
			case USB_CLEAR_FEATURE:
				if((hid_setup_type & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP)
				{
					switch((uint8_t)(hid_setup_buf->wIndex))
					{
					case 0x01:
						usbfsd_set_ep1_out_toggle(USBFSD_TOG_0);
						usbfsd_set_ep1_out_res(USBFSD_RES_ACK);
						break;
					case 0x81:
						usbfsd_set_ep1_in_toggle(USBFSD_TOG_0);
						usbfsd_set_ep1_in_res(USBFSD_RES_NAK);
						break;
					default:
						len = 0xFF;
						break;
					}
				}
				else
				{
					len = 0xFF;
				}
				break;
			case USB_SET_FEATURE:
				if(((hid_setup_type & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) && !(hid_setup_buf->wIndex & 0xFF00))
				{
					switch((uint8_t)(hid_setup_buf->wIndex))
					{
					case 0x01:
						usbfsd_set_ep1_out_toggle(USBFSD_TOG_0);
						usbfsd_set_ep1_out_res(USBFSD_RES_STALL);
						break;
					case 0x81:
						usbfsd_set_ep1_in_toggle(USBFSD_TOG_0);
						usbfsd_set_ep1_in_res(USBFSD_RES_STALL);
						break;
					default:
						len = 0xFF;
						break;
					}
				}
				else
				{
					len = 0xFF;
				}
				break;
			case USB_SET_ADDRESS:
				hid_address = (uint8_t)(hid_setup_buf->wValue);
				break;
			case USB_GET_DESCRIPTOR:
				switch((uint8_t)(hid_setup_buf->wValue >> 8))
				{
				case USB_DESCR_TYP_DEVICE:
					descriptor_ptr = hid_device_descriptor;
					len = HID_DEV_DESCR_SIZE;
					break;
				case USB_DESCR_TYP_CONFIG:
					descriptor_ptr = hid_config_descriptor;
					len = HID_CONF_DESCR_SIZE;
					break;
				case USB_DESCR_TYP_STRING:
					switch((uint8_t)(hid_setup_buf->wValue))
					{
					case 0:
						descriptor_ptr = hid_string_lang_id;
						break;
					case 1:
						descriptor_ptr = hid_string_vendor;
						break;
					case 2:
						descriptor_ptr = hid_string_product;
						break;
					case 3:
						descriptor_ptr = hid_string_serial;
						break;
					case 4:
						descriptor_ptr = hid_string_product;
						break;
					default:
						descriptor_ptr = hid_string_serial;
						break;
					}
					len = descriptor_ptr[0];
					break;
				case USB_DESCR_TYP_REPORT:
					if((uint8_t)(hid_setup_buf->wValue) == 0)
					{
						descriptor_ptr = hid_report_descriptor;
						len = HID_REPORT_DESCR_SIZE;
					}
					else
						len = 0xFF;
					break;
				default:
					len = 0xFF;
					break;
				}
				break;
			case USB_GET_CONFIGURATION:
				ep0_buffer[0] = hid_config;
				len = 1;
				break;
			case USB_SET_CONFIGURATION:
				hid_config = (uint8_t)(hid_setup_buf->wValue);
				break;
			case USB_GET_INTERFACE:
				ep0_buffer[0] = 0;
				len = 1;
				break;
			case USB_SET_INTERFACE:
				//nothing to do here
				break;
			default:
				len = 0xFF;
				break;
			}
		}
		else if((hid_setup_type & USB_REQ_TYP_MASK) == USB_REQ_TYP_CLASS)
		{
			switch(hid_setup_buf->bRequest)
			{
			case HID_GET_REPORT:
				for(idx = 0; idx < HID_MOUSE_REPORT_SIZE; ++idx)
				{
					ep0_buffer[idx] = ep1_buffer[idx];	//the report does not have its own buffer, it stays in ep1
				}
				len = HID_MOUSE_REPORT_SIZE;
				break;
			case HID_GET_IDLE:
				ep0_buffer[0] = hid_idle_rate;
				len = 1;
				break;
			case HID_GET_PROTOCOL:
				ep0_buffer[0] = hid_protocol;
				len = 1;
				break;
			case HID_SET_REPORT:
				// nothing to do here
				break;
			case HID_SET_IDLE:
				hid_idle_rate = (uint8_t)(hid_setup_buf->wValue >> 8);
				break;
			case HID_SET_PROTOCOL:
				hid_protocol = (uint8_t)(hid_setup_buf->wValue);
				break;
			default:
				len = 0xFF;
				break;
			}
		}
		else
		{
			len = 0xFF;
		}	// end  if((hid_setup_type & USB_REQ_TYP_MASK) == USB_REQ_TYP_STANDARD)

		if(len == 0xFF)
		{
			hid_setup_req = 0xFF;
			usbfsd_set_ep0_out_tog_res(USBFSD_TOG_1 | USBFSD_RES_STALL);
			usbfsd_set_ep0_in_tog_res(USBFSD_TOG_1 | USBFSD_RES_STALL);
		}
		else
		{
			if(hid_setup_len > len)
				hid_setup_len = len;
			len = (hid_setup_len > HID_ENDP0_SIZE) ? HID_ENDP0_SIZE : hid_setup_len;

			if(descriptor_ptr)
				hid_copy_descriptor(len);

			hid_setup_len -= len;
			usbfsd_set_ep0_tx_len(len);
			usbfsd_set_ep0_out_tog_res(USBFSD_TOG_1 | USBFSD_RES_ACK);
			usbfsd_set_ep0_in_tog_res(USBFSD_TOG_1 | USBFSD_RES_ACK);
		}
	}	// end if(ep == EP_0)
}

void hid_on_rst(void)
{
	usbfsd_set_ep0_out_tog_res(USBFSD_TOG_0 | USBFSD_RES_ACK | USBFSD_AUTOTOG_0);
	usbfsd_set_ep0_in_tog_res(USBFSD_TOG_0 | USBFSD_RES_NAK | USBFSD_AUTOTOG_0);
	usbfsd_set_ep1_out_tog_res(USBFSD_TOG_0 | USBFSD_RES_ACK | USBFSD_AUTOTOG_1);
	usbfsd_set_ep1_in_tog_res(USBFSD_TOG_0 | USBFSD_RES_NAK | USBFSD_AUTOTOG_1);

	hid_mouse_report->buttons = 0;
	hid_mouse_report->x_rel = 0;
	hid_mouse_report->y_rel = 0;
	hid_mouse_report->scroll_rel = 0;
	hid_report_pending = 0;
	hid_idle_rate = 0;
	hid_protocol = 0x01;	//default to report protocol
}

static const usbfsd_config_t usbfsd_config =
{
	(uint32_t)ep0_buffer,
	(uint32_t)ep1_buffer,
	0,
	0,
	0,
	0,
	0,
	0,
	USBFSD_TOG_0 | USBFSD_RES_NAK | USBFSD_AUTOTOG_0,	//EP_0 IN
	USBFSD_TOG_0 | USBFSD_RES_ACK | USBFSD_AUTOTOG_0,	//EP_0 OUT
	USBFSD_TOG_0 | USBFSD_RES_NAK | USBFSD_AUTOTOG_1,	//EP_1 IN
	USBFSD_TOG_0 | USBFSD_RES_ACK | USBFSD_AUTOTOG_1,	//EP_1 OUT
	USBFSD_TOG_0 | USBFSD_RES_NAK | USBFSD_AUTOTOG_0,	//EP_2 IN
	USBFSD_TOG_0 | USBFSD_RES_ACK | USBFSD_AUTOTOG_0,	//EP_2 OUT
	USBFSD_TOG_0 | USBFSD_RES_NAK | USBFSD_AUTOTOG_0,	//EP_3 IN
	USBFSD_TOG_0 | USBFSD_RES_ACK | USBFSD_AUTOTOG_0,	//EP_3 OUT
	USBFSD_TOG_0 | USBFSD_RES_NAK | USBFSD_AUTOTOG_0,	//EP_4 IN
	USBFSD_TOG_0 | USBFSD_RES_ACK | USBFSD_AUTOTOG_0,	//EP_4 OUT
	USBFSD_TOG_0 | USBFSD_RES_NAK | USBFSD_AUTOTOG_0,	//EP_5 IN
	USBFSD_TOG_0 | USBFSD_RES_ACK | USBFSD_AUTOTOG_0,	//EP_5 OUT
	USBFSD_TOG_0 | USBFSD_RES_NAK | USBFSD_AUTOTOG_0,	//EP_6 IN
	USBFSD_TOG_0 | USBFSD_RES_ACK | USBFSD_AUTOTOG_0,	//EP_6 OUT
	USBFSD_TOG_0 | USBFSD_RES_NAK | USBFSD_AUTOTOG_0,	//EP_7 IN
	USBFSD_TOG_0 | USBFSD_RES_ACK | USBFSD_AUTOTOG_0,	//EP_7 OUT
	USBFSD_EP1_TX_EN | USBFSD_EP1_BUF_SINGLE | USBFSD_EP4_BUF_SINGLE,
	USBFSD_EP2_BUF_SINGLE | USBFSD_EP3_BUF_SINGLE,
	USBFSD_EP5_BUF_SINGLE | USBFSD_EP6_BUF_SINGLE,
	USBFSD_EP7_BUF_SINGLE,
	USBFSD_INT_TRANSFER | USBFSD_INT_RESET
};

void hid_init(void)
{
	usbfsd_sof_callback = 0;
	usbfsd_out_callback = hid_on_out;
	usbfsd_in_callback = hid_on_in;
	usbfsd_setup_callback = hid_on_setup;
	usbfsd_reset_callback = hid_on_rst;
	usbfsd_wakeup_callback = 0;
	usbfsd_suspend_callback = 0;

	usbfsd_init(&usbfsd_config);
}

void hid_mouse_send_report(void)
{
	hid_report_pending = 1;
	usbfsd_set_ep1_tx_len(HID_MOUSE_REPORT_SIZE);
	usbfsd_set_ep1_in_res(USBFSD_RES_ACK);
}

void hid_mouse_press(uint8_t buttons)
{
	hid_mouse_report->buttons |= buttons;
	hid_mouse_send_report();
}

void hid_mouse_release(uint8_t buttons)
{
	hid_mouse_report->buttons &= ~buttons;
	hid_mouse_send_report();
}

void hid_mouse_move(uint8_t x_rel, uint8_t y_rel)
{
	hid_mouse_report->x_rel += x_rel;
	hid_mouse_report->y_rel += y_rel;
	hid_mouse_send_report();
}

void hid_mouse_scroll(uint8_t scroll_rel)
{
	hid_mouse_report->scroll_rel += scroll_rel;
	hid_mouse_send_report();
}
