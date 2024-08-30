#include "CH559.H"
#include "DEBUG.H"
#include "CH559_GPIO.h"
#include "CH559_TIMER.h"
#include "CH559_UART.h"
#include "CH559_USB_HOST.h"

//HINT: This is a very quick and dirty test program for the CH559 USB host library.
// It is designed to work only with one particular 3-button Logitech mouse, and is very likely to fail for other devices.

#define MOUSE_REPORT_LEN 4

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

typedef struct _MOUSE_REPORT {
	UINT8 buttons;
	UINT8 x_rel;
	UINT8 y_rel;
	UINT8 wheel_rel;
} mouse_report_t;

mouse_report_t prev_mouse_report = {0, 0, 0, 0};
mouse_report_t current_mouse_report;

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

UINT8 descr_buf[18];
usbh_ep_info_t ep0_info = {0, 8, USBH_IN_TOG_0 | USBH_IN_SEND_ACK, USBH_OUT_TOG_0 | USBH_OUT_EXPECT_ACK};
usbh_ep_info_t ep1_info = {1, 4, USBH_IN_TOG_0 | USBH_IN_SEND_ACK, USBH_OUT_TOG_0 | USBH_OUT_EXPECT_ACK};

char line_buf[10];

//Pins:
// RXD0 = P02
// TXD0 = P03
// LED1 = P14
// LED2 = P15
// USB_DM = P50
// USB_DP = P51

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
		/*if(len != 7)
		{
			RESET_KEEP |= 0x01;
			return;
		}*/
	}
}

#define S_RESET 0x00
#define S_DISCONNECTED 0x01
#define S_CONNECTED 0x02
#define S_IDLE 0x03
#define S_GET_DEV_DESCR 0x04
#define S_SET_ADDR 0x05
#define S_CONFIGURE 0x06
#define S_RUN 0x07

// test set address request
// test set configuration request
// test getting data from a mouse
int main()
{
	UINT8 usb_state;
	UINT8 response;
	
	CfgFsys();	//CH559 clock selection configuration
	
	// Configure UART pins
	gpio_set_port_mode(GPIO_PORT_MODE_OC, GPIO_PORT_0);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_0);
	gpio_set_pin_mode(GPIO_MODE_OC_PU, GPIO_PORT_0, GPIO_PIN_2);
	gpio_set_pin_mode(GPIO_MODE_OC_PU_PULSE, GPIO_PORT_0, GPIO_PIN_3);
	
	// Configure LED pins
	gpio_set_port_mode(GPIO_PORT_MODE_PP, GPIO_PORT_1);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_1);
	gpio_set_pin_mode(GPIO_MODE_OUTPUT_PP, GPIO_PORT_1, GPIO_PIN_4 | GPIO_PIN_5);
	
	uart0_init(TIMER_1, BAUD_RATE, UART_0_P02_P03);
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
	usbh_in_transfer_nak_limit = 0;	//no NAK retry for interrupt transfers
	usbh_out_transfer_nak_limit = 0xFFFF;
	
	
	usb_state = S_DISCONNECTED;
	while(TRUE)
	{	
		switch(usb_state)
		{
			case S_DISCONNECTED:
				if(usbh_port0_is_attached())
				{
					uart_write_string(UART_0, str_attached);
					timer_long_delay(TIMER_0, 50);
					usb_state = S_RESET;
				}
				break;
			case S_RESET:
				usbh_begin_port0_reset();
				timer_long_delay(TIMER_0, 50);	//reset on root hub port must last at least 50 ms
				usbh_end_port0_reset();
				
				timer_long_delay(TIMER_0, 1);
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
				break;
			case S_CONNECTED:
				gpio_write_pin(GPIO_PORT_1, GPIO_PIN_4, 1);
				timer_long_delay(TIMER_0, 10);
				usb_state = S_GET_DEV_DESCR;
				break;
			case S_IDLE:
				if(!usbh_port0_is_attached())
				{
					uart_write_string(UART_0, str_detached);
					gpio_write_pin(GPIO_PORT_1, GPIO_PIN_4, 0);
					usb_state = S_DISCONNECTED;
				}
				if(uart_bytes_available(UART_0))
				{
					switch(uart_read_byte(UART_0))
					{
						case 0x30:
							usb_state = S_RESET;
							break;
						case 0x31:
							usb_state = S_GET_DEV_DESCR;
							break;
						case 0x32:
							usb_state = S_SET_ADDR;
							break;
						default:
							uart_write_string(UART_0, str_bad_command);
							break;
					}
				}
				break;
			case S_GET_DEV_DESCR:
				copy_request(usb_request_dev_descr);
				response = usbh_control_transfer(&ep0_info, descr_buf);
			
				if(response == USB_PID_ACK)
				{
					uart_write_string(UART_0, str_descriptor);
					print_bytes_as_hex(descr_buf, 18);
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
				copy_request(usb_request_set_addr);
				response = usbh_control_transfer(&ep0_info, descr_buf);
				
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
				copy_request(usb_request_set_config);
				response = usbh_control_transfer(&ep0_info, descr_buf);
			
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
			case S_RUN:
				if(!usbh_port0_is_attached())
				{
					uart_write_string(UART_0, str_detached);
					gpio_write_pin(GPIO_PORT_1, GPIO_PIN_4, 0);
					usb_state = S_DISCONNECTED;
				}
				else
				{
					timer_long_delay(TIMER_0, 9);	// get report every 10 ms
					response = usbh_in_transfer(&ep1_info, (UINT8*)&current_mouse_report, MOUSE_REPORT_LEN);
					
					if(response == USB_PID_NAK)
					{
						break;	//no update, try again later
					}
					else if(response == USB_PID_DATA0 || response == USB_PID_DATA1)
					{
						if(current_mouse_report.buttons & ~prev_mouse_report.buttons)
						{
							uart_write_string(UART_0, str_button_pressed);
						}
						prev_mouse_report.buttons = current_mouse_report.buttons;
						
						if((current_mouse_report.x_rel != prev_mouse_report.x_rel) || (current_mouse_report.y_rel != prev_mouse_report.y_rel))
						{
							uart_write_string(UART_0, str_mouse_moved);
						}
						prev_mouse_report.x_rel = current_mouse_report.x_rel;
						prev_mouse_report.y_rel = current_mouse_report.y_rel;
						
						if(current_mouse_report.wheel_rel != prev_mouse_report.wheel_rel)
						{
							uart_write_string(UART_0, str_wheel_moved);
						}
						prev_mouse_report.wheel_rel = current_mouse_report.wheel_rel;
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
				}
				break;
			default:
				break;
		}
		
		timer_long_delay(TIMER_0, 1);
	}
}
