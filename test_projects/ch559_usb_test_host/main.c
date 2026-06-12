#include "CH559.H"
#include "CH559_RCC.h"
#include "CH559_GPIO.h"
#include "CH559_TIMER.h"
#include "CH559_UART.h"
#include "CH559_USB_HOST.h"

#define BAUD_RATE 125000

//Pins:
// RXD0 = P02
// TXD0 = P03
// LED1 = P14
// LED2 = P15
// RXD1 = P26
// TXD1 = P27
// USB_DM = P50
// USB_DP = P51

char code test_string[] = "Unicorn\n";
char code str_attached[] = "USB device connected\n";
char code str_found_ls[] = "Found low speed device\n";
char code str_found_fs[] = "Found full speed device\n";
char code str_detached[] = "USB device disconnected\n";
char code str_bad_command[] = "Got an invalid command\n";
char code str_response[] = "Response:\n";
char code str_descriptor[] = "Descriptor:\n";
char code str_response_ok[] = "Response ok\n";
char code str_button_pressed[] = "Button pressed\n";
char code str_mouse_moved[] = "Mouse moved\n";
char code str_wheel_moved[] = "Wheel moved\n";


UINT8C usb_request_dev_descr[] =
{
	USB_REQ_TYP_IN | USB_REQ_TYP_STANDARD | USB_REQ_RECIP_DEVICE,
	USB_GET_DESCRIPTOR,
	0x00, USB_DESCR_TYP_DEVICE,
	0x00, 0x00,
	18, 0x00
};

UINT8C usb_request_set_addr[] =
{
	USB_REQ_TYP_OUT | USB_REQ_TYP_STANDARD | USB_REQ_RECIP_DEVICE,
	USB_SET_ADDRESS,
	0x01, 0x00,
	0x00, 0x00,
	0x00, 0x00
};

UINT8C usb_request_set_config[] =
{
	USB_REQ_TYP_OUT | USB_REQ_TYP_STANDARD | USB_REQ_RECIP_DEVICE,
	USB_SET_CONFIGURATION,
	0x01, 0x00,
	0x00, 0x00,
	0x00, 0x00
};

UINT8 transfer_buf[256];
usbh_ep_info_t ep0_info = {0, 8, USBH_IN_TOG_0 | USBH_IN_SEND_ACK, USBH_OUT_TOG_0 | USBH_OUT_EXPECT_ACK};
usbh_ep_info_t ep1_info = {1, 4, USBH_IN_TOG_0 | USBH_IN_SEND_ACK, USBH_OUT_TOG_0 | USBH_OUT_EXPECT_ACK};
usbh_ep_info_t ep2_info = {2, 4, USBH_IN_TOG_0 | USBH_IN_SEND_ACK, USBH_OUT_TOG_0 | USBH_OUT_EXPECT_ACK};
usbh_ep_info_t ep3_info = {3, 4, USBH_IN_TOG_0 | USBH_IN_SEND_ACK, USBH_OUT_TOG_0 | USBH_OUT_EXPECT_ACK};
usbh_ep_info_t ep4_info = {4, 4, USBH_IN_TOG_0 | USBH_IN_SEND_ACK, USBH_OUT_TOG_0 | USBH_OUT_EXPECT_ACK};
usbh_ep_info_t ep5_info = {5, 4, USBH_IN_TOG_0 | USBH_IN_SEND_ACK, USBH_OUT_TOG_0 | USBH_OUT_EXPECT_ACK};
usbh_ep_info_t ep6_info = {6, 4, USBH_IN_TOG_0 | USBH_IN_SEND_ACK, USBH_OUT_TOG_0 | USBH_OUT_EXPECT_ACK};
usbh_ep_info_t ep7_info = {7, 4, USBH_IN_TOG_0 | USBH_IN_SEND_ACK, USBH_OUT_TOG_0 | USBH_OUT_EXPECT_ACK};

usbh_ep_info_t* code ep_info[8] = {&ep0_info, &ep1_info, &ep2_info, &ep3_info, &ep4_info, &ep5_info, &ep6_info, &ep7_info};

char line_buf[10];

void copy_request(UINT8C* source)
{
	UINT8 idx;
	
	for(idx = 0; idx < 8; ++idx)
	{
		usbh_tx_buf[idx] = source[idx];
	}
}

char code hex_table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
void byte_to_hex(UINT8 value, char* buff)
{
	buff[0] = hex_table[(value >> 4) & 0x0f];
	buff[1] = hex_table[(value) & 0x0f];
	buff[2] = '\0';
}

void print_bytes_as_hex(UINT8* source, UINT8 num_bytes)
{
	UINT8 idx;
	UINT16 len;
	
	for(idx = 0; idx < num_bytes; ++idx)
	{
		line_buf[0] = hex_table[idx >> 4];
		line_buf[1] = hex_table[idx & 0x0F];
		line_buf[2] = ':';
		line_buf[3] = ' ';
		line_buf[4] = hex_table[source[idx] >> 4];
		line_buf[5] = hex_table[source[idx] & 0x0F];
		line_buf[6] = '\n';
		line_buf[7] = '\0';
		len = uart_write_string(UART_0, line_buf);
	}
}


#define S_DISCONNECTED	0x00
#define S_RESET_START	0x01
#define S_RESET_END		0x02
#define S_SET_SPEED		0x03
#define S_CONNECTED		0x04
#define S_GET_DEV_DESCR	0x05
#define S_SET_ADDR		0x06
#define S_CONFIGURE		0x07
#define S_IDLE			0x08
#define S_RUN			0x09

int main()
{
	UINT8 usb_state;
	UINT8 response;
	UINT16 next_state_time;
	UINT8 datagram[2];
	UINT16 bytes_available;
	UINT8 temp;
	
	UINT8 rx_idx;
	UINT8 tx_idx;
	UINT8 ep_sel;
	UINT8 ep_idx;
	UINT8 transfer_idx;
	UINT8 transfer_len;
	
	rcc_set_clk_freq(RCC_CLK_FREQ_48M);
	
	// Configure UART pins
	gpio_set_port_mode(GPIO_PORT_MODE_OC, GPIO_PORT_0);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_0);
	gpio_set_pin_mode(GPIO_MODE_OC_PU, GPIO_PORT_0, GPIO_PIN_2);	//RXD0
	gpio_set_pin_mode(GPIO_MODE_OC_PU_PULSE, GPIO_PORT_0, GPIO_PIN_3);	//TXD0
	
	gpio_set_port_mode(GPIO_PORT_MODE_OC, GPIO_PORT_2);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_2);
	gpio_set_pin_mode(GPIO_MODE_OC_PU, GPIO_PORT_2, GPIO_PIN_6);	//RXD1
	gpio_set_pin_mode(GPIO_MODE_OC_PU_PULSE, GPIO_PORT_2, GPIO_PIN_7);	//TXD1
	
	// Configure LED pins
	gpio_set_port_mode(GPIO_PORT_MODE_PP, GPIO_PORT_1);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_1);
	gpio_set_pin_mode(GPIO_MODE_OUTPUT_PP, GPIO_PORT_1, GPIO_PIN_4 | GPIO_PIN_5);
	
	uart0_init(TIMER_1, BAUD_RATE, UART_0_P02_P03);
	uart1_init(BAUD_RATE, UART_1_P26_P27);
	timer_init(TIMER_0, NULL);
	timer_set_period(TIMER_0, FREQ_SYS / 1000ul);	//period is 1ms
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	//Blink LED once
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_4);
	timer_long_delay(TIMER_0, 250);
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_4);
	timer_long_delay(TIMER_0, 250);
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_4);
	uart_write_string(UART_0, test_string);
	
	byte_to_hex(RESET_KEEP, line_buf);
	line_buf[2] = '\n';
	line_buf[3] = '\0';
	uart_write_string(UART_0, line_buf);
	
	usbh_init();
	usbh_in_transfer_nak_limit = 0;
	usbh_out_transfer_nak_limit = 0;
	
	timer_start(TIMER_0);
	next_state_time = timer_overflow_counts[TIMER_0];
	
	
	usb_state = S_DISCONNECTED;
	while(TRUE)
	{
		bytes_available = uart_bytes_available(UART_1);
		temp = uart_peek(UART_1);
		if((bytes_available >= 2) && (temp & 0x80))	//Handle write datagram
		{
			uart_read_bytes(UART_1, datagram, 2);
			
			switch(datagram[0] & 0x7F)
			{
				case 0x08:
					rx_idx = datagram[1];
					break;
				case 0x09:
					usbh_rx_buf[rx_idx & 0x3F] = datagram[1];
					rx_idx += 1;
					break;
				case 0x0A:
					tx_idx = datagram[1];
					break;
				case 0x0B:
					usbh_tx_buf[tx_idx & 0x3F] = datagram[1];
					tx_idx += 1;
					break;
				case 0x0C:
					ep_sel = datagram[1] & 0x07;
					break;
				case 0x0D:
					ep_idx = datagram[1] & 0x03;
					break;
				case 0x0E:
					((UINT8*)(ep_info[ep_sel & 0x07]))[ep_idx & 0x03] = datagram[1];
					break;
				case 0x0F:
					transfer_len = datagram[1];
					break;
				case 0x10:
					usb_state = datagram[1];
					break;
				case 0x11:
					response = datagram[1];
					break;
				case 0x12:
					transfer_idx = datagram[1];
					break;
				case 0x13:
					transfer_buf[transfer_idx] = datagram[1];
					transfer_idx += 1;
					break;
				case 0x14:
					response = usbh_control_transfer(ep_info[ep_sel], transfer_buf);
					break;
				case 0x15:
					response = usbh_in_transfer(ep_info[ep_sel], transfer_buf, (UINT16)transfer_len);
					break;
				case 0x16:
					response = usbh_out_transfer(ep_info[ep_sel], transfer_buf, (UINT16)transfer_len);
					break;
				case 0x17:
					response = usbh_transact(0);
					break;
				case 0x18:
					usbh_in_transfer_nak_limit &= 0xFF00;
					usbh_in_transfer_nak_limit |= (UINT16)datagram[1];
					break;
				case 0x19:
					usbh_in_transfer_nak_limit &= 0x00FF;
					usbh_in_transfer_nak_limit |= (UINT16)datagram[1] << 8;
					break;
				case 0x1A:
					usbh_out_transfer_nak_limit &= 0xFF00;
					usbh_out_transfer_nak_limit |= (UINT16)datagram[1];
					break;
				case 0x1B:
					usbh_out_transfer_nak_limit &= 0x00FF;
					usbh_out_transfer_nak_limit |= (UINT16)datagram[1] << 8;
					break;
			}
		}
		if(bytes_available && !(temp & 0x80))	//handle read datagram
		{
			datagram[0] = uart_read_byte(UART_1);
			
			switch(datagram[0])
			{
				case 0x00:	//device_id[0]
					datagram[0] = 'U';
					break;
				case 0x01:	//device_id[1]
					datagram[0] = 'S';
					break;
				case 0x02:	//device_id[2]
					datagram[0] = 'B';
					break;
				case 0x03:	//device_id[3]
					datagram[0] = '1';
					break;
				
				case 0x04:	//unique_id[0]
					E_DIS = 1;
					datagram[0] = *(UINT8 code*)0x0020;
					E_DIS = 0;
					break;
				case 0x05:	//unique_id[1]
					E_DIS = 1;
					datagram[0] = *(UINT8 code*)0x0021;
					E_DIS = 0;
					break;
				case 0x06:	//unique_id[2]
					E_DIS = 1;
					datagram[0] = *(UINT8 code*)0x0022;
					E_DIS = 0;
					break;
				case 0x07:	//unique_id[3]
					E_DIS = 1;
					datagram[0] = *(UINT8 code*)0x0023;
					E_DIS = 0;
					break;
				
				case 0x08:
					datagram[0] = rx_idx;
					break;
				case 0x09:
					datagram[0] = usbh_rx_buf[rx_idx];
					rx_idx += 1;
					break;
				case 0x0A:
					datagram[0] = tx_idx;
					break;
				case 0x0B:
					datagram[0] = usbh_tx_buf[tx_idx];
					tx_idx += 1;
					break;
				case 0x0C:
					datagram[0] = ep_sel;
					break;
				case 0x0D:
					datagram[0] = ep_idx;
					break;
				case 0x0E:
					datagram[0] = ((UINT8*)(ep_info[ep_sel]))[ep_idx];
					break;
				case 0x0F:
					datagram[0] = transfer_len;
					break;
				case 0x10:
					datagram[0] = usb_state;
					break;
				case 0x11:
					datagram[0] = response;
					break;
				case 0x12:
					datagram[0] = transfer_idx;
					break;
				case 0x13:
					datagram[0] = transfer_buf[transfer_idx];
					transfer_idx += 1;
					break;
				case 0x18:
					datagram[0] = (UINT8)usbh_in_transfer_nak_limit;
					break;
				case 0x19:
					datagram[0] = (UINT8)(usbh_in_transfer_nak_limit >> 8);
					break;
				case 0x1A:
					datagram[0] = (UINT8)usbh_out_transfer_nak_limit;
					break;
				case 0x1B:
					datagram[0] = (UINT8)(usbh_out_transfer_nak_limit >> 8);
					break;
			}
			uart_write_byte(UART_1, datagram[0]);
		}
		
		if(timer_overflow_counts[TIMER_0] >= next_state_time)
		{
			switch(usb_state)
			{
				case S_DISCONNECTED:
					next_state_time = timer_overflow_counts[TIMER_0] + 1;
					if(usbh_port0_is_attached())
					{
						uart_write_string(UART_0, str_attached);
						next_state_time = timer_overflow_counts[TIMER_0] + 50;
						usb_state = S_RESET_START;
					}
					break;
				case S_RESET_START:
					usbh_begin_port0_reset();
					next_state_time = timer_overflow_counts[TIMER_0] + 50;	//reset on root hub port must last at least 50 ms
					usb_state = S_RESET_END;
					break;
				case S_RESET_END:
					usbh_end_port0_reset();
					next_state_time = timer_overflow_counts[TIMER_0] + 1;
					usb_state = S_SET_SPEED;
					break;
				case S_SET_SPEED:
					if(usbh_port0_is_low_speed())
					{
						uart_write_string(UART_0, str_found_ls);
						usbh_configure_port0(USBH_LOW_SPEED | USBH_PORT_ENABLE);
						usbh_ctrl_set_low_speed();
					}
					else
					{
						uart_write_string(UART_0, str_found_fs);
						usbh_configure_port0(USBH_FULL_SPEED | USBH_PORT_ENABLE);
						usbh_ctrl_set_full_speed();
					}
					usbh_set_address(0x00);
					usb_state = S_CONNECTED;
					next_state_time = timer_overflow_counts[TIMER_0] + 1;
					break;
				case S_CONNECTED:
					gpio_set_pin(GPIO_PORT_1, GPIO_PIN_4);
					next_state_time = timer_overflow_counts[TIMER_0] + 10;
					usb_state = S_GET_DEV_DESCR;
					break;
				case S_GET_DEV_DESCR:
					next_state_time = timer_overflow_counts[TIMER_0] + 1;
					copy_request(usb_request_dev_descr);
					response = usbh_control_transfer(&ep0_info, transfer_buf);
				
					if(response == USB_PID_ACK)
					{
						uart_write_string(UART_0, str_descriptor);
						print_bytes_as_hex(transfer_buf, 18);
						usb_state = S_SET_ADDR;
					}
					else
					{
						uart_write_string(UART_0, str_response);
						byte_to_hex(response, line_buf);
						line_buf[2] = '\n';
						line_buf[3] = 0x00;
						uart_write_string(UART_0, line_buf);
						usb_state = S_IDLE;
					}

					gpio_toggle_pin(GPIO_PORT_1, GPIO_PIN_5);
					break;
				case S_SET_ADDR:
					next_state_time = timer_overflow_counts[TIMER_0] + 1;
					copy_request(usb_request_set_addr);
					response = usbh_control_transfer(&ep0_info, transfer_buf);
					
					if(response && usbh_is_toggle_ok())
					{
						uart_write_string(UART_0, str_response_ok);
						usbh_set_address(0x01);
						usb_state = S_CONFIGURE;
					}
					else
					{
						uart_write_string(UART_0, str_response);
						byte_to_hex(response, line_buf);
						line_buf[2] = '\n';
						line_buf[3] = 0x00;
						uart_write_string(UART_0, line_buf);
						usb_state = S_IDLE;
					}
					break;
				case S_CONFIGURE:
					next_state_time = timer_overflow_counts[TIMER_0] + 1;
					copy_request(usb_request_set_config);
					response = usbh_control_transfer(&ep0_info, transfer_buf);
				
					if(response && usbh_is_toggle_ok())
					{
						uart_write_string(UART_0, str_response_ok);
						usb_state = S_RUN;
					}
					else
					{
						uart_write_string(UART_0, str_response);
						byte_to_hex(response, line_buf);
						line_buf[2] = '\n';
						line_buf[3] = 0x00;
						uart_write_string(UART_0, line_buf);
						usb_state = S_IDLE;
					}				
					break;
				case S_IDLE, S_RUN:
					next_state_time = timer_overflow_counts[TIMER_0] + 1;
					if(!usbh_port0_is_attached())
					{
						uart_write_string(UART_0, str_detached);
						gpio_write_pin(GPIO_PORT_1, GPIO_PIN_4, 0);
						usb_state = S_DISCONNECTED;
					}
					break;
				default:
					break;
			}
		}
	}
}
