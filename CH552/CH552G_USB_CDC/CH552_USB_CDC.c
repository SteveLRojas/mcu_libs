#include "CH552.H"
#include "CH552_FIFO.h"
#include "CH552_USB.h"
#include "CH552_USB_CDC.h"

#define SET_LINE_CODING         0x20  // host configures line coding
#define GET_LINE_CODING         0x21  // host reads configured line coding
#define SET_CONTROL_LINE_STATE  0x22  // generates RS-232/V.24 style control signals
#define SEND_BREAK              0x23  // send break

#define CDC_DEV_DESCR_SIZE 18
#define CDC_CONF_DESCR_SIZE 67
#define CDC_LINE_CODING_SIZE 7
#define CDC_SERIAL_STATE_SIZE 9

fifo_t cdc_rx_fifo;
fifo_t cdc_tx_fifo;
UINT8 cdc_rx_buf[CDC_RX_FIFO_SIZE];
UINT8 cdc_tx_buf[CDC_TX_FIFO_SIZE];

UINT16 cdc_last_data_time;
UINT16 cdc_last_status_time;
UINT8 cdc_tx_busy;

UINT8 xdata ep0_buffer[CDC_ENDP0_BUF_SIZE];
UINT8 xdata ep1_buffer[CDC_ENDP1_BUF_SIZE];
UINT8 xdata ep2_buffer[CDC_ENDP2_BUF_SIZE];
UINT8 xdata ep3_buffer[CDC_ENDP3_BUF_SIZE];

#define cdc_setup_buf ((PUSB_SETUP_REQ)ep0_buffer)

UINT8 cdc_setup_req;
UINT8 cdc_setup_type;
UINT16 cdc_setup_len;

UINT8 code* descriptor_ptr;
UINT8 cdc_address;
UINT8 cdc_config;

volatile cdc_line_coding_t cdc_line_coding = {0x48, 0xE8, 0x01, 0x00, 0x00, 0x00, 0x08};	//125K baud, 1 stop, no parity, 8 data
volatile UINT8 cdc_control_line_state = 0;

/* USB Device Descriptors */
UINT8 code cdc_device_descriptor[] =
{ 
    CDC_DEV_DESCR_SIZE,				// bLength
    0x01,                           // bDescriptorType
    0x10, 0x01,                     // bcdUSB
    0x02,                           // bDeviceClass	(Communications Device Class)
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

/* USB Configration Descriptors */
UINT8 code  cdc_config_descriptor[] =
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
UINT8 code cdc_string_lang_id[] =
{
	0x04,
	0x03,
	0x09,
	0x04 
};

/* USB Device String Vendor */
UINT8 code cdc_string_vendor[] =
{
	14,    
	0x03,           
	'w',0,'c',0,'h',0,'.',0,'c',0,'n',0
};

/* USB Device String Product */
UINT8 code cdc_string_product[] =
{
    22,
    0x03,
    'U', 0, 'S', 0, 'B', 0, ' ', 0, 'S', 0, 'e', 0, 'r', 0, 'i', 0, 'a', 0, 'l', 0
};

/* USB Device String Serial */
UINT8 code cdc_string_serial[] =
{
	22,          
	0x03,                   
	'D', 0, 'E', 0, 'A', 0, 'D', 0, 'B', 0, 'E', 0 , 'E', 0, 'F', 0, '0', 0, '1', 0
};

UINT8 code cdc_serial_state[] = {0xA1, 0x20, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00};


void cdc_copy_descriptor(UINT8 len)
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

// HINT: This library contains some nasty hacks to work around bugs in CH552.
// Enabling SOF interrupts makes interrupt transfers fail, since the SOF triggers UIF_TRANSFER (all enpoints respond with NAK while UIF_TRANSFER is set).
// When a notification needs to be sent the SOF interrupts are disabled so that the interrupt transfer can happen.
// SOF interrupts are re-enableb after the interrupt transfer completes.
void cdc_on_sof(void)
{
	UINT8 bytes_to_send;
	
	if(!cdc_tx_busy)
	{
		if((fifo_num_used(&cdc_tx_fifo) >= CDC_ENDP3_SIZE) || ((sof_count - cdc_last_data_time) >= CDC_TIMEOUT_MS))
		{
			bytes_to_send = fifo_num_used(&cdc_tx_fifo);
			if(bytes_to_send > CDC_ENDP3_SIZE)
				bytes_to_send = CDC_ENDP3_SIZE;
			
			fifo_read(&cdc_tx_fifo, ep3_buffer, bytes_to_send);
			usb_set_ep3_tx_len(bytes_to_send);
			usb_set_ep3_in_res(USB_IN_RES_EXPECT_ACK);
			cdc_tx_busy = 1;
		}
	}
	
	if(fifo_num_free(&cdc_rx_fifo) >= CDC_ENDP2_SIZE)
	{
		usb_set_ep2_out_res(USB_OUT_RES_ACK);
	}
	
	if((sof_count - cdc_last_status_time) >= CDC_TIMEOUT_MS)
	{
		usb_set_ep1_tx_len(CDC_SERIAL_STATE_SIZE);
		usb_set_ep1_in_res(USB_IN_RES_EXPECT_ACK);
		usb_disable_interrupts(USB_INT_SOF);
	}
}

void cdc_on_out(UINT8 ep)
{
	UINT8 idx;
	
	if(ep == EP_0)
	{
		if((cdc_setup_req == SET_LINE_CODING) && ((cdc_setup_type & USB_REQ_TYP_MASK) == USB_REQ_TYP_CLASS))
		{
			for(idx = 0; idx < CDC_LINE_CODING_SIZE; idx++)
			{
				((UINT8*)&cdc_line_coding)[idx] = ep0_buffer[idx];
			}
		}
		usb_set_ep0_tx_len(0);
		usb_set_ep0_tog_res(EP_OUT_TOG_1 | EP_IN_TOG_1 | USB_OUT_RES_ACK | USB_IN_RES_EXPECT_ACK);
	}
	
	if((ep == EP_2) && usb_is_toggle_ok())
	{
		fifo_write(&cdc_rx_fifo, ep2_buffer, usb_get_rx_len());
		if(fifo_num_free(&cdc_rx_fifo) < CDC_ENDP2_SIZE)
		{
			usb_set_ep2_out_res(USB_OUT_RES_NAK);
		}
	}
}

void cdc_on_in(UINT8 ep)
{
	UINT8 len;
	
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
					usb_set_ep0_tx_len(len);
					usb_toggle_ep0_in_toggle();
					break;
				case USB_SET_ADDRESS:
					usb_set_addr(cdc_address);
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
		cdc_last_status_time = sof_count;
		usb_enable_interrupts(USB_INT_SOF);
		++sof_count;
	}
	
	if(ep == EP_3)
	{
		if(fifo_num_used(&cdc_tx_fifo) >= CDC_ENDP3_SIZE)
		{
			fifo_read(&cdc_tx_fifo, ep3_buffer, CDC_ENDP3_SIZE);
			usb_set_ep3_tx_len(CDC_ENDP3_SIZE);
		}
		else
		{
			usb_set_ep3_tx_len(0);
			usb_set_ep3_in_res(USB_IN_RES_NAK);
			cdc_tx_busy = 0;
		}
		cdc_last_data_time = sof_count;
	}
}

void cdc_on_setup(UINT8 ep)
{
	UINT8 len;
	UINT8 idx;
	
	if(ep == EP_0)
	{
		cdc_setup_len = ((UINT16)cdc_setup_buf->wLengthH<<8) | (cdc_setup_buf->wLengthL);
		cdc_setup_req = cdc_setup_buf->bRequest;
		cdc_setup_type = cdc_setup_buf->bRequestType;
		descriptor_ptr = (UINT8 code*)NULL;
		len = 0;
		
		if((cdc_setup_type & USB_REQ_TYP_MASK) == USB_REQ_TYP_STANDARD)
		{
			switch(cdc_setup_req)
			{
				case USB_GET_DESCRIPTOR:
					switch(cdc_setup_buf->wValueH)
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
							switch(cdc_setup_buf->wValueL)
							{
								case 0:
									descriptor_ptr = cdc_string_lang_id;
									break;
								case 1:
									descriptor_ptr = cdc_string_vendor;
									break;
								case 2:
									descriptor_ptr = cdc_string_product;
									break;
								case 3:
									descriptor_ptr = cdc_string_serial;
									break;
								case 4:
									descriptor_ptr = cdc_string_product;
									break;
								default:
									descriptor_ptr = cdc_string_serial;
									break;
							}
							len = descriptor_ptr[0];
							break;
						default:
							len = 0xFF;
							break;
					}
					break;
				case USB_SET_ADDRESS:
					cdc_address = cdc_setup_buf->wValueL;
					break;
				case USB_GET_CONFIGURATION:
					ep0_buffer[0] = cdc_config;
					len = 1;
					break;
				case USB_SET_CONFIGURATION:
					cdc_config = cdc_setup_buf->wValueL;
					usb_enable_interrupts(USB_INT_SOF);
					break;
				case USB_GET_INTERFACE:
					ep0_buffer[0] = 0;
					len = 1;
					break;
				case USB_SET_INTERFACE:
					//nothing to do here
					break;
				case USB_GET_STATUS:
					ep0_buffer[0] = 0x00;
					ep0_buffer[1] = 0x00;
					len = 2;
					break;
				case USB_CLEAR_FEATURE:
					if((cdc_setup_type & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP)
					{
						switch(cdc_setup_buf->wIndexL)
						{
							case 0x01:
								usb_set_ep1_out_toggle(EP_OUT_TOG_0);
								usb_set_ep1_out_res(USB_OUT_RES_ACK);
								break;
							case 0x81:
								usb_set_ep1_in_toggle(EP_IN_TOG_0);
								usb_set_ep1_in_res(USB_IN_RES_NAK);
								usb_enable_interrupts(USB_INT_SOF);	//SOF interrupts must be enabled when EP1 is not ready
								break;
							case 0x02:
								usb_set_ep2_out_toggle(EP_OUT_TOG_0);
								usb_set_ep2_out_res(USB_OUT_RES_ACK);
								break;
							case 0x82:
								usb_set_ep2_in_toggle(EP_IN_TOG_0);
								usb_set_ep2_in_res(USB_IN_RES_NAK);
								break;
							case 0x03:
								usb_set_ep3_out_toggle(EP_OUT_TOG_0);
								usb_set_ep3_out_res(USB_OUT_RES_ACK);
								break;
							case 0x83:
								usb_set_ep3_in_toggle(EP_IN_TOG_0);
								usb_set_ep3_in_res(USB_IN_RES_NAK);
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
					if(((cdc_setup_type & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) && !cdc_setup_buf->wIndexH)
					{
						switch(cdc_setup_buf->wIndexL)
						{
							case 0x01:
								usb_set_ep1_out_toggle(EP_OUT_TOG_0);
								usb_set_ep1_out_res(USB_OUT_RES_STALL);
								break;
							case 0x81:
								usb_set_ep1_in_toggle(EP_IN_TOG_0);
								usb_set_ep1_in_res(USB_IN_RES_STALL);
								usb_enable_interrupts(USB_INT_SOF);	//SOF interrupts must be enabled when EP1 is not ready
								break;
							case 0x02:
								usb_set_ep2_out_toggle(EP_OUT_TOG_0);
								usb_set_ep2_out_res(USB_OUT_RES_STALL);
								break;
							case 0x82:
								usb_set_ep2_in_toggle(EP_IN_TOG_0);
								usb_set_ep2_in_res(USB_IN_RES_STALL);
								break;
							case 0x03:
								usb_set_ep3_out_toggle(EP_OUT_TOG_0);
								usb_set_ep3_out_res(USB_OUT_RES_STALL);
								break;
							case 0x83:
								usb_set_ep3_in_toggle(EP_IN_TOG_0);
								usb_set_ep3_in_res(USB_IN_RES_STALL);
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
						ep0_buffer[idx] = ((UINT8*)&cdc_line_coding)[idx];
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
			usb_set_ep0_tog_res(EP_OUT_TOG_1 | EP_IN_TOG_1 | USB_OUT_RES_STALL | USB_IN_RES_STALL);
		}
		else
		{
			if(cdc_setup_len > len)
				cdc_setup_len = len;
			len = (cdc_setup_len > CDC_ENDP0_SIZE) ? CDC_ENDP0_SIZE : cdc_setup_len;
			
			if(descriptor_ptr != (UINT8 code*)NULL)
				cdc_copy_descriptor(len);
			
			cdc_setup_len -= len;
			usb_set_ep0_tx_len(len);
			usb_set_ep0_tog_res(EP_OUT_TOG_1 | EP_IN_TOG_1 | USB_OUT_RES_ACK | USB_IN_RES_EXPECT_ACK);
		}
	}	//end if(ep == EP_0)
}

void cdc_on_rst(void)
{
	usb_disable_interrupts(USB_INT_SOF);
	fifo_init(&cdc_rx_fifo, cdc_rx_buf, CDC_RX_FIFO_SIZE);
	fifo_init(&cdc_tx_fifo, cdc_tx_buf, CDC_TX_FIFO_SIZE);
	
	usb_set_ep0_tog_res(USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_0);
	usb_set_ep1_tog_res(USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_1);
	usb_set_ep2_tog_res(USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_1);
	usb_set_ep3_tog_res(USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_1);
	
	cdc_last_data_time = 0;
	cdc_last_status_time = 0;
	sof_count = 0;
	cdc_tx_busy = 0;
}

usb_config_t code usb_config = 
{
	(UINT16)ep0_buffer,
	(UINT16)ep1_buffer,
	(UINT16)ep2_buffer,
	(UINT16)ep3_buffer,
	USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_0,
	USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_1,
	USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_1,
	USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_1,
	USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_0,
	USB_EP1_TX_EN | USB_EP1_BUF_SINGLE,
	USB_EP2_RX_EN | USB_EP2_BUF_SINGLE | USB_EP3_TX_EN | USB_EP3_BUF_SINGLE,
	USB_INT_TRANSFER | USB_INT_RST
};

void cdc_init(void)
{
	fifo_init(&cdc_rx_fifo, cdc_rx_buf, CDC_RX_FIFO_SIZE);
	fifo_init(&cdc_tx_fifo, cdc_tx_buf, CDC_TX_FIFO_SIZE);
	
	usb_sof_callback = cdc_on_sof;
	usb_out_callback = cdc_on_out;
	usb_in_callback = cdc_on_in;
	usb_setup_callback = cdc_on_setup;
	usb_rst_callback = cdc_on_rst;
	usb_suspend_callback = NULL;
	
	usb_init(&usb_config);
	cdc_set_serial_state(0x00);
}

void cdc_set_serial_state(UINT8 val)
{
	UINT8 idx;
	for(idx = 0; idx < CDC_SERIAL_STATE_SIZE; ++idx)
	{
		ep1_buffer[idx] = cdc_serial_state[idx];
	}
	ep1_buffer[7] = val;
	usb_set_ep1_tx_len(CDC_SERIAL_STATE_SIZE);
	usb_set_ep1_in_res(USB_IN_RES_EXPECT_ACK);
}

// Receive
UINT16 cdc_bytes_available(void)
{
	return fifo_num_used(&cdc_rx_fifo);
}

UINT8 cdc_peek(void)
{
	return fifo_peek(&cdc_rx_fifo);
}

UINT8 cdc_read_byte(void)
{
	UINT8 popped;

	while(fifo_empty(&cdc_rx_fifo));

	usb_disable_interrupts(USB_INT_TRANSFER | USB_INT_RST);
	popped = fifo_pop(&cdc_rx_fifo);
	usb_enable_interrupts(USB_INT_TRANSFER | USB_INT_RST);
	
	return popped;
}

void cdc_read_bytes(UINT8* dest, UINT16 num_bytes)
{
	UINT16 num_remaining = num_bytes;
	UINT16 num_to_read = 0;

	while(num_remaining)
	{
		num_to_read = fifo_num_used(&cdc_rx_fifo);
		if (num_to_read > num_remaining)
		{
			num_to_read = num_remaining;
		}

		usb_disable_interrupts(USB_INT_TRANSFER | USB_INT_RST);
		fifo_read(&cdc_rx_fifo, dest, num_to_read);
		usb_enable_interrupts(USB_INT_TRANSFER | USB_INT_RST);

		num_remaining -= num_to_read;
		dest += num_to_read;
	}
}

// Send
UINT16 cdc_bytes_available_for_write(void)
{
	return fifo_num_free(&cdc_tx_fifo);
}

void cdc_write_byte(UINT8 val)
{
	usb_disable_interrupts(USB_INT_TRANSFER | USB_INT_RST);
	fifo_push(&cdc_tx_fifo, val);
	usb_enable_interrupts(USB_INT_TRANSFER | USB_INT_RST);
}

void cdc_write_bytes(UINT8* src, UINT16 num_bytes)
{
	UINT16 num_remaining = num_bytes;
	UINT16 num_to_write = 0;

	while(num_remaining)
	{
		num_to_write = fifo_num_free(&cdc_tx_fifo);
		if(num_to_write > num_remaining)
		{
			num_to_write = num_remaining;
		}

		usb_disable_interrupts(USB_INT_TRANSFER | USB_INT_RST);
		fifo_write(&cdc_tx_fifo, src, num_to_write);
		usb_enable_interrupts(USB_INT_TRANSFER | USB_INT_RST);
		
		num_remaining -= num_to_write;
		src += num_to_write;
	}
}
