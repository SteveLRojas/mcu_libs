#include "CH547.H"
#include "CH547_RCC.h"
#include "CH547_GPIO.h"
#include "CH547_TIMER.h"
#include "CH547_USB_CDC.h"
#include "CH547_HX8346.h"

#define USE_EXT_CLK 0

char code str_unicorn[] = "Unicorn\n";
char code str_dragon[] = "Dragon\n";
char code str_wolf[] = "Wolf\n";
char code str_horse[] = "Horse\n";
char code str_lion[] = "Lion\n";
char code str_bear[] = "Bear\n";
char code str_bad_command[] = "Bad command!\n";

//Pins:
// DISP_DB_L = P0
// DISP_DB_H = P1
// LED0 = P20
// LED1 = P21
// LED2 = P22
// LED3 = P23
// DISP_RD = P30
// DISP_CS = P31
// DISP_RST = P32
// DISP_RS = P44
// DISP_WR = P45
// USB_DM = P50
// USB_DP = P51


int main()
{
	UINT8 data prev_control_line_state;
	UINT8 data reset_type;
	UINT8 idata datagram[2];
	UINT8 data temp;
	UINT8 data count;
	UINT16 data row;
	UINT16 data col;
	UINT16 image_buf[32];
	UINT32 bytes_received;
	UINT16 bytes_available;
	
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
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_3, GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0);	//DISP_RST, DISP_CS, DISP_RD
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_4, GPIO_PIN_5 | GPIO_PIN_4);	//DISP_WR, DISP_RS
	
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
	
	hx8346_init();

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
					hx8346_fill_color = 0xF800;
					hx8346_fill_display();
					break;
				case 0x01:
					hx8346_fill_color = 0x07E0;
					hx8346_fill_display();
					break;
				case 0x02:
					hx8346_fill_color = 0x001F;
					hx8346_fill_display();
					break;
				case 0x03:
					hx8346_fill_color = 0xFFFF;
					hx8346_fill_display();
					break;
				case 0x04:
					hx8346_fill_color = 0x0000;
					hx8346_fill_display();
					break;
				case 0x05:
					hx8346_text_bg_color = 0x0000;
					hx8346_text_fg_color = 0x27E4;
					hx8346_draw_text(0, 0, str_unicorn);
					break;
				case 0x06:
					hx8346_text_bg_color = 0x0000;
					hx8346_text_fg_color = 0x27E4;
					hx8346_draw_text(8, 0, str_dragon);
					break;
				case 0x07:
					hx8346_text_bg_color = 0x0000;
					hx8346_text_fg_color = 0x27E4;
					hx8346_draw_text(16, 0, str_wolf);
					break;
				case 0x08:
					hx8346_text_bg_color = 0x0000;
					hx8346_text_fg_color = 0x27E4;
					hx8346_draw_text(24, 0, str_horse);
					break;
				case 0x09:
					hx8346_text_bg_color = 0x0000;
					hx8346_text_fg_color = 0x27E4;
					hx8346_draw_text(32, 8, str_lion);
					break;
				case 0x0A:
					hx8346_text_bg_color = 0x0000;
					hx8346_text_fg_color = 0x27E4;
					hx8346_draw_text(40, 12, str_bear);
					break;
				case 0x0B:
					hx8346_fill_color = 0x87E0;
					hx8346_fill_rectangle(2, 2, 28, 236);
					break;
				case 0x0C:
					hx8346_fill_color = 0x87E0;
					hx8346_fill_rectangle(2, 290, 28, 236);
					break;
				case 0x0D:
					hx8346_fill_color = 0xFFE0;
					hx8346_fill_rectangle(2, 34, 252, 28);
					break;
				case 0x0E:
					hx8346_fill_color = 0xFFE0;
					hx8346_fill_rectangle(210, 34, 252, 28);
					break;
				case 0x0F:
					hx8346_fill_color = 0x07FF;
					hx8346_fill_rectangle(34, 34, 252, 172);
					break;
				case 0x10:
					hx8346_init();
					break;
				case 0x11:
					for(row = 0; row < HX8346_NUM_ROWS; ++row)
					{
						hx8346_set_pixel(row, row, 0x07E0);
					}
					break;
				case 0x12:
					for(col = 0; col < HX8346_NUM_COLUMNS; ++col)
					{
						row = col >> 2;
						if(row >= HX8346_NUM_ROWS)
							row = HX8346_NUM_ROWS - 1;
						hx8346_set_pixel(row, col, 0xF800);
					}
					break;
				case 0x13:
					for(col = 0; col < HX8346_NUM_COLUMNS; ++col)
					{
						row = col >> 1;
						if(row >= HX8346_NUM_ROWS)
							row = HX8346_NUM_ROWS - 1;
						hx8346_set_pixel(row, col, 0x001F);
					}
					break;
				case 0x14:
					// receive raw image data
					bytes_received = 0;

					hx8346_set_gram_window(HX8346_DA_ROW_START, HX8346_DA_COL_START, HX8346_NUM_COLUMNS, HX8346_NUM_ROWS);

					while(bytes_received < (HX8346_NUM_PIXELS * 2ul))
					{
						while(!(bytes_available = cdc_bytes_available() & 0xFFFE));
						if(bytes_available > 64)
							bytes_available = 64;
						cdc_read_bytes((UINT8*)image_buf, bytes_available);
						bytes_received += bytes_available;
						hx8346_write_data(image_buf, bytes_available >> 1);
					}
					break;
				case 0x15:
					temp = hx8346_read_reg(0x67);
					cdc_write_byte(temp);
					break;
				case 0x16:
					hx8346_fill_color = 0x07FF;
					hx8346_fill_rectangle(0, 0, 256, 128);
					break;
				case 0x17:
					hx8346_fill_color = 0xF81F;
					hx8346_fill_rectangle(0, 0, 255, 127);
					break;
				case 0x18:
					hx8346_fill_color = 0xFD00;
					hx8346_fill_rectangle(100, 100, 100, 100);
					break;
				case 0x19:
					hx8346_fill_color = 0xF81B;
					hx8346_fill_rectangle(0, 0, 320, 240);
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
