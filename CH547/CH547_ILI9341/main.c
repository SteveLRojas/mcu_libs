#include "CH547.H"
#include "CH547_RCC.h"
#include "CH547_GPIO.h"
#include "CH547_TIMER.h"
#include "CH547_USB_CDC.h"
#include "CH547_ILI9341.h"

#define USE_EXT_CLK 0

char code str_unicorn[] = "Unicorn\n";
char code str_dragon[] = "Dragon\n";
char code str_wolf[] = "Wolf\n";
char code str_horse[] = "Horse\n";
char code str_lion[] = "Lion\n";
char code str_bear[] = "Bear\n";
char code str_bad_command[] = "Bad command!\n";

//Pins:
// DISP_RST = P12
// DISP_DC = P13
// DISP_CS = P14
// DISP_SDI = P15
// DISP_SDO = P16
// DISP_SCL = P17
// LED0 = P20
// LED1 = P21
// LED2 = P22
// LED3 = P23
// USB_DM = P50
// USB_DP = P51

char code hex_table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};

void byte_to_hex(UINT8 value, char* buff)
{
	buff[0] = hex_table[(value >> 4) & 0x0F];
	buff[1] = hex_table[(value) & 0x0F];
	buff[2] = '\0';
}

void word_to_hex(UINT16 value, char* buff)
{
	buff[0] = hex_table[(value >> 12) & 0x0F];
	buff[1] = hex_table[(value >> 8) & 0x0F];
	buff[2] = hex_table[(value >> 4) & 0x0F];
	buff[3] = hex_table[(value) & 0x0F];
	buff[4] = '\0';
}

void cdc_print_bytes(UINT8* src, UINT8 num_bytes)
{
	UINT8 count = 0;
	UINT8 value;
	while(num_bytes)
	{
		value = *src;
		cdc_write_byte(hex_table[(value >> 4) & 0x0f]);
		cdc_write_byte(hex_table[(value) & 0x0f]);
		--num_bytes;
		++count;
		++src;
		if(!(count & 0x0F))
		{
			cdc_write_byte('\n');
		}
	}
}

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
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_7 | GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 | GPIO_PIN_2);	//DISP_SCL, DISP_SDI, DISP_CS, DISP_DC, DISP_RST
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_1, GPIO_PIN_6);	//DISP_SDO
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
	
	ili9341_init();

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
					ili9341_fill_color = 0xF800;
					ili9341_fill_display();
					break;
				case 0x01:
					ili9341_fill_color = 0x07E0;
					ili9341_fill_display();
					break;
				case 0x02:
					ili9341_fill_color = 0x001F;
					ili9341_fill_display();
					break;
				case 0x03:
					ili9341_fill_color = 0xFFFF;
					ili9341_fill_display();
					break;
				case 0x04:
					ili9341_fill_color = 0x0000;
					ili9341_fill_display();
					break;
				case 0x05:
					ili9341_text_bg_color = 0x0000;
					ili9341_text_fg_color = 0x27E4;
					ili9341_draw_text(0, 0, str_unicorn);
					break;
				case 0x06:
					ili9341_text_bg_color = 0x0000;
					ili9341_text_fg_color = 0x27E4;
					ili9341_draw_text(8, 0, str_dragon);
					break;
				case 0x07:
					ili9341_text_bg_color = 0x0000;
					ili9341_text_fg_color = 0x27E4;
					ili9341_draw_text(16, 0, str_wolf);
					break;
				case 0x08:
					ili9341_text_bg_color = 0x0000;
					ili9341_text_fg_color = 0x27E4;
					ili9341_draw_text(24, 0, str_horse);
					break;
				case 0x09:
					ili9341_text_bg_color = 0x0000;
					ili9341_text_fg_color = 0x27E4;
					ili9341_draw_text(32, 8, str_lion);
					break;
				case 0x0A:
					ili9341_text_bg_color = 0x0000;
					ili9341_text_fg_color = 0x27E4;
					ili9341_draw_text(40, 12, str_bear);
					break;
				case 0x0B:
					ili9341_fill_color = 0x87E0;
					ili9341_fill_rectangle(2, 2, 28, 236);
					break;
				case 0x0C:
					ili9341_fill_color = 0x87E0;
					ili9341_fill_rectangle(2, 290, 28, 236);
					break;
				case 0x0D:
					ili9341_fill_color = 0xFFE0;
					ili9341_fill_rectangle(2, 34, 252, 28);
					break;
				case 0x0E:
					ili9341_fill_color = 0xFFE0;
					ili9341_fill_rectangle(210, 34, 252, 28);
					break;
				case 0x0F:
					ili9341_fill_color = 0x07FF;
					ili9341_fill_rectangle(34, 34, 252, 172);
					break;
				case 0x10:
					ili9341_init();
					break;
				case 0x11:
					for(row = 0; row < ILI9341_NUM_ROWS; ++row)
					{
						ili9341_set_pixel(row, row, 0x07E0);
					}
					break;
				case 0x12:
					for(col = 0; col < ILI9341_NUM_COLUMNS; ++col)
					{
						row = col >> 2;
						if(row >= ILI9341_NUM_ROWS)
							row = ILI9341_NUM_ROWS - 1;
						ili9341_set_pixel(row, col, 0xF800);
					}
					break;
				case 0x13:
					for(col = 0; col < ILI9341_NUM_COLUMNS; ++col)
					{
						row = col >> 1;
						if(row >= ILI9341_NUM_ROWS)
							row = ILI9341_NUM_ROWS - 1;
						ili9341_set_pixel(row, col, 0x001F);
					}
					break;
				case 0x14:
					// receive raw image data
					bytes_received = 0;

					ili9341_set_gram_window(ILI9341_DA_ROW_START, ILI9341_DA_COL_START, ILI9341_NUM_COLUMNS, ILI9341_NUM_ROWS);

					while(bytes_received < (ILI9341_NUM_PIXELS * 2ul))
					{
						while(!(bytes_available = cdc_bytes_available() & 0xFFFE));
						if(bytes_available > 64)
							bytes_available = 64;
						cdc_read_bytes((UINT8*)image_buf, bytes_available);
						bytes_received += bytes_available;
						ili9341_write_gram(image_buf, bytes_available >> 1);
					}
					break;
				case 0x15:
					ili9341_write_command(ILI9341_COM_READ_ID4);
					ili9341_read_data((UINT8*)image_buf, 4);
					cdc_print_bytes((UINT8*)image_buf, 4);
					break;
				case 0x16:
					ili9341_fill_color = 0x07FF;
					ili9341_fill_rectangle(0, 0, 256, 128);
					break;
				case 0x17:
					ili9341_fill_color = 0xF81F;
					ili9341_fill_rectangle(0, 0, 255, 127);
					break;
				case 0x18:
					ili9341_fill_color = 0xFD00;
					ili9341_fill_rectangle(100, 100, 100, 100);
					break;
				case 0x19:
					ili9341_fill_color = 0xF81B;
					ili9341_fill_rectangle(0, 0, 320, 240);
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
