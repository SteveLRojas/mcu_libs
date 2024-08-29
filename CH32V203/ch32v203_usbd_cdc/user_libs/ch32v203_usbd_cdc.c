/*
 * ch32v203_usbd_cdc.c
 *
 *  Created on: Aug 9, 2024
 *      Author: Steve
 */
#include "ch32v20x.h"
#include "ch32v203_usbd.h"
#include "ch32v203_usbd_cdc.h"
//TODO: what happens if EP_2 receives a zero length packet?
//HINT: For double-buffered endpoints the CH32V203 reference manual does not say when the SW and HW toggles are compared. It also does not say when the STAT_RX/STAT_TX fields are updated and how the endpoint response is determined.
//Here is my current understanding: A double buffered endpoint will respond with ACK only if the SW and HW toggles are different and the STAT_RX/STAT_TX field is set to ACK, otherwise the response is NAK or as specified by STAT_RX/STAT_TX.
//At certain times the STAT_RX/STAT_TX field will be set to NAK by the hardware, this seems to happen when the CTR_RX/CTR_TX flag is cleared if the HW and SW toggles are equal.
//Once the hardware sets the STAT_RX/STAT_TX field to NAK, the user must set it back to ACK before communication on the endpoint resumes.

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


uint16_t cdc_last_data_time;
uint16_t cdc_last_status_time;
//volatile uint8_t cdc_tx_enabled;

//HINT:
//For RX double-buffered endpoint (EP_2) DTOG_RX indicates the buffer in use by HW and DTOG_TX indicates the buffer in use by SW.
//For TX double-buffered endpoint (EP_3) DTOG_TX indicates the buffer in use by HW and DTOG_RX indicates the buffer in use by SW.

//HINT: RX rules:
//1: If HW_toggle (EP_2 DTOG_RX) matches SW_toggle (EP_2 DTOG_TX) then move SW_toggle to match ep2_read_select when ep2_read_select changes.
//2: If HW_toggle (EP_2 DTOG_RX) moves then update SW_toggle (EP_2 DTOG_TX) to match ep2_read_select.
uint8_t ep2_read_select;
volatile uint8_t ep2_t0_num_bytes;
uint8_t ep2_t0_read_offset;
volatile uint8_t ep2_t1_num_bytes;
uint8_t ep2_t1_read_offset;

//HINT: TX rules:
//1: If HW_toggle (EP_3 DTOG_TX) matches SW_toggle (EP_3 DTOG_RX) then move SW_toggle to match ep3_write_select when ep3_write_select changes.
//2: If HW_toggle (EP_3 DTOG_TX) moves then update SW_toggle (EP_3 DTOG_RX) to match ep3_write_select.
//3: Update tx_len when SW_toggle (EP_3 DTOG_RX) changes.
volatile uint8_t ep3_wip;
volatile uint8_t ep3_write_select;
volatile uint8_t ep3_t0_num_bytes;
volatile uint8_t ep3_t1_num_bytes;

typedef struct _USB_SETUP_REQ
{
    uint8_t bRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} usb_setup_req_t;

usb_setup_req_t cdc_setup_buf __attribute__ ((aligned(2)));

const uint8_t* descriptor_ptr;
uint8_t cdc_address;
uint8_t cdc_config;

volatile cdc_line_coding_t cdc_line_coding __attribute__ ((aligned(2))) = {125000, 0x00, 0x00, 0x08, 0x00};	//125K baud, 1 stop, no parity, 8 data
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

void cdc_on_sof(void)
{
	// check for timeout, if the buffer is not being written to then enable transmitting and mark the buffer as full
	if(((uint16_t)(sof_count - cdc_last_data_time) >= (uint16_t)CDC_TIMEOUT_MS) && ((usbd_get_out_toggle(EP_3) && usbd_get_in_toggle(EP_3)) || (!usbd_get_out_toggle(EP_3) && !usbd_get_in_toggle(EP_3))) && !ep3_wip)
	{
		if(ep3_write_select)
		{
			usbd_set_tx_1_len(EP_3, ep3_t1_num_bytes);
			ep3_t1_num_bytes = CDC_ENDP3_SIZE;
			ep3_write_select = 0;
			//if((usbd_get_out_toggle(EP_3) && usbd_get_in_toggle(EP_3)) || (!usbd_get_out_toggle(EP_3) && !usbd_get_in_toggle(EP_3)))
			//{
				usbd_set_out_toggle(EP_3, USBD_OUT_TOG_0);
			//}
		}
		else
		{
			usbd_set_tx_0_len(EP_3, ep3_t0_num_bytes);
			ep3_t0_num_bytes = CDC_ENDP3_SIZE;
			ep3_write_select = 1;
			//if((usbd_get_out_toggle(EP_3) && usbd_get_in_toggle(EP_3)) || (!usbd_get_out_toggle(EP_3) && !usbd_get_in_toggle(EP_3)))
			//{
				usbd_set_out_toggle(EP_3, USBD_OUT_TOG_1);
			//}
		}
		usbd_set_in_res(EP_3, USBD_IN_RES_ACK);
		//cdc_tx_enabled = 1;
	}

	if((uint16_t)(sof_count - cdc_last_status_time) >= (uint16_t)CDC_TIMEOUT_MS)
	{
		usbd_set_tx_len(EP_1, CDC_SERIAL_STATE_SIZE);
		usbd_set_in_res(EP_1, USBD_IN_RES_ACK);
	}
}

void cdc_on_out(uint8_t ep)
{
	if(ep == EP_0)
	{
		if((cdc_setup_buf.bRequest == SET_LINE_CODING) && ((cdc_setup_buf.bRequestType & USB_REQ_TYP_MASK) == USB_REQ_TYP_CLASS))
		{
			usbd_read_from_pma(usbd_get_rx_buf_offset(EP_0), (uint16_t*)(&cdc_line_coding), (CDC_LINE_CODING_SIZE + 1) >> 1);
		}
		usbd_set_tx_len(EP_0, 0);
		usbd_set_tog_res(EP_0, USBD_OUT_TOG_1 | USBD_IN_TOG_1 | USBD_OUT_RES_ACK | USBD_IN_RES_ACK);
	}

	if(ep == EP_2)
	{
		//auto-toggle is enabled, so here the toggle should point to the next buffer that will receive
		if(usbd_get_out_toggle(EP_2))
		{
			ep2_t0_num_bytes = usbd_get_rx_0_len(EP_2);
			//EP_2 response set to NAK automatically if IN toggle still points to t1 buffer.
		}
		else
		{
			ep2_t1_num_bytes = usbd_get_rx_1_len(EP_2);
			//EP_2 response set to NAK automatically if IN toggle still points to t0 buffer.
		}
		usbd_set_in_toggle(EP_2, ep2_read_select ? USBD_IN_TOG_1 : USBD_IN_TOG_0);
	}
}

void cdc_on_in(uint8_t ep)
{
	uint8_t len;

	if(ep == EP_0)
	{
		if((cdc_setup_buf.bRequestType & USB_REQ_TYP_MASK) == USB_REQ_TYP_STANDARD)
		{
			switch(cdc_setup_buf.bRequest)
			{
			case USB_GET_DESCRIPTOR:
				len = cdc_setup_buf.wLength >= CDC_ENDP0_SIZE ? CDC_ENDP0_SIZE : cdc_setup_buf.wLength;
				usbd_write_to_pma(usbd_get_tx_buf_offset(EP_0), (uint16_t*)descriptor_ptr, (len + 1) >> 1);
				descriptor_ptr += len;
				cdc_setup_buf.wLength -= len;
				usbd_set_tx_len(EP_0, len);
				usbd_set_in_res(EP_0, USBD_IN_RES_ACK);
				break;
			case USB_SET_ADDRESS:
				usbd_set_addr(cdc_address);
				//in_res should be set to NAK automatically
				break;
			default:
				usbd_set_tx_len(EP_0, 0);
				//in_res should be set to NAK automatically
				break;
			}
		}
		else
		{
			usbd_set_tx_len(EP_0, 0);
			//in_res should be set to NAK automatically
		}
	}

	if(ep == EP_1)
	{
		usbd_set_tx_len(EP_1, 0);
		//in_res should be set to NAK automatically
		cdc_last_status_time = sof_count;
	}

	if(ep == EP_3)
	{
		//usbd_set_tx_len(EP_3, CDC_ENDP3_SIZE);
		//auto-toggle is enabled, so here the toggle should point to the next buffer that will transmit
		if(usbd_get_in_toggle(EP_3))
		{
			ep3_t0_num_bytes = 0;
			usbd_set_tx_0_len(EP_3, 0);
		}
		else
		{
			ep3_t1_num_bytes = 0;
			usbd_set_tx_1_len(EP_3, 0);
		}
		usbd_set_out_toggle(EP_3, ep3_write_select ? USBD_OUT_TOG_1 : USBD_OUT_TOG_0);
		cdc_last_data_time = sof_count;
	}
}

void cdc_on_setup(uint8_t ep)
{
	uint8_t len;

	if(ep == EP_0)
	{
		usbd_read_from_pma(usbd_get_rx_buf_offset(EP_0), (uint16_t*)(&cdc_setup_buf), 4);
		descriptor_ptr = 0;
		len = 0;

		if((cdc_setup_buf.bRequestType & USB_REQ_TYP_MASK) == USB_REQ_TYP_STANDARD)
		{
			switch(cdc_setup_buf.bRequest)
			{
			case USB_GET_STATUS:
				usbd_write_pma_word(usbd_get_tx_buf_offset(EP_0), 0);
				len = 2;
				break;
			case USB_CLEAR_FEATURE:
				if((cdc_setup_buf.bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP)
				{
					switch((uint8_t)(cdc_setup_buf.wIndex))
					{
					case 0x81:
						usbd_set_in_toggle(EP_1, USBD_IN_TOG_0);
						usbd_set_in_res(EP_1, USBD_IN_RES_NAK);
						break;
					case 0x02:
						usbd_set_out_toggle(EP_2, USBD_OUT_TOG_0);
						usbd_set_out_res(EP_2, USBD_OUT_RES_ACK);
						break;
					case 0x83:
						usbd_set_in_toggle(EP_3, USBD_IN_TOG_0);
						usbd_set_in_res(EP_3, USBD_IN_RES_NAK);
						break;
					default:
						len = 0xFF;
					}
				}
				else
				{
					len = 0xFF;
				}
				break;
			case USB_SET_FEATURE:
				if(((cdc_setup_buf.bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) && !(cdc_setup_buf.wIndex & 0xFF00))
				{
					switch((uint8_t)(cdc_setup_buf.wIndex))
					{
					case 0x81:
						usbd_set_in_toggle(EP_1, USBD_IN_TOG_0);
						usbd_set_in_res(EP_1, USBD_IN_RES_STALL);
						break;
					case 0x02:
						usbd_set_out_toggle(EP_2, USBD_OUT_TOG_0);
						usbd_set_out_res(EP_2, USBD_OUT_RES_STALL);
						break;
					case 0x83:
						usbd_set_in_toggle(EP_3, USBD_IN_TOG_0);
						usbd_set_in_res(EP_3, USBD_IN_RES_STALL);
						break;
					default:
						len = 0xFF;
					}
				}
				else
				{
					len = 0xFF;
				}
				break;
			case USB_SET_ADDRESS:
				cdc_address = (uint8_t)(cdc_setup_buf.wValue);
				break;
			case USB_GET_DESCRIPTOR:
				switch((uint8_t)(cdc_setup_buf.wValue >> 8))
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
					switch((uint8_t)(cdc_setup_buf.wValue))
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
			case USB_GET_CONFIGURATION:
				usbd_write_pma_word(usbd_get_tx_buf_offset(EP_0), (uint16_t)cdc_config);
				len = 1;
				break;
			case USB_SET_CONFIGURATION:
				cdc_config = (uint8_t)(cdc_setup_buf.wValue);
				break;
			case USB_GET_INTERFACE:
				usbd_write_pma_word(usbd_get_tx_buf_offset(EP_0), 0);
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
		else if((cdc_setup_buf.bRequestType & USB_REQ_TYP_MASK) == USB_REQ_TYP_CLASS)
		{
			switch(cdc_setup_buf.bRequest)
			{
			case GET_LINE_CODING:
				usbd_write_to_pma(usbd_get_tx_buf_offset(EP_0), (uint16_t*)(&cdc_line_coding), (CDC_LINE_CODING_SIZE + 1) >> 1);
				len = CDC_LINE_CODING_SIZE;
				break;
			case SET_CONTROL_LINE_STATE:
				cdc_control_line_state = (uint8_t)(cdc_setup_buf.wValue);
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
		}	// end if((hid_setup_buf.bRequestType & USB_REQ_TYP_MASK) == USB_REQ_TYP_STANDARD)

		if(len == 0xFF)
		{
			cdc_setup_buf.bRequest = 0xFF;
			usbd_set_tog_res(EP_0, USBD_OUT_TOG_1 | USBD_IN_TOG_1 | USBD_OUT_RES_STALL | USBD_IN_RES_STALL);
		}
		else
		{
			if(cdc_setup_buf.wLength > len)
				cdc_setup_buf.wLength = len;
			len = (cdc_setup_buf.wLength > CDC_ENDP0_SIZE) ? CDC_ENDP0_SIZE : cdc_setup_buf.wLength;

			if(descriptor_ptr)
			{
				usbd_write_to_pma(usbd_get_tx_buf_offset(EP_0), (uint16_t*)descriptor_ptr, (len + 1) >> 1);
				descriptor_ptr += len;
			}

			cdc_setup_buf.wLength -= len;
			usbd_set_tx_len(EP_0, len);
			usbd_set_tog_res(EP_0, USBD_OUT_TOG_1 | USBD_IN_TOG_1 | USBD_OUT_RES_ACK | USBD_IN_RES_ACK);
		}
	}	// end if(ep == EP_0)
}

void cdc_on_rst(void)
{
	ep2_read_select = 0;	//EP_2 IN toggle must be initialized to 1 (RX initially enabled)
	ep2_t0_num_bytes = 0;
	ep2_t0_read_offset = 0;
	ep2_t1_num_bytes = 0;
	ep2_t1_read_offset = 0;

	ep3_wip = 0;
	ep3_write_select = 0;	//EP_3 OUT toggle must be initialized to 0 (TX initially disabled)
	ep3_t0_num_bytes = 0;
	ep3_t1_num_bytes = 0;

	usbd_set_tog_res(EP_0, USBD_OUT_RES_ACK | USBD_IN_RES_NAK | USBD_OUT_TOG_0 | USBD_IN_TOG_0);
	usbd_set_tog_res(EP_1, USBD_OUT_RES_ACK | USBD_IN_RES_NAK | USBD_OUT_TOG_0 | USBD_IN_TOG_0);
	usbd_set_tog_res(EP_2, USBD_OUT_RES_ACK | USBD_IN_RES_DISABLED | USBD_OUT_TOG_0 | USBD_IN_TOG_1);
	usbd_set_tog_res(EP_3, USBD_OUT_RES_DISABLED | USBD_IN_RES_NAK | USBD_OUT_TOG_0 | USBD_IN_TOG_0);

	cdc_last_data_time = 0;
	cdc_last_status_time = 0;
	sof_count = 0;
}

//TODO: move these functions to the USBD library and use the DMA controller
void usbd_write_bytes_to_pma(uint16_t byte_offset, const uint8_t* source, uint16_t num_bytes)
{
	uint16_t* pma = (uint16_t*)(USBD_PMA_BASE + (byte_offset & 0xFFFE));
	uint16_t num_words;
	uint16_t pma_word;

	if(byte_offset & 0x01)
	{
		pma_word = *pma;
		((uint8_t*)(&pma_word))[1] = *source;
		*pma = pma_word;
		--num_bytes;
		pma += 2;
		++source;
	}

	num_words = num_bytes >> 1;
	while(num_words--)
	{
		((uint8_t*)(&pma_word))[0] = *source;
		++source;
		((uint8_t*)(&pma_word))[1] = *source;
		++source;
		*pma = pma_word;
		pma += 2;
	}

	if(num_bytes & 0x01)
	{
		*pma = (uint16_t)(*source);
	}
}

void usbd_read_bytes_from_pma(uint16_t byte_offset, uint8_t* dest, uint16_t num_bytes)
{
	uint16_t* pma = (uint16_t*)(USBD_PMA_BASE + (byte_offset & 0xFFFE));
	uint16_t num_words;
	uint16_t pma_word;

	if(byte_offset & 0x01)
	{
		*dest = (uint8_t)(*pma >> 8);
		--num_bytes;
		pma += 2;
		++dest;
	}

	num_words = num_bytes >> 1;
	while(num_words--)
	{
		pma_word = *pma;
		*dest = ((uint8_t*)(&pma_word))[0];
		++dest;
		*dest = ((uint8_t*)(&pma_word))[1];
		++dest;
		pma += 2;
	}

	if(num_bytes & 0x01)
	{
		*dest = (uint8_t)(*pma);
	}
}

static const usbd_config_t usbd_config =
{
	{CDC_ENDP0_SIZE, CDC_ENDP1_SIZE, 0, CDC_ENDP3_SIZE, 0, 0, 0, 0},
	{CDC_ENDP0_SIZE, 0, CDC_ENDP2_SIZE, 0, 0, 0, 0, 0},
	{USBD_EP_TYPE_CONTROL, USBD_EP_TYPE_INTERRUPT, USBD_EP_TYPE_BULK_DBL, USBD_EP_TYPE_BULK_DBL, USBD_EP_TYPE_BULK, USBD_EP_TYPE_BULK, USBD_EP_TYPE_BULK, USBD_EP_TYPE_BULK},
	USBD_INT_TRANSFER | USBD_INT_RESET | USBD_INT_SOF
};

void cdc_init(void)
{
	usbd_sof_callback = cdc_on_sof;
	usbd_esof_callback = 0;
	usbd_out_callback = cdc_on_out;
	usbd_in_callback = cdc_on_in;
	usbd_setup_callback = cdc_on_setup;
	usbd_reset_callback = cdc_on_rst;
	usbd_wakeup_callback = 0;
	usbd_suspend_callback = 0;

	usbd_init(&usbd_config);
	cdc_set_serial_state(0x00);
}

void cdc_set_serial_state(uint8_t val)
{
	usbd_write_to_pma(usbd_get_tx_buf_offset(EP_1), (uint16_t*)cdc_serial_state, (CDC_SERIAL_STATE_SIZE + 1) >> 1);
	usbd_write_pma_word(usbd_get_tx_buf_offset(EP_1) + 8, (uint16_t)val);
	usbd_set_tx_len(EP_1, CDC_SERIAL_STATE_SIZE);
	usbd_set_in_res(EP_1, USBD_IN_RES_ACK);
}

// Receive
uint16_t cdc_bytes_available(void)
{
	// count the number of bytes available in both buffers
	return (ep2_t0_num_bytes - ep2_t0_read_offset) + (ep2_t1_num_bytes - ep2_t1_read_offset);
}

uint8_t cdc_peek(void)
{
	uint16_t pma_word;
	uint8_t byte_select;

	if(ep2_read_select)
	{
		pma_word = usbd_read_pma_word(usbd_get_rx_1_buf_offset(EP_2) + (ep2_t1_read_offset & 0xFE));
		byte_select = ep2_t1_read_offset & 0x01;
	}
	else
	{
		pma_word = usbd_read_pma_word(usbd_get_rx_0_buf_offset(EP_2) + (ep2_t0_read_offset & 0xFE));
		byte_select = ep2_t0_read_offset & 0x01;
	}
	return ((uint8_t*)(&pma_word))[byte_select];
}

uint8_t cdc_read_byte(void)
{
	uint16_t pma_word;
	uint8_t byte_select;

	while(!cdc_bytes_available());

	if(ep2_read_select)
	{
		pma_word = usbd_read_pma_word(usbd_get_rx_1_buf_offset(EP_2) + (ep2_t1_read_offset & 0xFE));
		byte_select = ep2_t1_read_offset & 0x01;
		++ep2_t1_read_offset;
		if(ep2_t1_read_offset == ep2_t1_num_bytes)
		{
			ep2_t1_read_offset = 0;
			ep2_t1_num_bytes = 0;
			ep2_read_select = 0;
			if((usbd_get_out_toggle(EP_2) && usbd_get_in_toggle(EP_2)) || (!usbd_get_out_toggle(EP_2) && !usbd_get_in_toggle(EP_2)))
			{
				usbd_set_in_toggle(EP_2, USBD_IN_TOG_0);
				usbd_set_out_res(EP_2, USBD_OUT_RES_ACK);
			}
		}
	}
	else
	{
		pma_word = usbd_read_pma_word(usbd_get_rx_0_buf_offset(EP_2) + (ep2_t0_read_offset & 0xFE));
		byte_select = ep2_t0_read_offset & 0x01;
		++ep2_t0_read_offset;
		if(ep2_t0_read_offset == ep2_t0_num_bytes)
		{
			ep2_t0_read_offset = 0;
			ep2_t0_num_bytes = 0;
			ep2_read_select = 1;
			if((usbd_get_out_toggle(EP_2) && usbd_get_in_toggle(EP_2)) || (!usbd_get_out_toggle(EP_2) && !usbd_get_in_toggle(EP_2)))
			{
				usbd_set_in_toggle(EP_2, USBD_IN_TOG_1);
				usbd_set_out_res(EP_2, USBD_OUT_RES_ACK);
			}
		}
	}
	return ((uint8_t*)(&pma_word))[byte_select];
}

void cdc_read_bytes(uint8_t* dest, uint16_t num_bytes)
{
	uint16_t num_to_read;

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
			usbd_read_bytes_from_pma((usbd_get_rx_1_buf_offset(EP_2) << 1) + ep2_t1_read_offset, dest, num_to_read);

			if(ep2_t1_read_offset == ep2_t1_num_bytes)
			{
				ep2_t1_read_offset = 0;
				ep2_t1_num_bytes = 0;
				ep2_read_select = 0;
				if((usbd_get_out_toggle(EP_2) && usbd_get_in_toggle(EP_2)) || (!usbd_get_out_toggle(EP_2) && !usbd_get_in_toggle(EP_2)))
				{
					usbd_set_in_toggle(EP_2, USBD_IN_TOG_0);
					usbd_set_out_res(EP_2, USBD_OUT_RES_ACK);
				}
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
			usbd_read_bytes_from_pma((usbd_get_rx_0_buf_offset(EP_2) << 1) + ep2_t0_read_offset, dest, num_to_read);

			if(ep2_t0_read_offset == ep2_t0_num_bytes)
			{
				ep2_t0_read_offset = 0;
				ep2_t0_num_bytes = 0;
				ep2_read_select = 1;
				if((usbd_get_out_toggle(EP_2) && usbd_get_in_toggle(EP_2)) || (!usbd_get_out_toggle(EP_2) && !usbd_get_in_toggle(EP_2)))
				{
					usbd_set_in_toggle(EP_2, USBD_IN_TOG_1);
					usbd_set_out_res(EP_2, USBD_OUT_RES_ACK);
				}
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
	uint16_t pma_word;
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

		pma_word = usbd_read_pma_word(usbd_get_tx_1_buf_offset(EP_3) + (ep_num_bytes & 0xFE));
		((uint8_t*)(&pma_word))[ep_num_bytes & 0x01] = val;
		usbd_write_pma_word(usbd_get_tx_1_buf_offset(EP_3) + (ep_num_bytes & 0xFE), pma_word);
		++ep_num_bytes;
		ep3_t1_num_bytes = ep_num_bytes;

		if(ep_num_bytes == CDC_ENDP3_SIZE)
		{
			usbd_set_tx_1_len(EP_3, CDC_ENDP3_SIZE);
			ep3_write_select = 0;
			if((usbd_get_out_toggle(EP_3) && usbd_get_in_toggle(EP_3)) || (!usbd_get_out_toggle(EP_3) && !usbd_get_in_toggle(EP_3)))
			{
				usbd_set_out_toggle(EP_3, USBD_OUT_TOG_0);
				usbd_set_in_res(EP_3, USBD_IN_RES_ACK);
			}
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

		pma_word = usbd_read_pma_word(usbd_get_tx_0_buf_offset(EP_3) + (ep_num_bytes & 0xFE));
		((uint8_t*)(&pma_word))[ep_num_bytes & 0x01] = val;
		usbd_write_pma_word(usbd_get_tx_0_buf_offset(EP_3) + (ep_num_bytes & 0xFE), pma_word);
		++ep_num_bytes;
		ep3_t0_num_bytes = ep_num_bytes;

		if(ep_num_bytes == CDC_ENDP3_SIZE)
		{
			usbd_set_tx_0_len(EP_3, CDC_ENDP3_SIZE);
			ep3_write_select = 1;
			if((usbd_get_out_toggle(EP_3) && usbd_get_in_toggle(EP_3)) || (!usbd_get_out_toggle(EP_3) && !usbd_get_in_toggle(EP_3)))
			{
				usbd_set_out_toggle(EP_3, USBD_OUT_TOG_1);
				usbd_set_in_res(EP_3, USBD_IN_RES_ACK);
			}
		}
	}
	ep3_wip = 0;
}

void cdc_write_bytes(const uint8_t* src, uint16_t num_bytes)
{
	uint16_t num_to_write;
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
			usbd_write_bytes_to_pma((usbd_get_tx_1_buf_offset(EP_3) << 1) + ep_num_bytes, src, num_to_write);
			ep3_t1_num_bytes = ep_num_bytes;

			if(ep_num_bytes == CDC_ENDP3_SIZE)
			{
				usbd_set_tx_1_len(EP_3, CDC_ENDP3_SIZE);
				ep3_write_select = 0;
				if((usbd_get_out_toggle(EP_3) && usbd_get_in_toggle(EP_3)) || (!usbd_get_out_toggle(EP_3) && !usbd_get_in_toggle(EP_3)))
				{
					usbd_set_out_toggle(EP_3, USBD_OUT_TOG_0);
					usbd_set_in_res(EP_3, USBD_IN_RES_ACK);
				}
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
			usbd_write_bytes_to_pma((usbd_get_tx_0_buf_offset(EP_3) << 1) + ep_num_bytes, src, num_to_write);
			ep3_t0_num_bytes = ep_num_bytes;

			if(ep_num_bytes == CDC_ENDP3_SIZE)
			{
				usbd_set_tx_0_len(EP_3, CDC_ENDP3_SIZE);
				ep3_write_select = 1;
				if((usbd_get_out_toggle(EP_3) && usbd_get_in_toggle(EP_3)) || (!usbd_get_out_toggle(EP_3) && !usbd_get_in_toggle(EP_3)))
				{
					usbd_set_out_toggle(EP_3, USBD_OUT_TOG_1);
					usbd_set_in_res(EP_3, USBD_IN_RES_ACK);
				}
			}
		}
	}
	ep3_wip = 0;
}
