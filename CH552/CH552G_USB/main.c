#include "CH552.H"
#include "System.h"
#include "CH552_GPIO.h"
#include "CH552_UART.h"
#include "CH552_TIMER.h"
#include "CH552_USB.h"
#include "FTDI_serial.h"
	
char code test_string[] = "Booba\n";

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
	UINT8 bytes_to_transfer;
	UINT16 Uart_Timeout = 0;
	char last_keep_str[4];
	
	CfgFsys();	//CH559 clock selection configuration
	
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_1 | GPIO_PIN_4);
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_3, GPIO_PIN_1);
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_3, GPIO_PIN_0);
	
	uart0_init(TIMER_1, BUAD_RATE, UART_0_P30_P31);
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
	
	ftdi_init();
	
	while(TRUE)
	{	
		if(UsbConfig)
		{
			if(UpPoint1_Busy == 0)
			{
				bytes_to_transfer = uart_bytes_available(UART_0);
				if(bytes_to_transfer >= 62)
				{
					uart_read_bytes(UART_0, Ep1Buffer + 2, 62);
					UpPoint1_Busy = 1;
					usb_set_ep1_tx_len(64);
					usb_set_ep1_in_res(USB_IN_RES_EXPECT_ACK);
				}
				else if((UINT16) (sof_count - Uart_Timeout) >= Latency_Timer) //time out
				{
					Uart_Timeout = sof_count;
					uart_read_bytes(UART_0, Ep1Buffer + 2, bytes_to_transfer);
					UpPoint1_Busy = 1;
					usb_set_ep1_tx_len(bytes_to_transfer + 2);
					usb_set_ep1_in_res(USB_IN_RES_EXPECT_ACK);
				}
			}
			
			if(USBReceived)
			{
				gpio_write_pin(GPIO_PORT_1, GPIO_PIN_1, !gpio_read_pin(GPIO_PORT_1, GPIO_PIN_1));
				USBReceived = 0;
				uart_write_bytes(UART_0, Ep2Buffer, USBOutLength);
				usb_set_ep2_out_res(USB_OUT_RES_ACK);
			}
		}
	}
}

