#include "CH559.H"
#include "CH559_RCC.h"
#include "CH559_GPIO.h"
#include "CH559_TIMER.h"
#include "CH559_USB_CDC.h"
#include "CH559_RA6963.h"

#define USE_EXT_CLK 0

char code str_unicorn[] = "Unicorn\n";
char code str_dragon[] = "Dragon\n";
char code str_wolf[] = "Wolf\n";
char code str_horse[] = "Horse\n";
char code str_lion[] = "Lion\n";
char code str_bear[] = "Bear\n";
char code str_bad_command[] = "Bad command!\n";

//Pins:
// LED1 = P14
// LED2 = P15
// DISP_SD = P2
// DISP_CD = P40
// DISP_CE = P41
// DISP_RD = P42
// DISP_WR = P43
// USB_DM = P50
// USB_DP = P51


int main()
{
	UINT8 prev_control_line_state;
	UINT8 reset_type;
	UINT8 datagram[2];
	UINT8 temp;
	UINT8 count;
	
	rcc_set_clk_freq(RCC_CLK_FREQ_48M);
	reset_type = rcc_get_rst_typ();
	
#if USE_EXT_CLK	
	rcc_set_clk_src(RCC_CLK_OSC_EN);
	rcc_delay_ms(30);
	rcc_set_clk_src(RCC_CLK_SRC_EXT);
#endif

	if((reset_type == RCC_RST_TYP_WDOG) || (reset_type == RCC_RST_TYP_SOFT))
	{
		rcc_delay_ms(500);
	}
	else
	{
		rcc_delay_ms(50);
	}
	
	// Configure LED pins
	gpio_set_port_mode(GPIO_PORT_MODE_PP, GPIO_PORT_1);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_1);
	gpio_set_pin_mode(GPIO_MODE_OUTPUT_PP, GPIO_PORT_1, GPIO_PIN_4 | GPIO_PIN_5);
	
	// Configure display pins
	gpio_set_port_strength(GPIO_PORT_STRENGTH_5, GPIO_PORT_2);
	gpio_set_port_mode(GPIO_PORT_MODE_PP, GPIO_PORT_4);
	gpio_set_p4_strength(GPIO_PORT_STRENGTH_20);
	gpio_set_pin_mode(GPIO_MODE_OUTPUT_PP, GPIO_PORT_4, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
	gpio_set_pin(GPIO_PORT_4, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
	
	timer_init(TIMER_0, NULL);
	timer_set_period(TIMER_0, FREQ_SYS / 1000ul);	//period is 1ms
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	// Blink LED once
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_4);
	timer_long_delay(TIMER_0, 250);
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_4);
	timer_long_delay(TIMER_0, 250);
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_4);
	
	cdc_init();
	cdc_set_serial_state(CDC_SS_TXCARRIER | CDC_SS_RXCARRIER);
	prev_control_line_state = cdc_control_line_state;
	while(!cdc_config);
	
	ra6963_init();

	while(TRUE)
	{
		if(cdc_bytes_available() >= 2)
		{
			temp = cdc_peek();
			if((temp == '\r') || (temp == '\n'))
			{
				(void)cdc_read_byte();
				continue;
			}
			
			cdc_read_bytes(datagram, 2);
			for(count = 0; count < 2; ++count)
			{
				temp = datagram[count];
				if(temp >= '0' && temp <= '9')  //convert numbers
					temp = temp - '0';
				else if(temp >= 'A' && temp <= 'F')   //convert uppercase letters
					temp = temp - 'A' + 10;
				else if(temp >= 'a' && temp <= 'f')   //convert the annoying lowercase letters
					temp = temp - 'a' + 10;
				else
					continue;

				RESET_KEEP = RESET_KEEP << 4;
				RESET_KEEP = RESET_KEEP | temp;
			}
			
			gpio_toggle_pin(GPIO_PORT_1, GPIO_PIN_5);
			switch(RESET_KEEP)
			{
				case 0x00:
					ra6963_update_display();
					break;
				case 0x01:
					ra6963_clear_display(0x00);
					break;
				case 0x02:
					ra6963_clear_display(0xAA);
					break;
				case 0x03:
					ra6963_clear_display(0x55);
					break;
				case 0x04:
					ra6963_clear_display(0xFF);
					break;
				case 0x05:
					ra6963_set_pixel(20, 20, 0x01);
					break;
				case 0x06:
					ra6963_set_pixel(20, 20, 0x00);
					break;
				case 0x07:
					ra6963_draw_text(0, 0, str_unicorn);
					break;
				case 0x08:
					ra6963_draw_text(8, 0, str_dragon);
					break;
				case 0x09:
					ra6963_draw_text(16, 0, str_wolf);
					break;
				case 0x0A:
					ra6963_draw_text(24, 0, str_horse);
					break;
				case 0x0B:
					ra6963_draw_text(16, 8, str_lion);
					break;
				case 0x0C:
					ra6963_draw_text(24, 12, str_bear);
					break;
				case 0x0D:
					ra6963_clear_rows(0, 8, 0x00);
					break;
				case 0x0E:
					ra6963_clear_rows(8, 16, 0x00);
					break;
				case 0x0F:
					ra6963_clear_rows(16, 24, 0x00);
					break;
				case 0x10:
					ra6963_clear_rows(24, 32, 0x00);
					break;
				case 0x11:
					ra6963_clear_rows(8, 24, 0x00);
					break;
				case 0x12:
					ra6963_clear_rows(0, 32, 0x00);
					break;
				case 0x13:
					ra6963_update_rows(0, 8);
					break;
				case 0x14:
					ra6963_update_rows(8, 16);
					break;
				case 0x15:
					ra6963_update_rows(16, 24);
					break;
				case 0x16:
					ra6963_update_rows(24, 32);
					break;
				case 0x17:
					ra6963_update_rows(8, 24);
					break;
				case 0x18:
					ra6963_update_rows(0, 32);
					break;
				case 0x19:
					ra6963_scroll_up(8);
					ra6963_clear_rows(24, 32, 0x00);
					break;
				case 0x1A:
					ra6963_scroll_up(16);
					ra6963_clear_rows(16, 32, 0x00);
					break;
				case 0x1B:
					ra6963_init();
					break;
				case 0x1C:
					ra6963_clear_display(0x00);
					ra6963_set_pixel(4,   4, 1);
					ra6963_set_pixel(4,  12, 1);
					ra6963_set_pixel(4,  20, 1);
					ra6963_set_pixel(4,  28, 1);
					ra6963_set_pixel(4,  36, 1);
					ra6963_set_pixel(4,  44, 1);
					ra6963_set_pixel(4,  52, 1);
					ra6963_set_pixel(4,  60, 1);
					ra6963_update_display();
					break;
				case 0x1D:
					for(temp = 0; temp < 32; ++temp)
					{
						ra6963_set_pixel(temp, temp, 0x01);
					}
					ra6963_update_display();
					break;
				case 0x1E:
					for(temp = 0; temp < 128; ++temp)
					{
						ra6963_set_pixel(temp >> 2, temp, 0x01);
					}
					ra6963_update_display();
					break;
				case 0x1F:
					for(temp = 0; temp < 128; ++temp)
					{
						ra6963_set_pixel(temp >> 1, temp, 0x01);
					}
					ra6963_update_display();
					break;
				default:
					cdc_write_string(str_bad_command);
					break;
			}
			gpio_write_pin(GPIO_PORT_1, GPIO_PIN_4, gpio_read_pin(GPIO_PORT_1, GPIO_PIN_5));
		}
		
		if(prev_control_line_state != cdc_control_line_state)
		{
			cdc_set_serial_state(cdc_control_line_state & 3);
			prev_control_line_state = cdc_control_line_state;
		}
	}
}
