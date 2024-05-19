#include <string.h>
#include "CH552.H"
#include "System.h"
#include "CH552_USB.h"
#include "FTDI_serial.h"

void usb_halt(UINT8 keep);	//TODO: remove

//char code ftdi_reset_string[] = "FTDI bus reset\n";
//char code ftdi_suspend_string[] = "FTDI suspend\n";
//char code ftdi_get_descriptor_string[] = "FTDI get descriptor\n";
//char code ftdi_set_address_string[] = "FTDI set_address\n";
//char code ftdi_set_configuration_string[] = "FTDI set configuration\n";
//char code ftdi_set_feature_string[] = "FTDI set feature\n";

UINT8 xdata Ep0Buffer[DEFAULT_ENDP0_SIZE + 2] _at_ 0x0000;	//Endpoint 0 OUT&IN buffer, must be an even address
UINT8 xdata Ep1Buffer[MAX_PACKET_SIZE] _at_ 0x0040;		//Endpoint 1 IN send buffer
UINT8 xdata Ep2Buffer[MAX_PACKET_SIZE] _at_ 0x0080;		//Endpoint 2 OUT receive buffer

UINT16 SetupLen;
UINT8  SetupReq, UsbConfig;
UINT8  VendorControl;

UINT8 code* pDescr;			//USB configuration flag
UINT8 pDescr_Index = 0;
#define UsbSetupBuf	 ((PUSB_SETUP_REQ)Ep0Buffer)

/*device descriptor*/
UINT8 code DevDesc[] =
{
	0x12, 0x01, 0x10, 0x01, 0x00, 0x00, 0x00, DEFAULT_ENDP0_SIZE,
	0x03, 0x04, 0x01, 0x60, 0x00, 0x04, 0x01, 0x02,
	0x03, 0x01
};

UINT8 code CfgDesc[] =
{
	0x09, 0x02, 32, 0,
	0x01, 0x01, 0x00, 0x80, 0x32,		 //Configuration descriptor (1 interface)
	//The following are interface 0 (data interface) descriptors
	0x09, 0x04, 0x00, 0x00, 0x02, 0xff, 0xff, 0xff, 0x00,	//data interface descriptor
	0x07, 0x05, 0x81, 0x02, 0x40, 0x00, 0x00,				//Endpoint Descriptor EP1 BULK IN
	0x07, 0x05, 0x02, 0x02, 0x40, 0x00, 0x00,				//Endpoint Descriptor EP2 BULK OUT
};

/*string descriptor*/
unsigned char code LangDes[] = {0x04, 0x03, 0x09, 0x04};	//language descriptor

unsigned char code Prod_Des[] =							//Product String Descriptor
{
	26, 0x03,
	'M', 0x00, '5', 0x00, 'S', 0x00, 't', 0x00, 'a', 0x00, 'c', 0x00,
	'k', 0x00, ' ', 0x00, 'I', 0x00, 'n', 0x00, 't', 0x00, 'f', 0x00
};

unsigned char code Manuf_Des[] =
{
	28, 0x03,
	'K', 0x00, 'o', 0x00, 'n', 0x00, 'g', 0x00, 'o', 0x00, 'u', 0x00,
	' ', 0x00, 'H', 0x00, 'i', 0x00, 'k', 0x00, 'a', 0x00, 'r', 0x00, 'i', 0x00
};

/* Download control */
volatile UINT8 idata USBOutLength = 0;
volatile UINT8 idata USBOutPtr = 0;
volatile UINT8 idata USBReceived = 0;
/* upload control */
volatile UINT8 idata UpPoint1_Busy = 0;   //Is the upload endpoint busy flag

/* Miscellaneous */
volatile UINT8 idata Latency_Timer = 4; //Latency Timer
//volatile UINT8 idata Require_DFU = 0;

/*void Jump_to_BL()
{
	ES = 0;
	PS = 0;

	USB_INT_EN = 0;
	USB_CTRL = 0x06;
	//UDEV_CTRL = 0x80;

	mDelaymS(100);

	EA = 0;

	while(1)
	{
		ASM
		LJMP 0x3800
		ENDASM;
	}
}*/

UINT8 code HexToAscTab[] = "0123456789ABCDEF";

void uuidcpy(UINT8 xdata *dest, UINT8 index, UINT8 len) /* Generate USB Serial Number using UUID */
{
	UINT8 i;
	UINT8 p = 0; /* UUID format, ten hexadecimal digits */
	UINT8 code *puuid;
	for(i = index; i < (index + len); i++)
	{
		if(i == 0)
			dest[p++] = 22; //10 * 2 + 2
		else if(i == 1)
			dest[p++] = 0x03;
		else
		{
			if(i & 0x01) //odd number
			{
				dest[p++] = 0x00;
			}
			else
			{
				puuid = (UINT8 code *) (0x3ffa + (i - 2) / 4);
				if(i & 0x02)
					dest[p++] = HexToAscTab[(*puuid) >> 4];
				else
					dest[p++] = HexToAscTab[(*puuid) & 0x0f];
			}
		}
	}
}

void ftdi_on_out(UINT8 ep)
{
	if((ep == EP_2) && usb_is_toggle_ok())
	{
		RESET_KEEP = 0x02;
		usb_set_ep2_out_res(USB_OUT_RES_NAK);
		USBReceived = 1;
		USBOutPtr = 0;
		USBOutLength = usb_get_rx_len();
	}
	
	if(ep == EP_0)
	{
		if(SetupReq != 0x22)
		{
			usb_set_ep0_tx_len(0);
			usb_set_ep0_in_res(USB_IN_RES_NAK);
			usb_set_ep0_out_res(USB_OUT_RES_ACK);
		}
	}
}

void ftdi_on_in(UINT8 ep)
{
	UINT16 len;
	
	if(ep == EP_1)
	{
		usb_set_ep1_tx_len(0);
		usb_set_ep1_in_res(USB_IN_RES_NAK);
		UpPoint1_Busy = 0;
	}
	
	if(ep == 0)
	{
		switch(SetupReq)
		{
		case USB_GET_DESCRIPTOR:
			len = SetupLen >= DEFAULT_ENDP0_SIZE ? DEFAULT_ENDP0_SIZE : SetupLen;	//The length of this transmission
			if(pDescr == (UINT8 code*)0xffff)
			{
				uuidcpy(Ep0Buffer, pDescr_Index, len);
			}
			else
			{
				memcpy(Ep0Buffer, pDescr + pDescr_Index, len);	//load upload data
			}
			SetupLen -= len;
			pDescr_Index += len;
			usb_set_ep0_tx_len(len);									 
			usb_toggle_ep0_in_toggle();	//Sync flag flip
			break;
		case USB_SET_ADDRESS:
			if(VendorControl == 0)
			{
				usb_set_addr(SetupLen);
				usb_set_ep0_tog_res(EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_0 | USB_OUT_RES_ACK | USB_IN_RES_NAK);
			}
			break;
		default:
			usb_set_ep0_tx_len(0);	//Status phase completion interrupt or forced upload of 0-length data packets to end control transmission
			usb_set_ep0_tog_res(EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_0 | USB_OUT_RES_ACK | USB_IN_RES_NAK);
			break;
		}
	}
}

void ftdi_on_setup(UINT8 ep)
{
	UINT16 len;
	
	if(ep == EP_0)
	{
		len = usb_get_rx_len();
		if(len == sizeof(USB_SETUP_REQ))
		{
			SetupLen = ((UINT16)UsbSetupBuf->wLengthH << 8) | (UsbSetupBuf->wLengthL);
			len = 0;	// Default is success and upload 0 length
			VendorControl = 0;
			SetupReq = UsbSetupBuf->bRequest;
			if((UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD)//non-standard request
			{
				VendorControl = 1;
				if(UsbSetupBuf->bRequestType & USB_REQ_TYP_READ)
				{
					//read
					switch(SetupReq)
					{
					case 0x90: //READ EEPROM
						Ep0Buffer[0] = 0xff;
						Ep0Buffer[1] = 0xff;
						len = 2;
						break;
					case 0x0a:
						Ep0Buffer[0] = Latency_Timer;
						len = 1;
						break;
					case 0x05:
						Ep0Buffer[0] = 0x01;
						Ep0Buffer[1] = 0x60;
						len = 2;
						break;
					default:
						len = 0xFF;	 /*command not supported*/
						break;
					}
				}
				else
				{
					//Write
					switch(SetupReq)
					{
					case 0x02:
					case 0x04:
					case 0x06:
					case 0x07:
					case 0x0b:
					case 0x92:
						len = 0;
						break;
					case 0x91: //WRITE EEPROM, FT_PROG action, jump directly to BL
						//Require_DFU = 1;
						usb_halt(0x04);	//TODO: remove
						len = 0;
						break;
					case 0x00:
						UpPoint1_Busy = 0;
						len = 0;
						break;
					case 0x09: //SET LATENCY TIMER
						Latency_Timer = UsbSetupBuf->wValueL;
						len = 0;
						break;
					case 0x03:
						//TODO: adjust baud rate here...
						len = 0;
						break;
					case 0x01: //MODEM Control
						//TODO: do modem control stuff here
						len = 0;
						break;
					default:
						len = 0xFF;
						break;
					}
				}
			}
			else	//Standard request
			{
				switch(SetupReq)	//request code
				{
				case USB_GET_DESCRIPTOR:
					switch(UsbSetupBuf->wValueH)
					{
					case 1:			//device descriptor
						pDescr = DevDesc;	//Send the device descriptor to the buffer to be sent
						len = sizeof(DevDesc);
						break;
					case 2:			//configuration descriptor
						pDescr = CfgDesc;	//Send the device descriptor to the buffer to be sent
						len = sizeof(CfgDesc);
						break;
					case 3:
						if(UsbSetupBuf->wValueL == 0)
						{
							pDescr = LangDes;
							len = sizeof(LangDes);
						}
						else if(UsbSetupBuf->wValueL == 1)
						{
							pDescr = Manuf_Des;
							len = sizeof(Manuf_Des);
						}
						else if(UsbSetupBuf->wValueL == 2)
						{
							pDescr = Prod_Des;
							len = sizeof(Prod_Des);
						}
						else
						{
							pDescr = (UINT8 code*)0xffff;	//special case handled below
							len = 22;
						}
						break;
					default:
						len = 0xff;
						break;
					}

					if(SetupLen > len)
					{
						SetupLen = len;
					}
					len = (SetupLen >= DEFAULT_ENDP0_SIZE) ? DEFAULT_ENDP0_SIZE : SetupLen;							//??????
					if(pDescr == (UINT8 code*) 0xffff)
					{
						uuidcpy(Ep0Buffer, 0, len);
					}
					else
					{
						memcpy(Ep0Buffer, pDescr, len);
					}
					SetupLen -= len;
					pDescr_Index = len;
					break;
				case USB_SET_ADDRESS:
					SetupLen = UsbSetupBuf->wValueL;
					break;
				case USB_GET_CONFIGURATION:
					Ep0Buffer[0] = UsbConfig;
					if(SetupLen >= 1)
					{
						len = 1;
					}
					break;
				case USB_SET_CONFIGURATION:
					UsbConfig = UsbSetupBuf->wValueL;
					break;
				case USB_GET_INTERFACE:
					break;
				case USB_CLEAR_FEATURE:
					if((UsbSetupBuf->bRequestType & 0x1F ) == USB_REQ_RECIP_DEVICE)
					{
						if((((UINT16)UsbSetupBuf->wValueH << 8) | UsbSetupBuf->wValueL) == 0x01)
						{
							if(!(CfgDesc[7] & 0x20))
							{
								len = 0xFF;
							}
						}
						else
						{
							len = 0xFF;
						}
					}
					else if ((UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP)
					{
						switch(UsbSetupBuf->wIndexL)
						{
						case 0x83:
							//UEP3_CTRL = UEP3_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
							usb_set_ep3_in_toggle(EP_IN_TOG_0);
							usb_set_ep3_in_res(USB_IN_RES_NAK);
							break;
						case 0x03:
							//UEP3_CTRL = UEP3_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
							usb_set_ep3_out_toggle(EP_OUT_TOG_0);
							usb_set_ep3_out_res(USB_OUT_RES_ACK);
							break;
						case 0x82:
							//UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
							usb_set_ep2_in_toggle(EP_IN_TOG_0);
							usb_set_ep2_in_res(USB_IN_RES_NAK);
							break;
						case 0x02:
							//UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
							usb_set_ep2_out_toggle(EP_OUT_TOG_0);
							usb_set_ep2_out_res(USB_OUT_RES_ACK);
							break;
						case 0x81:
							//UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
							usb_set_ep1_in_toggle(EP_IN_TOG_0);
							usb_set_ep1_in_res(USB_IN_RES_NAK);
							break;
						case 0x01:
							//UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
							usb_set_ep1_out_toggle(EP_OUT_TOG_0);
							usb_set_ep1_out_res(USB_OUT_RES_ACK);
							break;
						default:
							len = 0xFF;
							break;
						}
						UpPoint1_Busy = 0;
					}
					else
					{
						len = 0xFF;
					}
					break;
				case USB_SET_FEATURE:
					if((UsbSetupBuf->bRequestType & 0x1F) == USB_REQ_RECIP_DEVICE)
					{
						if((((UINT16)UsbSetupBuf->wValueH << 8) | UsbSetupBuf->wValueL) == 0x01)
						{
							if(CfgDesc[7] & 0x20)
							{
								RESET_KEEP = 0x06;
								/*while (XBUS_AUX & bUART0_TX)
								{
									;
								}
								SAFE_MOD = 0x55;
								SAFE_MOD = 0xAA;
								WAKE_CTRL = bWAK_BY_USB;
								PCON |= PD;
								SAFE_MOD = 0x55;
								SAFE_MOD = 0xAA;
								WAKE_CTRL = 0x00;*/ //TODO:
							}
							else
							{
								len = 0xFF;
							}
						}
						else
						{
							len = 0xFF;
						}
					}
					else if((UsbSetupBuf->bRequestType & 0x1F) == USB_REQ_RECIP_ENDP)
					{
						if((((UINT16)UsbSetupBuf->wValueH << 8) | UsbSetupBuf->wValueL) == 0x00)
						{
							switch(((UINT16)UsbSetupBuf->wIndexH << 8) | UsbSetupBuf->wIndexL)
							{
							case 0x83:
								//UEP3_CTRL = UEP3_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;
								usb_set_ep3_in_toggle(EP_IN_TOG_0);
								usb_set_ep3_in_res(USB_IN_RES_STALL);
								break;
							case 0x03:
								//UEP3_CTRL = UEP3_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;
								usb_set_ep3_out_toggle(EP_OUT_TOG_0);
								usb_set_ep3_out_res(USB_OUT_RES_STALL);
								break;
							case 0x82:
								//UEP2_CTRL = UEP2_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;
								usb_set_ep2_in_toggle(EP_IN_TOG_0);
								usb_set_ep2_in_res(USB_IN_RES_STALL);
								break;
							case 0x02:
								//UEP2_CTRL = UEP2_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;
								usb_set_ep2_out_toggle(EP_OUT_TOG_0);
								usb_set_ep2_out_res(USB_OUT_RES_STALL);
								break;
							case 0x81:
								//UEP1_CTRL = UEP1_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;
								usb_set_ep1_in_toggle(EP_IN_TOG_0);
								usb_set_ep1_in_res(USB_IN_RES_STALL);
								break;
							case 0x01:
								//UEP1_CTRL = UEP1_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;
								usb_set_ep1_out_toggle(EP_OUT_TOG_0);
								usb_set_ep1_out_res(USB_OUT_RES_STALL);
								break;
							default:
								len = 0xFF;				/* operation failed */
								break;
							}
						}
						else
						{
							len = 0xFF;
						}
					}
					else
					{
						len = 0xFF;
					}
					break;
				case USB_GET_STATUS:
					RESET_KEEP = 0x07;
					Ep0Buffer[0] = 0x00;
					Ep0Buffer[1] = 0x00;
					if ( SetupLen >= 2 )
					{
						len = 2;
					}
					else
					{
						len = SetupLen;
					}
					break;
				default:
					len = 0xff;			//operation failed
					break;
				}
			}
		}
		else
		{
			RESET_KEEP = 0x08;
			len = 0xff;			//wrong packet length
		}
		if(len == 0xff)
		{
			RESET_KEEP = 0x09;
			SetupReq = 0xFF;
			usb_set_ep0_tog_res(EP_OUT_TOG_1 | EP_IN_TOG_1 | USB_OUT_RES_STALL | USB_IN_RES_STALL);
		}
		else if(len <= DEFAULT_ENDP0_SIZE)	//The upload data or status phase returns a 0-length packet
		{
			usb_set_ep0_tx_len(len);
			//The default data packet is DATA1, and the response ACK is returned
			usb_set_ep0_tog_res(EP_OUT_TOG_1 | EP_IN_TOG_1 | USB_OUT_RES_ACK | USB_IN_RES_EXPECT_ACK);
		}
		else
		{
			usb_set_ep0_tx_len(0);	//Although it has not yet reached the state stage, upload 0-length data packets in advance to prevent the host from entering the state stage in advance
			//The default data packet is DATA1, and the response ACK is returned
			usb_set_ep0_tog_res(EP_OUT_TOG_1 | EP_IN_TOG_1 | USB_OUT_RES_ACK | USB_IN_RES_EXPECT_ACK);
		}
	}
}

void ftdi_on_rst(void)
{
	//uart_write_string(UART_0, ftdi_reset_string);
	usb_set_ep0_tog_res(USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_0);
	usb_set_ep1_tog_res(USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_1);
	usb_set_ep2_tog_res(USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_1);
	
	UsbConfig = 0;		  //Clear configuration values
	UpPoint1_Busy = 0;

	USBOutLength = 0;
	USBOutPtr = 0;
	USBReceived = 0;
}

void ftdi_on_suspend(void)
{
	//uart_write_string(UART_0, ftdi_suspend_string);
	RESET_KEEP = 0x0A;
	/*SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	WAKE_CTRL = bWAK_BY_USB | bWAK_RXD0_LO | bWAK_RXD1_LO;
	PCON |= PD;
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	WAKE_CTRL = 0x00;*/
}

usb_config_t code usb_config = 
{
	(UINT16)Ep0Buffer,
	(UINT16)Ep1Buffer,
	(UINT16)Ep2Buffer,
	0x0000,
	USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_0,
	USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_1,
	USB_OUT_RES_ACK | USB_IN_RES_EXPECT_ACK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_1,
	USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_0,
	USB_OUT_RES_ACK | USB_IN_RES_NAK | EP_OUT_TOG_0 | EP_IN_TOG_0 | EP_AUTOTOG_0,
	USB_EP1_TX_EN | USB_EP1_BUF_SINGLE,
	USB_EP2_RX_EN | USB_EP2_BUF_SINGLE,
	USB_INT_SOF | USB_INT_SUSPEND | USB_INT_TRANSFER | USB_INT_RST
};

void ftdi_init(void)
{
	usb_out_callback = ftdi_on_out;
	usb_in_callback = ftdi_on_in;
	usb_setup_callback = ftdi_on_setup;
	usb_rst_callback = ftdi_on_rst;
	usb_suspend_callback = ftdi_on_suspend;
	
	usb_init(&usb_config);
	
	/* Pre-populated Modem Status */
	Ep1Buffer[0] = 0x01;
	Ep1Buffer[1] = 0x60;
}

//TODO: ftdi_task function
