/*
 * ch32v203_usbfsd_cdc.c
 *
 *  Created on: Aug 27, 2024
 *      Author: Steve
 */
#include "ch32v20x.h"
#include "ch32v203_usbfsd.h"
#include "ch32v203_usbfsd_cdc.h"
//TODO: what happens if EP_2 receives a zero length packet?

#define SET_LINE_CODING         0x20  // host configures line coding
#define GET_LINE_CODING         0x21  // host reads configured line coding
#define SET_CONTROL_LINE_STATE  0x22  // generates RS-232/V.24 style control signals
#define SEND_BREAK              0x23  // send break

#define CDC_DEV_DESCR_SIZE 18
#define CDC_CONF_DESCR_SIZE 67
#define CDC_LINE_CODING_SIZE 7
#define CDC_SERIAL_STATE_SIZE 10

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

typedef struct _USBFSD_DBL_BUF
{
	uint8_t t0_buf[64];
	uint8_t t1_buf[64];
} usbfsd_dbl_buf_t;

uint16_t cdc_last_data_time;
uint16_t cdc_last_status_time;
volatile uint8_t cdc_tx_enabled;
volatile uint8_t cdc_rx_enabled;

volatile uint8_t ep0_buffer[CDC_ENDP0_BUF_SIZE] __attribute__ ((aligned(4)));
volatile uint8_t ep1_buffer[CDC_ENDP1_SIZE] __attribute__ ((aligned(4)));
volatile usbfsd_dbl_buf_t ep2_buffer __attribute__ ((aligned(4)));
volatile usbfsd_dbl_buf_t ep3_buffer __attribute__ ((aligned(4)));

uint8_t ep2_read_select;
volatile uint8_t ep2_t0_num_bytes;
uint8_t ep2_t0_read_offset;
volatile uint8_t ep2_t1_num_bytes;
uint8_t ep2_t1_read_offset;

volatile uint8_t ep3_wip;
volatile uint8_t ep3_write_select;
volatile uint8_t ep3_t0_num_bytes;
volatile uint8_t ep3_t1_num_bytes;

#define cdc_setup_buf ((usb_setup_req_t*)ep0_buffer)
#define CDC_ENDP2_SIZE 64
#define CDC_ENDP3_SIZE 64

uint8_t cdc_setup_req;
uint8_t cdc_setup_type;
uint16_t cdc_setup_len;

const uint8_t* descriptor_ptr;
uint8_t cdc_address;
uint8_t cdc_config;

volatile cdc_line_coding_t cdc_line_coding = {125000, 0x00, 0x00, 0x08};	//125K baud, 1 stop, no parity, 8 data
volatile uint8_t cdc_control_line_state = 0;

/* USB Device Descriptors */
static const uint8_t cdc_device_descriptor[] __attribute__ ((aligned(2))) =
{
    CDC_DEV_DESCR_SIZE,				// bLength
    0x01,                           // bDescriptorType
    0x10, 0x01,                     // bcdUSB
    0x02,                           // bDeviceClass
    0x00,                           // bDeviceSubClass
    0x00,                           // bDeviceProtocol
    CDC_ENDP0_SIZE,					// bMaxPacketSize0
    0x86, 0x1A,                     // idVendor
    0x0C, 0xFE,                     // idProduct
    0x00, 0x01,                     // bcdDevice
    0x01,                           // iManufacturer
    0x02,                           // iProduct
    0x03,                           // iSerialNumber
    0x01,                           // bNumConfigurations
};

/* USB Configuration Descriptors */
static const uint8_t cdc_config_descriptor[] __attribute__ ((aligned(2))) =
{
    /* Configuration Descriptor */
    0x09,                           // bLength
    0x02,                           // bDescriptorType
    CDC_CONF_DESCR_SIZE & 0xFF, CDC_CONF_DESCR_SIZE >> 8, // wTotalLength
    0x02,                           // bNumInterfaces
    0x01,                           // bConfigurationValue
    0x00,                           // iConfiguration
    0x80,                           // bmAttributes: Bus Powered
    0x32,                           // MaxPower: 100mA

    /* Interface 0 (CDC) descriptor */
    0x09,                           // bLength
    0x04,                           // bDescriptorType (Interface)
    0x00,                           // bInterfaceNumber 0
    0x00,                           // bAlternateSetting
    0x01,                           // bNumEndpoints 1
    0x02,                           // bInterfaceClass	(Communications Interface Class)
    0x02,                           // bInterfaceSubClass	 (Abstract Control Model)
    0x01,                           // bInterfaceProtocol	(AT Commands: V.250)
    0x04,                           // iInterface (String Index)

    /* Functional Descriptors */
    0x05, 0x24, 0x00, 0x10, 0x01,	//CDC Header Functional Descriptor (CDC 1.2)

    /* Length/management descriptor */
    0x05, 0x24, 0x01, 0x00, 0x01,	//Call Management Functional Descriptor (interface 1)
    0x04, 0x24, 0x02, 0x02,			//Abstract Control Management Functional Descriptor
    0x05, 0x24, 0x06, 0x00, 0x01,	//Union Functional Descriptor (interface 0, interface 1)

    /* Interrupt upload endpoint descriptor */
    0x07,                           // bLength
    0x05,                           // bDescriptorType (Endpoint)
    0x81,                           // bEndpointAddress (IN/D2H)
    0x03,                           // bmAttributes (Interrupt)
    CDC_ENDP1_SIZE & 0xFF, CDC_ENDP1_SIZE >> 8, // wMaxPacketSize
    0x01,                           // bInterval 1 (unit depends on device speed)

    /* Interface 1 (data interface) descriptor */
    0x09,                           // bLength
    0x04,                           // bDescriptorType (Interface)
    0x01,                           // bInterfaceNumber 1
    0x00,                           // bAlternateSetting
    0x02,                           // bNumEndpoints 2
    0x0A,                           // bInterfaceClass	(Data Interface Class)
    0x00,                           // bInterfaceSubClass
    0x00,                           // bInterfaceProtocol
    0x00,                           // iInterface (String Index)

    /* Endpoint descriptor */
    0x07,                           // bLength
    0x05,                           // bDescriptorType (Endpoint)
    0x02,                           // bEndpointAddress (OUT/H2D)
    0x02,                           // bmAttributes (Bulk)
    CDC_ENDP2_SIZE & 0xFF, CDC_ENDP2_SIZE >> 8, // wMaxPacketSize
    0x00,                           // bInterval 0 (unit depends on device speed)

    /* Endpoint descriptor */
    0x07,                           // bLength
    0x05,                           // bDescriptorType (Endpoint)
    0x83,                           // bEndpointAddress (IN/D2H)
    0x02,                           // bmAttributes (Bulk)
    CDC_ENDP3_SIZE & 0xFF, CDC_ENDP3_SIZE >> 8, // wMaxPacketSize
    0x00                            // bInterval 0 (unit depends on device speed)
};

/* USB String Descriptors */
static const uint8_t cdc_string_lang_id[] __attribute__ ((aligned(2))) =
{
	0x04,
	0x03,
	0x09,
	0x04
};

/* USB Device String Vendor */
static const uint8_t cdc_string_vendor[] __attribute__ ((aligned(2))) =
{
	14,
	0x03,
	'w',0,'c',0,'h',0,'.',0,'c',0,'n',0
};

/* USB Device String Product */
static const uint8_t cdc_string_product[] __attribute__ ((aligned(2))) =
{
    22,
    0x03,
    'U', 0, 'S', 0, 'B', 0, ' ', 0, 'S', 0, 'e', 0, 'r', 0, 'i', 0, 'a', 0, 'l', 0
};

/* USB Device String Serial */
static const uint8_t cdc_string_serial[] __attribute__ ((aligned(2))) =
{
	22,
	0x03,
	'D', 0, 'E', 0, 'A', 0, 'D', 0, 'B', 0, 'E', 0 , 'E', 0, 'F', 0, '0', 0, '1', 0
};

static const uint8_t cdc_serial_state[] __attribute__ ((aligned(2))) = {0xA1, 0x20, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};

void cdc_copy_descriptor(uint8_t len)
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

// HINT: This library contains some nasty hacks to work around bugs in CH32V203.
// Enabling SOF interrupts makes interrupt transfers fail, since the SOF triggers UIF_TRANSFER (all enpoints respond with NAK while UIF_TRANSFER is set).
// When a notification needs to be sent the SOF interrupts are disabled so that the interrupt transfer can happen.
// SOF interrupts are re-enabled after the interrupt transfer completes.
void cdc_on_sof(void)
{
	// check for timeout, if the buffer is not being written to then enable transmitting and mark the buffer as full
	if(((uint16_t)(usbfsd_sof_count - cdc_last_data_time) >= (uint16_t)CDC_TIMEOUT_MS) && !(ep3_wip | cdc_tx_enabled))
	{
		if(ep3_write_select)
		{
			usbfsd_set_ep3_tx_len(ep3_t1_num_bytes);
			ep3_t1_num_bytes = CDC_ENDP3_SIZE;
			ep3_write_select = 0;
		}
		else
		{
			usbfsd_set_ep3_tx_len(ep3_t0_num_bytes);
			ep3_t0_num_bytes = CDC_ENDP3_SIZE;
			ep3_write_select = 1;
		}
		usbfsd_set_ep3_in_res(USBFSD_RES_ACK);
		cdc_tx_enabled = 1;
	}

	if((uint16_t)(usbfsd_sof_count - cdc_last_status_time) >= (uint16_t)CDC_TIMEOUT_MS)
	{
		usbfsd_set_ep1_tx_len(CDC_SERIAL_STATE_SIZE);
		usbfsd_set_ep1_in_res(USBFSD_RES_ACK);
		usbfsd_disable_interrupts(USBFSD_INT_SOF);
	}
}

void cdc_on_out(uint8_t ep)
{
	uint8_t idx;

	if(ep == EP_0)
	{
		if((cdc_setup_req == SET_LINE_CODING) && ((cdc_setup_type & USB_REQ_TYP_MASK) == USB_REQ_TYP_CLASS))
		{
			for(idx = 0; idx < CDC_LINE_CODING_SIZE; idx++)
			{
				((uint8_t*)&cdc_line_coding)[idx] = ep0_buffer[idx];
			}
		}
		usbfsd_set_ep0_tx_len(0);
		usbfsd_set_ep0_out_tog_res(USBFSD_TOG_1 | USBFSD_RES_ACK);
		usbfsd_set_ep0_in_tog_res(USBFSD_TOG_1 | USBFSD_RES_ACK);
	}

	if(ep == EP_2)
	{
		//auto-toggle is enabled, so here the toggle should point to the next buffer that will receive
		if(usbfsd_get_ep2_out_toggle())
		{
			ep2_t0_num_bytes = usbfsd_get_rx_len();
			if(ep2_t1_num_bytes)
			{
				usbfsd_set_ep2_out_res(USBFSD_RES_NAK);
				cdc_rx_enabled = 0;
			}
		}
		else
		{
			ep2_t1_num_bytes = usbfsd_get_rx_len();
			if(ep2_t0_num_bytes)
			{
				usbfsd_set_ep2_out_res(USBFSD_RES_NAK);
				cdc_rx_enabled = 0;
			}
		}
	}
}

void cdc_on_in(uint8_t ep)
{
	uint8_t len;

	if(ep == EP_0)
	{
		if((cdc_setup_type & USB_REQ_TYP_MASK) == USB_REQ_TYP_STANDARD)
		{
			switch(cdc_setup_req)
			{
			case USB_GET_DESCRIPTOR:
				len = cdc_setup_len >= CDC_ENDP0_SIZE ? CDC_ENDP0_SIZE : cdc_setup_len;
				cdc_copy_descriptor(len);
				cdc_setup_len -= len;
				usbfsd_set_ep0_tx_len(len);
				usbfsd_toggle_ep0_in_toggle();
				break;
			case USB_SET_ADDRESS:
				usbfsd_set_addr(cdc_address);
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
		cdc_last_status_time = usbfsd_sof_count;
		usbfsd_enable_interrupts(USBFSD_INT_SOF);
		++usbfsd_sof_count;
	}

	if(ep == EP_3)
	{
		cdc_last_data_time = usbfsd_sof_count;
		usbfsd_set_ep3_tx_len(CDC_ENDP3_SIZE);
		//auto-toggle is enabled, so here the toggle should point to the next buffer that will transmit
		if(usbfsd_get_ep3_in_toggle())
		{
			ep3_t0_num_bytes = 0;
			if(ep3_t1_num_bytes != CDC_ENDP3_SIZE)
			{
				usbfsd_set_ep3_tx_len(0);
				usbfsd_set_ep3_in_res(USBFSD_RES_NAK);
				cdc_tx_enabled = 0;
			}
		}
		else
		{
			ep3_t1_num_bytes = 0;
			if(ep3_t0_num_bytes != CDC_ENDP3_SIZE)
			{
				usbfsd_set_ep3_tx_len(0);
				usbfsd_set_ep3_in_res(USBFSD_RES_NAK);
				cdc_tx_enabled = 0;
			}
		}
	}
}

void cdc_on_setup(uint8_t ep)
{
	uint8_t len;
	uint8_t idx;

	if(ep == EP_0)
	{
		cdc_setup_len = cdc_setup_buf->wLength;
		cdc_setup_req = cdc_setup_buf->bRequest;
		cdc_setup_type = cdc_setup_buf->bRequestType;
		descriptor_ptr = 0;
		len = 0;

		if((cdc_setup_type & USB_REQ_TYP_MASK) == USB_REQ_TYP_STANDARD)
		{
			switch(cdc_setup_req)
			{
			case USB_GET_STATUS:
				ep0_buffer[0] = 0x00;
				ep0_buffer[1] = 0x00;
				len = 2;
				break;
			case USB_CLEAR_FEATURE:
				if((cdc_setup_type & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP)
				{
					switch((uint8_t)(cdc_setup_buf->wIndex))
					{
					case 0x81:
						usbfsd_set_ep1_in_toggle(USBFSD_TOG_0);
						usbfsd_set_ep1_in_res(USBFSD_RES_NAK);
						usbfsd_enable_interrupts(USBFSD_INT_SOF);	//SOF interrupts must be enabled when EP1 is not ready
						break;
					case 0x02:
						usbfsd_set_ep2_out_toggle(USBFSD_TOG_0);
						usbfsd_set_ep2_out_res(USBFSD_RES_ACK);
						break;
					case 0x83:
						usbfsd_set_ep3_in_toggle(USBFSD_TOG_0);
						usbfsd_set_ep3_in_res(USBFSD_RES_NAK);
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
				if(((cdc_setup_type & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) && !(cdc_setup_buf->wIndex & 0xFF00))
				{
					switch((uint8_t)(cdc_setup_buf->wIndex))
					{
					case 0x81:
						usbfsd_set_ep1_in_toggle(USBFSD_TOG_0);
						usbfsd_set_ep1_in_res(USBFSD_RES_STALL);
						usbfsd_enable_interrupts(USBFSD_INT_SOF);	//SOF interrupts must be enabled when EP1 is not ready
						break;
					case 0x02:
						usbfsd_set_ep2_out_toggle(USBFSD_TOG_0);
						usbfsd_set_ep2_out_res(USBFSD_RES_STALL);
						break;
					case 0x83:
						usbfsd_set_ep3_in_toggle(USBFSD_TOG_0);
						usbfsd_set_ep3_in_res(USBFSD_RES_STALL);
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
				cdc_address = (uint8_t)(cdc_setup_buf->wValue);
				break;
			case USB_GET_DESCRIPTOR:
				switch((uint8_t)(cdc_setup_buf->wValue >> 8))
				{
				case USB_DESCR_TYP_DEVICE:
					descriptor_ptr = cdc_device_descriptor;
					len = CDC_DEV_DESCR_SIZE;
					break;
				case USB_DESCR_TYP_CONFIG:
					descriptor_ptr = cdc_config_descriptor;
					len = CDC_CONF_DESCR_SIZE;
					break;
				case USB_DESCR_TYP_STRING:
					switch((uint8_t)(cdc_setup_buf->wValue))
					{
					case 0:
						descriptor_ptr = cdc_string_lang_id;
						break;
					case 1:
						descriptor_ptr = cdc_string_vendor;
						break;
					case 3:
						descriptor_ptr = cdc_string_serial;
						break;
					default:
						descriptor_ptr = cdc_string_product;
						break;
					}
					len = descriptor_ptr[0];
					break;
				default:
					len = 0xFF;
					break;
				}
				break;
			case USB_GET_CONFIGURATION:
				ep0_buffer[0] = cdc_config;
				len = 1;
				break;
			case USB_SET_CONFIGURATION:
				cdc_config = (uint8_t)(cdc_setup_buf->wValue);
				usbfsd_enable_interrupts(USBFSD_INT_SOF);
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
		else if((cdc_setup_type & USB_REQ_TYP_MASK) == USB_REQ_TYP_CLASS)
		{
			switch(cdc_setup_req)
			{
			case GET_LINE_CODING:
				for(idx = 0; idx < CDC_LINE_CODING_SIZE; ++idx)
				{
					ep0_buffer[idx] = ((uint8_t*)&cdc_line_coding)[idx];
				}
				len = CDC_LINE_CODING_SIZE;
				break;
			case SET_CONTROL_LINE_STATE:
				cdc_control_line_state = ep0_buffer[2];
				break;
			case SET_LINE_CODING:
				// nothing to do here
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

		if(len == 0xFF)
		{
			cdc_setup_req = 0xFF;
			usbfsd_set_ep0_out_tog_res(USBFSD_TOG_1 | USBFSD_RES_STALL);
			usbfsd_set_ep0_in_tog_res(USBFSD_TOG_1 | USBFSD_RES_STALL);
		}
		else
		{
			if(cdc_setup_len > len)
				cdc_setup_len = len;
			len = (cdc_setup_len > CDC_ENDP0_SIZE) ? CDC_ENDP0_SIZE : cdc_setup_len;

			if(descriptor_ptr)
				cdc_copy_descriptor(len);

			cdc_setup_len -= len;
			usbfsd_set_ep0_tx_len(len);
			usbfsd_set_ep0_out_tog_res(USBFSD_TOG_1 | USBFSD_RES_ACK);
			usbfsd_set_ep0_in_tog_res(USBFSD_TOG_1 | USBFSD_RES_ACK);
		}
	}	//end if(ep == EP_0)
}

void cdc_on_rst(void)
{
	usbfsd_disable_interrupts(USBFSD_INT_SOF);

	ep2_read_select = 0;
	ep2_t0_num_bytes = 0;
	ep2_t0_read_offset = 0;
	ep2_t1_num_bytes = 0;
	ep2_t1_read_offset = 0;

	ep3_wip = 0;
	ep3_write_select = 0;
	ep3_t0_num_bytes = 0;
	ep3_t1_num_bytes = 0;

	usbfsd_set_ep0_out_tog_res(USBFSD_TOG_0 | USBFSD_RES_ACK | USBFSD_AUTOTOG_0);
	usbfsd_set_ep0_in_tog_res(USBFSD_TOG_0 | USBFSD_RES_NAK | USBFSD_AUTOTOG_0);
	usbfsd_set_ep1_out_tog_res(USBFSD_TOG_0 | USBFSD_RES_ACK | USBFSD_AUTOTOG_1);
	usbfsd_set_ep1_in_tog_res(USBFSD_TOG_0 | USBFSD_RES_NAK | USBFSD_AUTOTOG_1);
	usbfsd_set_ep2_out_tog_res(USBFSD_TOG_0 | USBFSD_RES_ACK | USBFSD_AUTOTOG_1);
	usbfsd_set_ep2_in_tog_res(USBFSD_TOG_0 | USBFSD_RES_NAK | USBFSD_AUTOTOG_1);
	usbfsd_set_ep3_out_tog_res(USBFSD_TOG_0 | USBFSD_RES_ACK | USBFSD_AUTOTOG_1);
	usbfsd_set_ep3_in_tog_res(USBFSD_TOG_0 | USBFSD_RES_NAK | USBFSD_AUTOTOG_1);

	cdc_last_data_time = 0;
	cdc_last_status_time = 0;
	usbfsd_sof_count = 0;
	cdc_tx_enabled = 0;
	cdc_rx_enabled = 1;
}

void cdc_enable_tx(void)
{
	// check toggle, if the buffer pointed to by the toggle is completely full then enable transmitting
	if(!cdc_tx_enabled && ((usbfsd_get_ep3_in_toggle() ? ep3_t1_num_bytes : ep3_t0_num_bytes) == CDC_ENDP3_SIZE))
	{
		usbfsd_set_ep3_tx_len(CDC_ENDP3_SIZE);
		usbfsd_set_ep3_in_res(USBFSD_RES_ACK);
		cdc_tx_enabled = 1;
	}
}

void cdc_enable_rx(void)
{
	// check toggle, if the buffer pointed to by the toggle is empty then enable receiving
	if(!cdc_rx_enabled && ((usbfsd_get_ep2_out_toggle() ? ep2_t1_num_bytes : ep2_t0_num_bytes) == 0))
	{
		usbfsd_set_ep2_out_res(USBFSD_RES_ACK);
		cdc_rx_enabled = 1;
	}
}

static const usbfsd_config_t usbfsd_config =
{
	(uint32_t)ep0_buffer,
	(uint32_t)ep1_buffer,
	(uint32_t)(&ep2_buffer),
	(uint32_t)(&ep3_buffer),
	0,
	0,
	0,
	0,
	USBFSD_TOG_0 | USBFSD_RES_NAK | USBFSD_AUTOTOG_0,	//EP_0 IN
	USBFSD_TOG_0 | USBFSD_RES_ACK | USBFSD_AUTOTOG_0,	//EP_0 OUT
	USBFSD_TOG_0 | USBFSD_RES_NAK | USBFSD_AUTOTOG_1,	//EP_1 IN
	USBFSD_TOG_0 | USBFSD_RES_ACK | USBFSD_AUTOTOG_1,	//EP_1 OUT
	USBFSD_TOG_0 | USBFSD_RES_NAK | USBFSD_AUTOTOG_1,	//EP_2 IN
	USBFSD_TOG_0 | USBFSD_RES_ACK | USBFSD_AUTOTOG_1,	//EP_2 OUT
	USBFSD_TOG_0 | USBFSD_RES_NAK | USBFSD_AUTOTOG_1,	//EP_3 IN
	USBFSD_TOG_0 | USBFSD_RES_ACK | USBFSD_AUTOTOG_1,	//EP_3 OUT
	USBFSD_TOG_0 | USBFSD_RES_NAK | USBFSD_AUTOTOG_0,	//EP_4 IN
	USBFSD_TOG_0 | USBFSD_RES_ACK | USBFSD_AUTOTOG_0,	//EP_4 OUT
	USBFSD_TOG_0 | USBFSD_RES_NAK | USBFSD_AUTOTOG_0,	//EP_5 IN
	USBFSD_TOG_0 | USBFSD_RES_ACK | USBFSD_AUTOTOG_0,	//EP_5 OUT
	USBFSD_TOG_0 | USBFSD_RES_NAK | USBFSD_AUTOTOG_0,	//EP_6 IN
	USBFSD_TOG_0 | USBFSD_RES_ACK | USBFSD_AUTOTOG_0,	//EP_6 OUT
	USBFSD_TOG_0 | USBFSD_RES_NAK | USBFSD_AUTOTOG_0,	//EP_7 IN
	USBFSD_TOG_0 | USBFSD_RES_ACK | USBFSD_AUTOTOG_0,	//EP_7 OUT
	USBFSD_EP1_TX_EN | USBFSD_EP1_BUF_SINGLE | USBFSD_EP4_BUF_SINGLE,
	USBFSD_EP2_RX_EN | USBFSD_EP2_BUF_DOUBLE | USBFSD_EP3_TX_EN | USBFSD_EP3_BUF_DOUBLE,
	USBFSD_EP5_BUF_SINGLE | USBFSD_EP6_BUF_SINGLE,
	USBFSD_EP7_BUF_SINGLE,
	USBFSD_INT_TRANSFER | USBFSD_INT_RESET
};

void cdc_init(void)
{
	usbfsd_sof_callback = cdc_on_sof;
	usbfsd_out_callback = cdc_on_out;
	usbfsd_in_callback = cdc_on_in;
	usbfsd_setup_callback = cdc_on_setup;
	usbfsd_reset_callback = cdc_on_rst;
	usbfsd_wakeup_callback = 0;
	usbfsd_suspend_callback = 0;

	usbfsd_init(&usbfsd_config);
	cdc_set_serial_state(0x00);
}

void cdc_set_serial_state(uint8_t val)
{
	uint8_t idx;
	for(idx = 0; idx < CDC_SERIAL_STATE_SIZE; ++idx)
	{
		ep1_buffer[idx] = cdc_serial_state[idx];
	}
	ep1_buffer[8] = val;
	usbfsd_set_ep1_tx_len(CDC_SERIAL_STATE_SIZE);
	usbfsd_set_ep1_in_res(USBFSD_RES_ACK);
}

// Receive
uint16_t cdc_bytes_available(void)
{
	// count the number of bytes available in both buffers
	return (ep2_t0_num_bytes - ep2_t0_read_offset) + (ep2_t1_num_bytes - ep2_t1_read_offset);
}

uint8_t cdc_peek(void)
{
	if(ep2_read_select)
		return ep2_buffer.t1_buf[ep2_t1_read_offset];
	else
		return ep2_buffer.t0_buf[ep2_t0_read_offset];
}

uint8_t cdc_read_byte(void)
{
	uint8_t read_val;

	while(!cdc_bytes_available());

	if(ep2_read_select)
	{
		read_val = ep2_buffer.t1_buf[ep2_t1_read_offset];
		++ep2_t1_read_offset;
		if(ep2_t1_read_offset == ep2_t1_num_bytes)
		{
			ep2_t1_read_offset = 0;
			ep2_t1_num_bytes = 0;
			ep2_read_select = 0;
			cdc_enable_rx();
		}
	}
	else
	{
		read_val = ep2_buffer.t0_buf[ep2_t0_read_offset];
		++ep2_t0_read_offset;
		if(ep2_t0_read_offset == ep2_t0_num_bytes)
		{
			ep2_t0_read_offset = 0;
			ep2_t0_num_bytes = 0;
			ep2_read_select = 1;
			cdc_enable_rx();
		}
	}

	return read_val;
}

void cdc_read_bytes(uint8_t* dest, uint16_t num_bytes)
{
	uint16_t num_to_read = 0;

	while(num_bytes)
	{
		if(ep2_read_select)
		{
			num_to_read = ep2_t1_num_bytes - ep2_t1_read_offset;
			if(!num_to_read)
				continue;
			if(num_to_read > num_bytes)
			{
				num_to_read = num_bytes;
			}

			num_bytes -= num_to_read;
			do
			{
				*dest = ep2_buffer.t1_buf[ep2_t1_read_offset];
				++dest;
				++ep2_t1_read_offset;
			} while(--num_to_read);

			if(ep2_t1_read_offset == ep2_t1_num_bytes)
			{
				ep2_t1_read_offset = 0;
				ep2_t1_num_bytes = 0;
				ep2_read_select = 0;
				cdc_enable_rx();
			}
		}
		else
		{
			num_to_read = ep2_t0_num_bytes - ep2_t0_read_offset;
			if(!num_to_read)
				continue;
			if(num_to_read > num_bytes)
			{
				num_to_read = num_bytes;
			}

			num_bytes -= num_to_read;
			do
			{
				*dest = ep2_buffer.t0_buf[ep2_t0_read_offset];
				++dest;
				++ep2_t0_read_offset;
			} while(--num_to_read);

			if(ep2_t0_read_offset == ep2_t0_num_bytes)
			{
				ep2_t0_read_offset = 0;
				ep2_t0_num_bytes = 0;
				ep2_read_select = 1;
				cdc_enable_rx();
			}
		}
	}
}

// Send
uint16_t cdc_bytes_available_for_write(void)
{
	return (CDC_ENDP3_SIZE - ep3_t0_num_bytes) + (CDC_ENDP3_SIZE - ep3_t1_num_bytes);
}

void cdc_write_byte(uint8_t val)
{
	uint8_t ep_num_bytes;

	ep3_wip = 1;
	if(ep3_write_select)
	{
		ep_num_bytes = ep3_t1_num_bytes;
		if(ep_num_bytes == CDC_ENDP3_SIZE)
		{
			ep3_wip = 0;
			return;
		}
		ep3_buffer.t1_buf[ep_num_bytes] = val;
		++ep_num_bytes;
		ep3_t1_num_bytes = ep_num_bytes;
		if(ep_num_bytes == CDC_ENDP3_SIZE)
		{
			ep3_write_select = 0;
			cdc_enable_tx();
		}
	}
	else
	{
		ep_num_bytes = ep3_t0_num_bytes;
		if(ep_num_bytes == CDC_ENDP3_SIZE)
		{
			ep3_wip = 0;
			return;
		}
		ep3_buffer.t0_buf[ep_num_bytes] = val;
		++ep_num_bytes;
		ep3_t0_num_bytes = ep_num_bytes;
		if(ep_num_bytes == CDC_ENDP3_SIZE)
		{
			ep3_write_select = 1;
			cdc_enable_tx();
		}
	}
	ep3_wip = 0;
}

void cdc_write_bytes(const uint8_t* src, uint16_t num_bytes)
{
	uint16_t num_to_write = 0;
	uint8_t ep_num_bytes;

	ep3_wip = 1;
	while(num_bytes)
	{
		if(ep3_write_select)
		{
			ep_num_bytes = ep3_t1_num_bytes;
			num_to_write = CDC_ENDP3_SIZE - ep_num_bytes;
			if(!num_to_write)
				continue;
			if(num_to_write > num_bytes)
				num_to_write = num_bytes;

			num_bytes -= num_to_write;
			do
			{
				ep3_buffer.t1_buf[ep_num_bytes] = *src;
				++src;
				++ep_num_bytes;
			} while(--num_to_write);
			ep3_t1_num_bytes = ep_num_bytes;

			if(ep_num_bytes == CDC_ENDP3_SIZE)
			{
				ep3_write_select = 0;
				cdc_enable_tx();
			}
		}
		else
		{
			ep_num_bytes = ep3_t0_num_bytes;
			num_to_write = CDC_ENDP3_SIZE - ep_num_bytes;
			if(!num_to_write)
				continue;
			if(num_to_write > num_bytes)
				num_to_write = num_bytes;

			num_bytes -= num_to_write;
			do
			{
				ep3_buffer.t0_buf[ep_num_bytes] = *src;
				++src;
				++ep_num_bytes;
			} while(--num_to_write);
			ep3_t0_num_bytes = ep_num_bytes;

			if(ep_num_bytes == CDC_ENDP3_SIZE)
			{
				ep3_write_select = 1;
				cdc_enable_tx();
			}
		}
	}
	ep3_wip = 0;
}
