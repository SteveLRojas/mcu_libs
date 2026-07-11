#include "CH552.H"
#include "CH552_USB.h"
#include "CH552_USB_HID_KB.h"

#define HID_DEV_DESCR_SIZE 18
#define HID_CONF_DESCR_SIZE 34
#define HID_REPORT_DESCR_SIZE 65
#define HID_KB_REPORT_SIZE 8

UINT8 xdata hid_ep0_buf[HID_ENDP0_BUF_SIZE];
UINT8 xdata hid_ep1_buf[HID_ENDP1_BUF_SIZE];

#define hid_setup_buf ((PUSB_SETUP_REQ)hid_ep0_buf)

UINT8 hid_setup_req;
UINT8 hid_setup_type;
UINT16 hid_setup_len;
UINT8 code* hid_descriptor_ptr;
#if HID_USE_UNIQUE_ID
UINT8 hid_string_serial_offset;
#endif

UINT8 hid_kb_address;
volatile UINT8 hid_kb_config;

volatile UINT8 hid_kb_report_pending;
volatile UINT8 hid_kb_idle_rate;
volatile UINT8 hid_kb_indicators;
UINT8 hid_kb_protocol;

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
    0x01,                           // bNumInterfaces
    0x01,                           // bConfigurationValue
    0x00,                           // iConfiguration
    0x80,                           // bmAttributes: bus powered, no wakeup
    0x32,                           // MaxPower: 100mA

    /* Interface Descriptor (Keyboard) */
    0x09,                           // bLength
    0x04,                           // bDescriptorType
    0x00,                           // bInterfaceNumber
    0x00,                           // bAlternateSetting
    0x01,                           // bNumEndpoints
    0x03,                           // bInterfaceClass
    0x01,                           // bInterfaceSubClass
    0x01,                           // bInterfaceProtocol: Keyboard
    0x04,                           // iInterface

    /* HID Descriptor (Keyboard) */
    0x09,                           // bLength
    0x21,                           // bDescriptorType
    0x10, 0x01,                     // bcdHID
    0x00,                           // bCountryCode
    0x01,                           // bNumDescriptors
    0x22,                           // bDescriptorType
    HID_REPORT_DESCR_SIZE & 0xFF, HID_REPORT_DESCR_SIZE >> 8, // wDescriptorLength

    /* Endpoint Descriptor (Keyboard) */
    0x07,                           // bLength
    0x05,                           // bDescriptorType
    0x81,                           // bEndpointAddress: IN Endpoint 1
    0x03,                           // bmAttributes
    HID_ENDP1_SIZE & 0xFF, HID_ENDP1_SIZE >> 8, // wMaxPacketSize
    0x01                            // bInterval: 1mS
};

/* HID Report Descriptor */
UINT8 code hid_report_descriptor[] =
{
    0x05, 0x01,                     // Usage Page (Generic Desktop)
    0x09, 0x06,                     // Usage (Keyboard)
    0xA1, 0x01,                     // Collection (Application)
	0x05, 0x07,                     // Usage Page (Keyboard/keypad)
	0x19, 0xE0,						// Usage Minimum (Keyboard LeftControl)
	0x29, 0xE7,                     // Usage Maximum (Keyboard Right GUI)
    0x15, 0x00,                     // Logical Minimum (0)
    0x25, 0x01,                     // Logical Maximum (1)
	0x75, 0x01,                     // Report Size (1)
    0x95, 0x08,                     // Report Count (8)
	0x81, 0x02,                     // Input (Data, Variable, Absolute)
	0x75, 0x08,                     // Report Size (8)
    0x95, 0x01,                     // Report Count (1)
	0x81, 0x01,                     // Input (Constant, Array, Absolute)
	0x75, 0x01,                     // Report Size (1)
    0x95, 0x05,                     // Report Count (5)
	0x05, 0x08,                     // Usage Page (LED)
	0x19, 0x01,						// Usage Minimum (Num Lock)
	0x29, 0x05,                     // Usage Maximum (Kana)
	0x91, 0x02,                     // Output (Data, Variable, Absolute)
	0x75, 0x03,                     // Report Size (3)
    0x95, 0x01,                     // Report Count (1)
	0x91, 0x01,                     // Output (Constant, Array, Absolute)
	0x75, 0x08,                     // Report Size (8)
    0x95, 0x06,                     // Report Count (6)
	0x15, 0x00,                     // Logical Minimum (0)
    0x26, 0xFF, 0x00,               // Logical Maximum (255)
	0x05, 0x07,                     // Usage Page (Keyboard/Keypad)
	0x19, 0x00,                     // Usage Minimum (Reserved)
    0x2A, 0xFF, 0x00,               // Usage Maximum (Reserved)
	0x81, 0x00,                     // Input (Data, Array, Absolute)
    0xC0                            // End Collection
};

/* USB String Descriptors */
UINT8 code hid_string_lang_id[] =
{
	0x04,
	0x03,
	0x09,
	0x04 
};

/* USB Device String Vendor */
UINT8 code hid_string_vendor[] =
{
	14,    
	0x03,           
	'w',0,'c',0,'h',0,'.',0,'c',0,'n',0
};

/* USB Device String Product */
UINT8 code hid_string_product[] =
{
    26,
    0x03,
    'U', 0, 'S', 0, 'B', 0, ' ', 0, 'K', 0, 'e', 0, 'y', 0, 'b', 0, 'o', 0, 'a', 0, 'r', 0, 'd', 0
};

/* USB Device String Serial */
#if HID_USE_UNIQUE_ID
UINT8 code hid_string_serial[] = {22, 0x03};
#else
UINT8 code hid_string_serial[] =
{
	22,
	0x03,
	'D', 0, 'E', 0, 'A', 0, 'D', 0, 'B', 0, 'E', 0 , 'E', 0, 'F', 0, '0', 0, '7', 0
};
#endif

void hid_kb_copy_descriptor(UINT8 len)
{
	UINT8* dest = hid_ep0_buf;
	while(len)
	{
		*dest = *hid_descriptor_ptr;
		++dest;
		++hid_descriptor_ptr;
		--len;
	}
}

#if HID_USE_UNIQUE_ID
void hid_gen_string_serial(UINT8 len)
{
	UINT8* dest = hid_ep0_buf;
	UINT16 chip_id;
	UINT8 id_hex;
	while(len && (hid_string_serial_offset < 2))
	{
		*dest = hid_string_serial[hid_string_serial_offset];
		dest += 1;
		hid_string_serial_offset += 1;
		len -= 1;
	}
	
	while(len)
	{
		if(hid_string_serial_offset & 0x01)
		{
			*dest = 0x00;
		}
		else
		{
			chip_id = ROM_CHIP_ID_HX | 0x01;	//The LSB is set here and byte order reversed later to skip constant at 0x3FFB
			chip_id = (chip_id + ((hid_string_serial_offset - 2) >> 2)) ^ 0x01;
			id_hex = *((UINT8 code*)chip_id);
			if((hid_string_serial_offset >> 1) & 0x01)
				id_hex = id_hex >> 4;
			id_hex &= 0x0F;
			if(id_hex < 10)
				id_hex += '0';
			else
				id_hex += 'A' - 10;
			*dest = id_hex;
		}
		dest += 1;
		hid_string_serial_offset += 1;
		len -= 1;
	}
}
#endif

void hid_kb_on_out(UINT8 ep)
{
	if(ep == EP_0)
	{
		if((hid_setup_req == HID_SET_REPORT) && ((hid_setup_type & USB_REQ_TYP_MASK) == USB_REQ_TYP_CLASS))
			hid_kb_indicators = hid_ep0_buf[0];
		
		usb_set_ep0_tx_len(0);
		usb_set_ep0_tog_res(EP_OUT_TOG_1 | EP_IN_TOG_1 | USB_OUT_RES_ACK | USB_IN_RES_EXPECT_ACK);
	}
}

void hid_kb_on_in(UINT8 ep)
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
#if HID_USE_UNIQUE_ID
					if(hid_string_serial_offset)
						hid_gen_string_serial(len);
					else
						hid_kb_copy_descriptor(len);
#else
					hid_kb_copy_descriptor(len);
#endif
					hid_setup_len -= len;
					usb_set_ep0_tx_len(len);
					usb_toggle_ep0_in_toggle();
					break;
				case USB_SET_ADDRESS:
					usb_set_addr(hid_kb_address);
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
		hid_kb_report_pending = 0;
	}
}

void hid_kb_on_setup(UINT8 ep)
{
	UINT8 len;
	UINT8 idx;
	
	if(ep == EP_0)
	{
		hid_setup_len = ((UINT16)hid_setup_buf->wLengthH<<8) | (hid_setup_buf->wLengthL);
		hid_setup_req = hid_setup_buf->bRequest;
		hid_setup_type = hid_setup_buf->bRequestType;
		hid_descriptor_ptr = (UINT8 code*)NULL;
#if HID_USE_UNIQUE_ID
		hid_string_serial_offset = 0;
#endif
		len = 0;
		
		if((hid_setup_type & USB_REQ_TYP_MASK) == USB_REQ_TYP_STANDARD)
		{
			switch(hid_setup_req)
			{
				case USB_GET_STATUS:
					hid_ep0_buf[0] = 0x00;
					hid_ep0_buf[1] = 0x00;
					len = 2;
					break;
				case USB_CLEAR_FEATURE:
					if((hid_setup_type & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP)
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
					if(((hid_setup_type & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) && !hid_setup_buf->wIndexH)
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
					hid_kb_address = hid_setup_buf->wValueL;
					break;
				case USB_GET_DESCRIPTOR:
					switch(hid_setup_buf->wValueH)
					{
						case USB_DESCR_TYP_DEVICE:
							hid_descriptor_ptr = hid_device_descriptor;
							len = HID_DEV_DESCR_SIZE;
							break;
						case USB_DESCR_TYP_CONFIG:
							hid_descriptor_ptr = hid_config_descriptor;
							len = HID_CONF_DESCR_SIZE;
							break;
						case USB_DESCR_TYP_STRING:
							switch(hid_setup_buf->wValueL)
							{
								case 0:
									hid_descriptor_ptr = hid_string_lang_id;
									break;
								case 1:
									hid_descriptor_ptr = hid_string_vendor;
									break;
								case 2:
									hid_descriptor_ptr = hid_string_product;
									break;
								case 3:
									hid_descriptor_ptr = hid_string_serial;
#if HID_USE_UNIQUE_ID
									hid_string_serial_offset = 0xFF;
#endif
									break;
								case 4:
									hid_descriptor_ptr = hid_string_product;
									break;
								default:
									hid_descriptor_ptr = hid_string_product;
									break;
							}
							len = hid_descriptor_ptr[0];
							break;
						case USB_DESCR_TYP_REPORT:
							if(hid_setup_buf->wValueL == 0)
							{
								hid_descriptor_ptr = hid_report_descriptor;
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
					hid_ep0_buf[0] = hid_kb_config;
					len = 1;
					break;
				case USB_SET_CONFIGURATION:
					hid_kb_config = hid_setup_buf->wValueL;
					break;
				case USB_GET_INTERFACE:
					hid_ep0_buf[0] = 0;
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
					for(idx = 0; idx < HID_KB_REPORT_SIZE; ++idx)
					{
						hid_ep0_buf[idx] = hid_ep1_buf[idx];	//the report does not have its own buffer, it stays in ep1
					}
					len = HID_KB_REPORT_SIZE;
					break;
				case HID_GET_IDLE:
					hid_ep0_buf[0] = hid_kb_idle_rate;
					len = 1;
					break;
				case HID_GET_PROTOCOL:
					hid_ep0_buf[0] = hid_kb_protocol;
					len = 1;
					break;
				case HID_SET_REPORT:
					//nothing to do here
					break;
				case HID_SET_IDLE:
					hid_kb_idle_rate = hid_setup_buf->wValueH;
					break;
				case HID_SET_PROTOCOL:
					hid_kb_protocol = hid_setup_buf->wValueL;
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
			
#if HID_USE_UNIQUE_ID			
			if(hid_string_serial_offset)
			{
				hid_string_serial_offset = 0;
				hid_gen_string_serial(len);
			}
			else if(hid_descriptor_ptr != (UINT8 code*)NULL)
				hid_kb_copy_descriptor(len);
#else
			if(hid_descriptor_ptr != (UINT8 code*)NULL)
				hid_kb_copy_descriptor(len);
#endif
			
			hid_setup_len -= len;
			usb_set_ep0_tx_len(len);
			usb_set_ep0_tog_res(EP_OUT_TOG_1 | EP_IN_TOG_1 | USB_OUT_RES_ACK | USB_IN_RES_EXPECT_ACK);
		}
	}	// end if(ep == EP_0)
}

void hid_kb_on_rst(void)
{
	UINT8 idx;
	usb_set_ep0_tog_res(USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_0);
	usb_set_ep1_tog_res(USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_1);
	
	for(idx = 0; idx < HID_KB_REPORT_SIZE; ++idx)
	{
		hid_ep1_buf[idx] = 0x00;
	}
	hid_kb_report_pending = 0;
	hid_kb_idle_rate = 0;
	hid_kb_protocol = 0x01;	//default to report protocol
	hid_kb_config = 0;
	hid_kb_indicators = 0;
}

usb_config_t code usb_kb_config = 
{
	(UINT16)hid_ep0_buf,
	(UINT16)hid_ep1_buf,
	0x0000,
	0x0000,
	USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_0,
	USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_1,
	USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_0,
	USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_0,
	USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_0,
	USB_EP1_TX_EN | USB_EP1_BUF_SINGLE,
	USB_EP2_BUF_SINGLE | USB_EP3_BUF_SINGLE,
	USB_INT_TRANSFER | USB_INT_RST
};

void hid_kb_init(void)
{
	usb_sof_callback = NULL;
	usb_out_callback = hid_kb_on_out;
	usb_in_callback = hid_kb_on_in;
	usb_setup_callback = hid_kb_on_setup;
	usb_rst_callback = hid_kb_on_rst;
	usb_suspend_callback = NULL;
	
	usb_init(&usb_kb_config);
}

void hid_kb_send_report(void)
{
	hid_kb_report_pending = 1;
	usb_set_ep1_tx_len(HID_KB_REPORT_SIZE);
	usb_set_ep1_in_res(USB_IN_RES_EXPECT_ACK);
}

void hid_kb_press_modifier(UINT8 modifiers)
{
	hid_ep1_buf[0] |= modifiers;
	hid_kb_send_report();
}

void hid_kb_release_modifier(UINT8 modifiers)
{
	hid_ep1_buf[0] &= ~modifiers;
	hid_kb_send_report();
}

void hid_kb_press_key(UINT8 key)
{
	UINT8 idx;
	
	for(idx = 2; idx < HID_KB_REPORT_SIZE; ++idx)
	{
		if(hid_ep1_buf[idx] == key)
			return;	//key is already pressed
	}
	
	for(idx = 2; idx < HID_KB_REPORT_SIZE; ++idx)
	{
		if(!hid_ep1_buf[idx])
		{
			hid_ep1_buf[idx] = key;
			hid_kb_send_report();
			return;
		}
	}
	
	for(idx = 2; idx < HID_KB_REPORT_SIZE; ++idx)
	{
		hid_ep1_buf[idx] = 0x00;
	}
	hid_ep1_buf[2] = HID_KB_KEY_ROLL_OVER;
	hid_kb_send_report();
}

void hid_kb_release_key(UINT8 key)
{
	UINT8 idx;
	
	for(idx = 2; idx < HID_KB_REPORT_SIZE; ++idx)
	{
		if(hid_ep1_buf[idx] == key)
		{
			hid_ep1_buf[idx] = 0x00;
			hid_kb_send_report();
			return;
		}
	}
}

void hid_kb_release_all_keys(void)
{
	UINT8 idx;
	
	for(idx = 0; idx < HID_KB_REPORT_SIZE; ++idx)
	{
		hid_ep1_buf[idx] = 0x00;
	}
}
