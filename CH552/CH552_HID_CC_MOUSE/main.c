#include "CH552.H"
#include "System.h"
#include "CH552_GPIO.h"
#include "CH552_UART.h"
#include "CH552_TIMER.h"
#include "CH552_HID_CC_MOUSE.h"
	
char code test_string[] = "Unicorn\n";

//Pins:
// LED = P11
// TEST = P14
// RXD = P30
// TXD = P31
// UDP = P36
// UDM = P37

void usb_halt(UINT8 keep)
{
	RESET_KEEP = keep;
	USB_CTRL = 0;
	IE_USB = 0;
	
	while(TRUE)
	{
		gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_1);
		mDelaymS(50);
		gpio_set_pin(GPIO_PORT_1, GPIO_PIN_1);
		mDelaymS(50);
	}
}

void byte_to_hex(UINT8 value, char* buff)
{
	const char table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	buff[0] = table[(value >> 4) & 0x0f];
	buff[1] = table[(value) & 0x0f];
	buff[2] = '\0';
}

int main()
{
	UINT8 temp;
	char last_keep_str[4];
	UINT8 mouse_timer = 0;
	UINT8 cc_timer = 0;
	
	CfgFsys();
	
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_1 | GPIO_PIN_4);
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_3, GPIO_PIN_1);
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_3, GPIO_PIN_0);
	
	uart0_init(TIMER_1, BAUD_RATE, UART_0_P30_P31);
	timer_init(TIMER_0, NULL);
	timer_set_period(TIMER_0, FREQ_SYS / 1000ul);	//period is 1ms
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	//Blink LED once
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_1);
	timer_long_delay(TIMER_0, 250);
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_1);
	timer_long_delay(TIMER_0, 250);
	uart_write_string(UART_0, test_string);
	
	byte_to_hex(RESET_KEEP, last_keep_str);
	last_keep_str[2] = '\n';
	last_keep_str[3] = '\0';
	uart_write_string(UART_0, last_keep_str);
	
	hid_init();
	
	while(TRUE)
	{	
		if(uart_bytes_available(UART_0))
		{
			temp = uart_read_byte(UART_0);
			
			switch(temp)
			{
				case 0x01:
					hid_mouse_press(HID_MOUSE_BTN_LEFT);
					timer_long_delay(TIMER_0, 100);
					hid_mouse_release(HID_MOUSE_BTN_LEFT);
					break;
				case 0x02:
					hid_mouse_press(HID_MOUSE_BTN_RIGHT);
					timer_long_delay(TIMER_0, 100);
					hid_mouse_release(HID_MOUSE_BTN_RIGHT);
					break;
				case 0x03:
					hid_mouse_press(HID_MOUSE_BTN_WHEEL);
					timer_long_delay(TIMER_0, 100);
					hid_mouse_release(HID_MOUSE_BTN_WHEEL);
					break;
				case 0x04:
					temp = uart_read_byte(UART_0);
					hid_mouse_move(temp, 0x00);
					break;
				case 0x05:
					temp = uart_read_byte(UART_0);
					hid_mouse_move(0x00, temp);
					break;
				case 0x06:
					temp = uart_read_byte(UART_0);
					hid_mouse_scroll(temp);
					break;
				case 0x07:
					hid_cc_press(HID_CC_BTN_VOL_MUTE);
					timer_long_delay(TIMER_0, 50);
					hid_cc_press(HID_CC_BTN_NONE);
					break;
				case 0x08:
					hid_cc_press(HID_CC_BTN_VOL_UP);
					timer_long_delay(TIMER_0, 50);
					hid_cc_press(HID_CC_BTN_NONE);
					break;
				case 0x09:
					hid_cc_press(HID_CC_BTN_VOL_DOWN);
					timer_long_delay(TIMER_0, 50);
					hid_cc_press(HID_CC_BTN_NONE);
					break;
				default:
					hid_cc_press(HID_CC_BTN_NONE);
					break;
			}

			gpio_write_pin(GPIO_PORT_1, GPIO_PIN_4, gpio_read_pin(GPIO_PORT_1, GPIO_PIN_1));
			gpio_write_pin(GPIO_PORT_1, GPIO_PIN_1, !gpio_read_pin(GPIO_PORT_1, GPIO_PIN_1));
		}
		
		timer_long_delay(TIMER_0, 1);
		++mouse_timer;
		++cc_timer;
		
		if(hid_mouse_idle_rate && ((mouse_timer >> 2) >= hid_mouse_idle_rate))
		{
			hid_mouse_send_report();
			mouse_timer = 0;
		}
		
		if(hid_cc_idle_rate && ((cc_timer >> 2) >= hid_cc_idle_rate))
		{
			hid_cc_send_report();
			cc_timer = 0;
		}
	}
}
