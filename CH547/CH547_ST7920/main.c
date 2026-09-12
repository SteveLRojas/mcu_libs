#include "CH547.H"
#include "CH547_RCC.h"
#include "CH547_GPIO.h"
#include "CH547_TIMER.h"
#include "CH547_USB_CDC.h"
#include "CH547_ST7920.h"

#define USE_EXT_CLK 0

char code str_unicorn[] = "Unicorn\n";
char code str_dragon[] = "Dragon\n";
char code str_wolf[] = "Wolf\n";
char code str_horse[] = "Horse\n";
char code str_lion[] = "Lion\n";
char code str_bear[] = "Bear\n";
char code str_bad_command[] = "Bad command!\n";

//Pins:
// DISP_CS = P14
// DISP_SID = P15
// DISP_SCLK = P17
// USB_DM = P50
// USB_DP = P51
// LED0 = P20
// LED1 = P21
// LED2 = P22
// LED3 = P23

int main()
{
	UINT8 data prev_control_line_state;
	UINT8 data reset_type;
	UINT8 data datagram[2];
	UINT8 data temp;
	UINT8 data count;
	
	rcc_set_clk_freq(RCC_CLK_FREQ_24M);
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
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_2, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0);	//LED3, LED2, LED1, LED0
	
	// Configure display pins
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_7 | GPIO_PIN_5 | GPIO_PIN_4);	//DISP_SCLK, DISP_SID, DISP_CS
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_4);
	
	timer_init(TIMER_0, NULL);
	timer_set_period(TIMER_0, FREQ_SYS / 1000ul);	//period is 1ms
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	// Blink LED once
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0);
	timer_long_delay(TIMER_0, 250);
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0);
	timer_long_delay(TIMER_0, 250);
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0);
	
	cdc_init();
	cdc_set_serial_state(CDC_SS_TXCARRIER | CDC_SS_RXCARRIER);
	prev_control_line_state = cdc_control_line_state;
	while(!cdc_config);
	
	st7920_init();

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
			
			gpio_toggle_pin(GPIO_PORT_2, GPIO_PIN_0);
			switch(RESET_KEEP)
			{
				case 0x00:
					st7920_update_display();
					break;
				case 0x01:
					st7920_clear_display(0x00);
					break;
				case 0x02:
					st7920_clear_display(0xAA);
					break;
				case 0x03:
					st7920_clear_display(0x55);
					break;
				case 0x04:
					st7920_clear_display(0xFF);
					break;
				case 0x05:
					st7920_set_pixel(20, 20, 0x01);
					break;
				case 0x06:
					st7920_set_pixel(20, 20, 0x00);
					break;
				case 0x07:
					st7920_draw_text(0, 0, str_unicorn);
					break;
				case 0x08:
					st7920_draw_text(8, 0, str_dragon);
					break;
				case 0x09:
					st7920_draw_text(16, 0, str_wolf);
					break;
				case 0x0A:
					st7920_draw_text(24, 0, str_horse);
					break;
				case 0x0B:
					st7920_draw_text(16, 8, str_lion);
					break;
				case 0x0C:
					st7920_draw_text(24, 12, str_bear);
					break;
				case 0x0D:
					st7920_clear_rows(0, 8, 0x00);
					break;
				case 0x0E:
					st7920_clear_rows(8, 16, 0x00);
					break;
				case 0x0F:
					st7920_clear_rows(16, 24, 0x00);
					break;
				case 0x10:
					st7920_clear_rows(24, 32, 0x00);
					break;
				case 0x11:
					st7920_clear_rows(8, 24, 0x00);
					break;
				case 0x12:
					st7920_clear_rows(0, 32, 0x00);
					break;
				case 0x13:
					st7920_update_rows(0, 8);
					break;
				case 0x14:
					st7920_update_rows(8, 16);
					break;
				case 0x15:
					st7920_update_rows(16, 24);
					break;
				case 0x16:
					st7920_update_rows(24, 32);
					break;
				case 0x17:
					st7920_update_rows(8, 24);
					break;
				case 0x18:
					st7920_update_rows(0, 32);
					break;
				case 0x19:
					st7920_scroll_up(8);
					st7920_clear_rows(24, 32, 0x00);
					break;
				case 0x1A:
					st7920_scroll_up(16);
					st7920_clear_rows(16, 32, 0x00);
					break;
				case 0x1B:
					st7920_init();
					break;
				case 0x1C:
					st7920_clear_display(0x00);
					st7920_set_pixel(4,   4, 1);
					st7920_set_pixel(4,  12, 1);
					st7920_set_pixel(4,  20, 1);
					st7920_set_pixel(4,  28, 1);
					st7920_set_pixel(4,  36, 1);
					st7920_set_pixel(4,  44, 1);
					st7920_set_pixel(4,  52, 1);
					st7920_set_pixel(4,  60, 1);
					st7920_update_display();
					break;
				case 0x1D:
					for(count = 0; count < ST7920_NUM_ROWS; ++count)
					{
						st7920_set_pixel(count, count, 0x01);
					}
					st7920_update_display();
					break;
				case 0x1E:
					for(count = 0; count < ST7920_NUM_COLUMNS; ++count)
					{
						temp = count >> 2;
						if(temp >= ST7920_NUM_ROWS)
							temp = ST7920_NUM_ROWS - 1;
						st7920_set_pixel(temp, count, 0x01);
					}
					st7920_update_display();
					break;
				case 0x1F:
					for(count = 0; count < ST7920_NUM_COLUMNS; ++count)
					{
						temp = count >> 1;
						if(temp >= ST7920_NUM_ROWS)
							temp = ST7920_NUM_ROWS - 1;
						st7920_set_pixel(temp, count, 0x01);
					}
					st7920_update_display();
					break;
				default:
					cdc_write_string(str_bad_command);
					break;
			}
			gpio_write_pin(GPIO_PORT_2, GPIO_PIN_1, gpio_read_pin(GPIO_PORT_2, GPIO_PIN_0));
		}
		
		if(prev_control_line_state != cdc_control_line_state)
		{
			cdc_set_serial_state(cdc_control_line_state & 3);
			prev_control_line_state = cdc_control_line_state;
		}
	}
}
