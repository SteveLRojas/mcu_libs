#include "ch32v20x.h"
#include "ch32v203_core.h"
#include "debug.h"
#include "fifo.h"
#include "ch32v203_uart.h"
#include "ch32v203_gpio.h"
#include "ch32v203_rcc.h"
#include "ch32v203_usbfsh.h"

//HINT: This is a very quick and dirty test program for the CH32V203 USB host library.
// It is designed to work only with one particular 3-button Logitech mouse, and is very likely to fail for other devices.

//Pins:
// LED3 = PA8
// TXD = PA9
// RXD = PA10
// UDM = PA11
// UDP = PA12
// LED2 = PC13
// LED1 = PC14
// LED0 = PC15

/* USB standard device request code */
#ifndef USB_GET_DESCRIPTOR
#define USB_GET_STATUS              0x00
#define USB_CLEAR_FEATURE           0x01
#define USB_SET_FEATURE             0x03
#define USB_SET_ADDRESS             0x05
#define USB_GET_DESCRIPTOR          0x06
#define USB_SET_DESCRIPTOR          0x07
#define USB_GET_CONFIGURATION       0x08
#define USB_SET_CONFIGURATION       0x09
#define USB_GET_INTERFACE           0x0A
#define USB_SET_INTERFACE           0x0B
#define USB_SYNCH_FRAME             0x0C
#endif

/* USB Descriptor Type */
#ifndef USB_DESCR_TYP_DEVICE
#define USB_DESCR_TYP_DEVICE        0x01
#define USB_DESCR_TYP_CONFIG        0x02
#define USB_DESCR_TYP_STRING        0x03
#define USB_DESCR_TYP_INTERF        0x04
#define USB_DESCR_TYP_ENDP          0x05
#define USB_DESCR_TYP_QUALIF        0x06
#define USB_DESCR_TYP_SPEED         0x07
#define USB_DESCR_TYP_OTG           0x09
#define USB_DESCR_TYP_BOS           0X0F
#define USB_DESCR_TYP_HID           0x21
#define USB_DESCR_TYP_REPORT        0x22
#define USB_DESCR_TYP_PHYSIC        0x23
#define USB_DESCR_TYP_CS_INTF       0x24
#define USB_DESCR_TYP_CS_ENDP       0x25
#define USB_DESCR_TYP_HUB           0x29
#endif

#define MOUSE_REPORT_LEN 4

typedef struct _MOUSE_REPORT
{
	uint8_t buttons;
	uint8_t x_rel;
	uint8_t y_rel;
	uint8_t wheel_rel;
} mouse_report_t;

mouse_report_t prev_mouse_report = {0, 0, 0, 0};
mouse_report_t current_mouse_report;

static const uint8_t usb_request_dev_descr[] =
{
	USB_REQ_TYP_IN | USB_REQ_TYP_STANDARD | USB_REQ_RECIP_DEVICE,
	USB_GET_DESCRIPTOR,
	0x00, USB_DESCR_TYP_DEVICE,
	0x00, 0x00,
	18, 0x00
};

static const uint8_t usb_request_set_addr[] =
{
	USB_REQ_TYP_OUT | USB_REQ_TYP_STANDARD | USB_REQ_RECIP_DEVICE,
	USB_SET_ADDRESS,
	0x01, 0x00,
	0x00, 0x00,
	0x00, 0x00
};

static const uint8_t usb_request_set_config[] =
{
	USB_REQ_TYP_OUT | USB_REQ_TYP_STANDARD | USB_REQ_RECIP_DEVICE,
	USB_SET_CONFIGURATION,
	0x01, 0x00,
	0x00, 0x00,
	0x00, 0x00
};

uint8_t descr_buf[18];
usbfsh_ep_info_t ep0_info = {0, 8, USBFSH_TOG_0 | USBFSH_SEND_ACK, USBFSH_TOG_0 | USBFSH_EXPECT_ACK};
usbfsh_ep_info_t ep1_info = {1, 4, USBFSH_TOG_0 | USBFSH_SEND_ACK, USBFSH_TOG_0 | USBFSH_EXPECT_ACK};

void copy_request(const uint8_t* source)
{
	uint8_t idx;

	for(idx = 0; idx < 8; ++idx)
	{
		usbfsh_tx_buf[idx] = source[idx];
	}
}

void print_bytes_as_hex(const uint8_t* source, uint8_t num_bytes)
{
	uint8_t idx;

	for(idx = 0; idx < num_bytes; ++idx)
	{
		printf("%02X: %02X\n", idx, source[idx]);
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

int main(void)
{
	rcc_apb2_clk_enable(RCC_AFIOEN | RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_TIM1EN | RCC_SPI1EN | RCC_USART1EN);
	rcc_apb1_clk_enable(RCC_TIM2EN | RCC_USBEN);
	rcc_ahb_clk_enable(RCC_USBFS);

	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_AFIO_PP, GPIO_PIN_9);
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_IN | GPIO_MODE_FLOAT_IN, GPIO_PIN_10);
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_8);
	gpio_set_mode(GPIOC, GPIO_DIR_SPD_OUT_2MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
	gpio_set_pin(GPIOB, GPIO_PIN_4);

    delay_init();
    uart_init(USART1, 115200);
    core_enable_irq(USART1_IRQn);

    printf("SYSCLK: %u\n", rcc_compute_sysclk_freq());
	printf("HCLK: %u\n", rcc_compute_hclk_freq());
	printf("PCLK1: %u\n", rcc_compute_pclk1_freq());
	printf("PCLK2: %u\n", rcc_compute_pclk2_freq());
	printf("ADCCLK: %u\n", rcc_compute_adcclk());

    // blink the led once
    gpio_set_pin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_set_pin(GPIOA, GPIO_PIN_8);
    delay_ms(100);
    gpio_clear_pin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_clear_pin(GPIOA, GPIO_PIN_8);
    delay_ms(100);
    gpio_write_pin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, 1);
    delay_ms(100);
    printf("Unicorn\n");

    usbfsh_init();
    usbfsh_in_transfer_nak_limit = 0;	//no NAK retry for interrupt transfers

    uint8_t usb_state = S_DISCONNECTED;
    uint8_t response;
	while(1)
	{
		switch(usb_state)
		{
		case S_DISCONNECTED:
			if(usbfsh_port_is_attached())
			{
				printf("USB device connected\n");
				delay_ms(50);
				usb_state = S_RESET;
			}
			break;
		case S_RESET:
			usbfsh_begin_port_reset();
			delay_ms(50);	//reset on root hub port must last at least 50 ms
			usbfsh_end_port_reset();

			delay_ms(1);
			if(usbfsh_port_is_low_speed())
			{
				printf("Found low speed device\n");
				usbfsh_configure_port(USBFSH_LOW_SPEED | USBFSH_PORT_ENABLE);
				usbfsh_ctrl_set_low_speed();
			}
			else
			{
				printf("Found full speed device\n");
				usbfsh_configure_port(USBFSH_FULL_SPEED | USBFSH_PORT_ENABLE);
				usbfsh_ctrl_set_full_speed();
			}
			usbfsh_set_address(0x00);
			usb_state = S_CONNECTED;
			break;
		case S_CONNECTED:
			gpio_clear_pin(GPIOA, GPIO_PIN_8);
			delay_ms(10);
			usb_state = S_GET_DEV_DESCR;
			break;
		case S_IDLE:
			if(!usbfsh_port_is_attached())
			{
				printf("USB device disconnected\n");
				gpio_set_pin(GPIOA, GPIO_PIN_8);
				usb_state = S_DISCONNECTED;
			}
			if(uart_bytes_available(uart1_rx_fifo))
			{
				switch(uart_read_byte(USART1, uart1_rx_fifo))
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
						printf("Got an invalid command\n");
						break;
				}
			}
			break;
		case S_GET_DEV_DESCR:
			copy_request(usb_request_dev_descr);
			response = usbfsh_control_transfer(&ep0_info, descr_buf);

			if(response == USB_PID_ACK)
			{
				printf("Descriptor:\n");
				print_bytes_as_hex(descr_buf, 18);
				usb_state = S_SET_ADDR;
			}
			else
			{
				printf("Response: %02X\n", response);
				usb_state = S_IDLE;
			}

			gpio_toggle_pin(GPIOC, GPIO_PIN_14);
			break;
		case S_SET_ADDR:
			copy_request(usb_request_set_addr);
			response = usbfsh_control_transfer(&ep0_info, descr_buf);

			if(response && usbfsh_is_toggle_ok())
			{
				printf("Response ok\n");
				usbfsh_set_address(0x01);
				usb_state = S_CONFIGURE;
			}
			else
			{
				printf("Response: %02X\n", response);
				usb_state = S_IDLE;
			}
			break;
		case S_CONFIGURE:
			copy_request(usb_request_set_config);
			response = usbfsh_control_transfer(&ep0_info, descr_buf);

			if(response && usbfsh_is_toggle_ok())
			{
				printf("Response ok\n");
				usb_state = S_RUN;
			}
			else
			{
				printf("Response: %02X\n", response);
				usb_state = S_IDLE;
			}
			break;
		case S_RUN:
			if(!usbfsh_port_is_attached())
			{
				printf("USB device disconnected\n");
				gpio_set_pin(GPIOA, GPIO_PIN_8);
				usb_state = S_DISCONNECTED;
			}
			else
			{
				delay_ms(9);	// get report every 10 ms
				response = usbfsh_in_transfer(&ep1_info, (uint8_t*)&current_mouse_report, MOUSE_REPORT_LEN);

				if(response == USB_PID_NAK)
				{
					break;	//no update, try again later
				}
				else if(response == USB_PID_DATA0 || response == USB_PID_DATA1)
				{
					if(current_mouse_report.buttons & ~prev_mouse_report.buttons)
					{
						printf("Button pressed: %02X\n", current_mouse_report.buttons);
					}
					prev_mouse_report.buttons = current_mouse_report.buttons;

					if((current_mouse_report.x_rel != prev_mouse_report.x_rel) || (current_mouse_report.y_rel != prev_mouse_report.y_rel))
					{
						printf("Mouse moved: %02X, %02X\n", current_mouse_report.x_rel, current_mouse_report.y_rel);
					}
					prev_mouse_report.x_rel = current_mouse_report.x_rel;
					prev_mouse_report.y_rel = current_mouse_report.y_rel;

					if(current_mouse_report.wheel_rel != prev_mouse_report.wheel_rel)
					{
						printf("Wheel moved: %02X\n", current_mouse_report.wheel_rel);
					}
					prev_mouse_report.wheel_rel = current_mouse_report.wheel_rel;
				}
				else
				{
					printf("Response: %02X\n", response);
					usb_state = S_IDLE;
				}
			}
			break;
		default:
			break;
		}

		delay_ms(1);
	}
}

