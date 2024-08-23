#include "CH552.H"
#include "CH552_USB.h"
#include "CH552_HID_CC_PAN_MOUSE.h"

#define HID_DEV_DESCR_SIZE 18
#define HID_CONF_DESCR_SIZE 59
#define HID_MOUSE_REPORT_DESCR_SIZE 67
#define HID_CC_REPORT_DESCR_SIZE 25
#define HID_MOUSE_REPORT_SIZE 5
#define HID_CC_REPORT_SIZE 3

UINT8 xdata ep0_buffer[HID_ENDP0_BUF_SIZE];
UINT8 xdata ep1_buffer[HID_ENDP1_BUF_SIZE];
UINT8 xdata ep2_buffer[HID_ENDP2_BUF_SIZE];

#define hid_setup_buf ((PUSB_SETUP_REQ)ep0_buffer)

UINT8 hid_setup_req;
UINT8 hid_setup_type;
UINT16 hid_setup_len;

UINT8 code* descriptor_ptr;
UINT8 hid_address;
UINT8 hid_config;

volatile UINT8 hid_mouse_report_pending;
volatile UINT8 hid_mouse_idle_rate;
UINT8 hid_mouse_protocol;
volatile UINT8 hid_cc_report_pending;
volatile UINT8 hid_cc_idle_rate;
UINT8 hid_cc_protocol;

/* USB Device Descriptors */
UINT8 code hid_device_descriptor[] =
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

/* USB Configration Descriptors */
UINT8 code hid_config_descriptor[] =
{ 
    /* Configuration Descriptor */
    0x09,                           // bLength
    0x02,                           // bDescriptorType
    HID_CONF_DESCR_SIZE & 0xFF, HID_CONF_DESCR_SIZE >> 8, // wTotalLength
    0x02,                           // bNumInterfaces
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
    HID_MOUSE_REPORT_DESCR_SIZE & 0xFF, HID_MOUSE_REPORT_DESCR_SIZE >> 8, // wDescriptorLength

    /* Endpoint Descriptor (Mouse) */
    0x07,                           // bLength
    0x05,                           // bDescriptorType
    0x81,                           // bEndpointAddress: IN Endpoint 1
    0x03,                           // bmAttributes
    HID_ENDP1_SIZE & 0xFF, HID_ENDP1_SIZE >> 8, // wMaxPacketSize
    0x01,                            // bInterval: 1mS
	
	/* Interface Descriptor (Consumer Control)*/
    0x09,                           // bLength
    0x04,                           // bDescriptorType
    0x01,                           // bInterfaceNumber
    0x00,                           // bAlternateSetting
    0x01,                           // bNumEndpoints
    0x03,                           // bInterfaceClass: HID
    0x01,                           // bInterfaceSubClass: boot interface
    0x01,                           // bInterfaceProtocol: Keyboard
    0x05,                           // iInterface

    /* HID Descriptor */
    0x09,                           // bLength
    0x21,                           // bDescriptorType
    0x10, 0x01,                     // bcdHID
    0x00,                           // bCountryCode
    0x01,                           // bNumDescriptors
    0x22,                           // bDescriptorType
    HID_CC_REPORT_DESCR_SIZE & 0xFF, HID_CC_REPORT_DESCR_SIZE >> 8, // wDescriptorLength

    /* Endpoint Descriptor (Mouse) */
    0x07,                           // bLength
    0x05,                           // bDescriptorType
    0x82,                           // bEndpointAddress: IN Endpoint 2
    0x03,                           // bmAttributes
    HID_ENDP2_SIZE & 0xFF, HID_ENDP2_SIZE >> 8, // wMaxPacketSize
    0x01                            // bInterval: 1mS
};

/* HID Report Descriptor (Mouse) */
UINT8 code hid_mouse_report_descriptor[] =
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
	0x05, 0x0c,                     // USAGE PAGE (Consumer Devices)
	0x0a, 0x38, 0x02,               // USAGE (AC Pan)
	0x15, 0x81,                     // LOGICAL_MINIMUM (-127)
	0x25, 0x7f,                     // LOGICAL_MAXIMUM (127)
	0x75, 0x08,                     // REPORT_SIZE (8)
	0x95, 0x01,                     // REPORT_COUNT (1)
	0x81, 0x06,                     // INPUT (Data, Var, Rel)
    0xC0,                           // End Collection
    0xC0                            // End Collection
};

/* HID Report Descriptor (Consumer Control) */
UINT8 code hid_cc_report_descriptor[] =
{
	0x05, 0x0c,           // USAGE_PAGE (Consumer Devices)
	0x09, 0x01,           // USAGE (Consumer Control)
	0xa1, 0x01,           // COLLECTION (Application)
	0x85, 0x01,           //   REPORT_ID (1)
	0x19, 0x00,           //   USAGE_MINIMUM (Unassigned)
	0x2a, 0x3c, 0x02,     //   USAGE_MAXIMUM (AC Format)
	0x15, 0x00,           //   LOGICAL_MINIMUM (0)
	0x26, 0x3c, 0x02,     //   LOGICAL_MAXIMUM (572)
	0x95, 0x01,           //   REPORT_COUNT (1)
	0x75, 0x10,           //   REPORT_SIZE (16)
	0x81, 0x00,           //   INPUT (Data,Var,Abs)
	0xc0                  // END_COLLECTION
};

/* USB Language String Descriptor */
UINT8 code hid_string_lang_id[] =
{
	0x04,
	0x03,
	0x09,
	0x04 
};

/* USB Manufacturer String Descriptor */
UINT8 code hid_string_vendor[] =
{
	14,    
	0x03,           
	'w',0,'c',0,'h',0,'.',0,'c',0,'n',0
};

/* USB Product String Descriptor */
UINT8 code hid_string_product[] =
{
    34,
    0x03,
    'U', 0, 'S', 0, 'B', 0, ' ', 0, 'C', 0, 'C', 0, ' ', 0, 'P', 0, 'a', 0, 'n', 0, ' ', 0, 'M', 0, 'o', 0, 'u', 0, 's', 0, 'e', 0
};

/* USB Serial String Descriptor */
UINT8 code hid_string_serial[] =
{
	22,          
	0x03,                   
	'D', 0, 'E', 0, 'A', 0, 'D', 0, 'B', 0, 'E', 0 , 'E', 0, 'F', 0, '0', 0, '5', 0
};

void hid_copy_descriptor(UINT8 len)
{
	UINT8* dest = ep0_buffer;
	while(len)
	{
		*dest = *descriptor_ptr;
		++dest;
		++descriptor_ptr;
		--len;
	}
}

void hid_on_out(UINT8 ep)
{
	if(ep == EP_0)
	{
		usb_set_ep0_tx_len(0);
		usb_set_ep0_tog_res(EP_OUT_TOG_1 | EP_IN_TOG_1 | USB_OUT_RES_ACK | USB_IN_RES_EXPECT_ACK);
	}
}

void hid_on_in(UINT8 ep)
{
	UINT8 len;
	
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
					usb_set_ep0_tx_len(len);
					usb_toggle_ep0_in_toggle();
					break;
				case USB_SET_ADDRESS:
					usb_set_addr(hid_address);
					usb_set_ep0_in_res(USB_IN_RES_NAK);
					break;
				default:
					usb_set_ep0_tx_len(0);
					usb_set_ep0_in_res(USB_IN_RES_NAK);
					break;
			}
		}
		else
		{
			usb_set_ep0_tx_len(0);
			usb_set_ep0_in_res(USB_IN_RES_NAK);
		}
	}
	
	if(ep == EP_1)
	{
		usb_set_ep1_tx_len(0);
		usb_set_ep1_in_res(USB_IN_RES_NAK);
		
		ep1_buffer[1] = 0;	//clear movement and scrolling
		ep1_buffer[2] = 0;
		ep1_buffer[3] = 0;
		ep1_buffer[4] = 0;
		hid_mouse_report_pending = 0;
	}
	
	if(ep == EP_2)
	{
		usb_set_ep2_tx_len(0);
		usb_set_ep2_in_res(USB_IN_RES_NAK);
		hid_cc_report_pending = 0;
	}
}

void hid_on_setup(UINT8 ep)
{
	UINT8 len;
	UINT8 idx;
	
	if(ep == EP_0)
	{
		hid_setup_len = ((UINT16)hid_setup_buf->wLengthH<<8) | (hid_setup_buf->wLengthL);
		hid_setup_req = hid_setup_buf->bRequest;
		hid_setup_type = hid_setup_buf->bRequestType;
		descriptor_ptr = (UINT8 code*)NULL;
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
					if(((hid_setup_type & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) && !hid_setup_buf->wValueL)
					{
						switch(hid_setup_buf->wIndexL)
						{
							case 0x01:
								usb_set_ep1_out_toggle(EP_OUT_TOG_0);
								usb_set_ep1_out_res(USB_OUT_RES_ACK);
								break;
							case 0x81:
								usb_set_ep1_in_toggle(EP_IN_TOG_0);
								usb_set_ep1_in_res(USB_IN_RES_NAK);
								break;
							case 0x82:
								usb_set_ep2_in_toggle(EP_IN_TOG_0);
								usb_set_ep2_in_res(USB_IN_RES_NAK);
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
					if(((hid_setup_type & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) && !hid_setup_buf->wValueL)
					{
						switch(hid_setup_buf->wIndexL)
						{
							case 0x01:
								usb_set_ep1_out_toggle(EP_OUT_TOG_0);
								usb_set_ep1_out_res(USB_OUT_RES_STALL);
								break;
							case 0x81:
								usb_set_ep1_in_toggle(EP_IN_TOG_0);
								usb_set_ep1_in_res(USB_IN_RES_STALL);
								break;
							case 0x82:
								usb_set_ep2_in_toggle(EP_IN_TOG_0);
								usb_set_ep2_in_res(USB_IN_RES_STALL);
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
					hid_address = hid_setup_buf->wValueL;
					break;
				case USB_GET_DESCRIPTOR:
					switch(hid_setup_buf->wValueH)
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
							switch(hid_setup_buf->wValueL)
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
								default:
									descriptor_ptr = hid_string_product;
									break;
							}
							len = descriptor_ptr[0];
							break;
						case USB_DESCR_TYP_REPORT:
							if(hid_setup_buf->wIndexL == 0)
							{
								//Mouse interface
								descriptor_ptr = hid_mouse_report_descriptor;
								len = HID_MOUSE_REPORT_DESCR_SIZE;
							}
							else
							{
								//Consumer Control interface
								descriptor_ptr = hid_cc_report_descriptor;
								len = HID_CC_REPORT_DESCR_SIZE;
							}
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
					hid_config = hid_setup_buf->wValueL;
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
			switch(hid_setup_req)
			{
				case HID_GET_REPORT:
					if(hid_setup_buf->wIndexL == 0)
					{
						//Mouse interface
						for(idx = 0; idx < HID_MOUSE_REPORT_SIZE; ++idx)
						{
							ep0_buffer[idx] = ep1_buffer[idx];	//the report does not have its own buffer, it stays in ep1
						}
						len = HID_MOUSE_REPORT_SIZE;
					}
					else
					{
						//Consumer Control interface
						for(idx = 0; idx < HID_CC_REPORT_SIZE; ++idx)
						{
							ep0_buffer[idx] = ep2_buffer[idx];	//the report does not have its own buffer, it stays in ep2
						}
						len = HID_CC_REPORT_SIZE;
					}
					break;
				case HID_GET_IDLE:
					if(hid_setup_buf->wIndexL == 0)
					{
						ep0_buffer[0] = hid_mouse_idle_rate;
						len = 1;
					}
					else
					{
						ep0_buffer[0] = hid_cc_idle_rate;
						len = 1;
					}
					break;
				case HID_GET_PROTOCOL:
					if(hid_setup_buf->wIndexL == 0)
					{
						ep0_buffer[0] = hid_mouse_protocol;
						len = 1;
					}
					else
					{
						ep0_buffer[0] = hid_cc_protocol;
						len = 1;
					}
					break;
				case HID_SET_REPORT:
					// nothing to do here
					break;
				case HID_SET_IDLE:
					if(hid_setup_buf->wIndexL == 0)
					{
						hid_mouse_idle_rate = hid_setup_buf->wValueH;
					}
					else
					{
						hid_cc_idle_rate = hid_setup_buf->wValueH;
					}
					break;
				case HID_SET_PROTOCOL:
					if(hid_setup_buf->wIndexL == 0)
					{
						hid_mouse_protocol = hid_setup_buf->wValueL;
					}
					else
					{
						hid_cc_protocol = hid_setup_buf->wValueL;
					}
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
			usb_set_ep0_tog_res(EP_OUT_TOG_1 | EP_IN_TOG_1 | USB_OUT_RES_STALL | USB_IN_RES_STALL);
		}
		else
		{
			if(hid_setup_len > len)
				hid_setup_len = len;
			len = (hid_setup_len > HID_ENDP0_SIZE) ? HID_ENDP0_SIZE : hid_setup_len;
			
			if(descriptor_ptr != (UINT8 code*)NULL)
				hid_copy_descriptor(len);
			
			hid_setup_len -= len;
			usb_set_ep0_tx_len(len);
			usb_set_ep0_tog_res(EP_OUT_TOG_1 | EP_IN_TOG_1 | USB_OUT_RES_ACK | USB_IN_RES_EXPECT_ACK);
		}
	}	// end if(ep == EP_0)
}

void hid_on_rst(void)
{
	usb_set_ep0_tog_res(USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_0);
	usb_set_ep1_tog_res(USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_1);
	usb_set_ep2_tog_res(USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_1);
	
	ep1_buffer[0] = 0;
	ep1_buffer[1] = 0;
	ep1_buffer[2] = 0;
	ep1_buffer[3] = 0;
	ep1_buffer[4] = 0;
	hid_mouse_report_pending = 0;
	hid_mouse_idle_rate = 0;
	hid_mouse_protocol = 0x01;	//default to report protocol
	
	ep2_buffer[0] = 1;	//report id
	ep2_buffer[1] = 0;
	ep2_buffer[2] = 0;
	hid_cc_report_pending = 0;
	hid_cc_idle_rate = 0;
	hid_cc_protocol = 0x01;	//default to report protocol
}

usb_config_t code usb_config = 
{
	(UINT16)ep0_buffer,
	(UINT16)ep1_buffer,
	(UINT16)ep2_buffer,
	0x0000,
	USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_0,
	USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_1,
	USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_1,
	USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_0,
	USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_0,
	USB_EP1_TX_EN | USB_EP1_BUF_SINGLE,
	USB_EP2_TX_EN | USB_EP2_BUF_SINGLE | USB_EP3_BUF_SINGLE,
	USB_INT_TRANSFER | USB_INT_RST
};

void hid_init(void)
{
	usb_sof_callback = NULL;
	usb_out_callback = hid_on_out;
	usb_in_callback = hid_on_in;
	usb_setup_callback = hid_on_setup;
	usb_rst_callback = hid_on_rst;
	usb_suspend_callback = NULL;
	
	usb_init(&usb_config);
}

void hid_mouse_send_report(void)
{
	hid_mouse_report_pending = 1;
	usb_set_ep1_tx_len(HID_MOUSE_REPORT_SIZE);
	usb_set_ep1_in_res(USB_IN_RES_EXPECT_ACK);
}

void hid_mouse_press(UINT8 buttons)
{
	ep1_buffer[0] = ep1_buffer[0] | buttons;
	hid_mouse_send_report();
}

void hid_mouse_release(UINT8 buttons)
{
	ep1_buffer[0] = ep1_buffer[0] & ~buttons;
	hid_mouse_send_report();
}

void hid_mouse_move(UINT8 x_rel, UINT8 y_rel)
{
	ep1_buffer[1] += x_rel;
	ep1_buffer[2] += y_rel;
	hid_mouse_send_report();
}

void hid_mouse_scroll(UINT8 v_rel, UINT8 h_rel)
{
	ep1_buffer[3] += v_rel;
	ep1_buffer[4] += h_rel;
	hid_mouse_send_report();
}

void hid_cc_send_report(void)
{
	hid_cc_report_pending = 1;
	usb_set_ep2_tx_len(HID_CC_REPORT_SIZE);
	usb_set_ep2_in_res(USB_IN_RES_EXPECT_ACK);
}

void hid_cc_press(UINT16 key)
{
	ep2_buffer[1] = (UINT8)key;
	ep2_buffer[2] = (UINT8)(key >> 8);
	hid_cc_send_report();
}
