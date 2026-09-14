#include "CH559.H"
#include "CH559_RCC.h"
#include "CH559_GPIO.h"
#include "CH559_TIMER.h"
#include "CH559_LED.h"
#include "CH559_USB_CDC.h"
#include "CH559_UC1701.h"
#include "pseudo_random.h"

#define USE_EXT_CLK 0

char code str_unicorn[] = "Unicorn\n";
char code str_dragon[] = "Dragon\n";
char code str_wolf[] = "Wolf\n";
char code str_horse[] = "Horse\n";
char code str_lion[] = "Lion\n";
char code str_bear[] = "Bear\n";
char code str_bad_command[] = "Bad command!\n";

//Pins:
// DBGLED1 = P14
// DBGLED2 = P15
// DISP_RST = P20
// DISP_SDA = P21
// DISP_CD = P22
// DISP_SCK = P23
// DISP_CS = P24
// BL_LED = P32
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
	
	// Configure debug LED pins
	gpio_set_port_mode(GPIO_PORT_MODE_PP, GPIO_PORT_1);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_1);
	gpio_set_pin_mode(GPIO_MODE_OUTPUT_PP, GPIO_PORT_1, GPIO_PIN_4 | GPIO_PIN_5);
	
	// Configure display pins
	gpio_set_port_mode(GPIO_PORT_MODE_PP, GPIO_PORT_2);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_2);
	gpio_set_pin_mode(GPIO_MODE_OUTPUT_PP, GPIO_PORT_2, GPIO_PIN_4 | GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0);
	gpio_set_pin(GPIO_PORT_2, GPIO_PIN_4 | GPIO_PIN_0);
	
	// Configure WS2811 LED pins
	gpio_set_port_mode(GPIO_PORT_MODE_PP, GPIO_PORT_3);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_3);
	gpio_set_pin_mode(GPIO_MODE_OUTPUT_PP, GPIO_PORT_3, GPIO_PIN_2);
	gpio_clear_pin(GPIO_PORT_3, GPIO_PIN_2);
	
	timer_init(TIMER_0, NULL);
	timer_set_period(TIMER_0, FREQ_SYS / 1000ul);	//period is 1ms
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	led_init(15, LED_MODE_1_CHANNEL | LED_POL_NORMAL | LED_ORDER_LSB_FIRST);
	for(count = 0; count < LED_NUM_LEDS; ++count)
	{
		led_set_color(count, 0x00, 0x00, 0x00);
	}
	led_update();

	if((reset_type == RCC_RST_TYP_WDOG) || (reset_type == RCC_RST_TYP_SOFT))
	{
		rcc_delay_ms(500);
	}
	else
	{
		rcc_delay_ms(50);
	}
	
	//Blink LED once
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_4);
	timer_long_delay(TIMER_0, 250);
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_4);
	timer_long_delay(TIMER_0, 250);
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_4);
	
	cdc_init();
	cdc_set_serial_state(CDC_SS_TXCARRIER | CDC_SS_RXCARRIER);
	prev_control_line_state = cdc_control_line_state;
	while(!cdc_config);
	
	for(count = 0; count < LED_NUM_LEDS; ++count)
	{
		led_set_color(count, 0x20, 0x00, 0x00);
	}
	led_update();
	//led_update(NUM_LEDS);

	uc1701_init();
	pseudo_random_seed(0xDEADBEEF);
	
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
					led_set_color(0, 0x10, 0x10, 0x10);
					led_update();
					//led_update(NUM_LEDS);
					break;
				case 0x01:
					led_set_color(0, 0x20, 0x20, 0x20);
					led_update();
					//led_update(NUM_LEDS);
					break;
				case 0x02:
					led_set_color(0, 0x40, 0x40, 0x40);
					led_update();
					//led_update(NUM_LEDS);
					break;
				case 0x03:
					led_set_color(0, 0x80, 0x80, 0x80);
					led_update();
					//led_update(NUM_LEDS);
					break;
				case 0x04:
					pseudo_random_generate(8);
					temp = pseudo_random_get_byte();
					temp = temp >> 2;
					led_set_color_single(1, LED_CHANNEL_RED, temp);
					led_set_color_single(2, LED_CHANNEL_RED, temp);
					pseudo_random_generate(8);
					temp = pseudo_random_get_byte();
					temp = temp >> 2;
					led_set_color_single(1, LED_CHANNEL_GREEN, temp);
					led_set_color_single(2, LED_CHANNEL_GREEN, temp);
					pseudo_random_generate(8);
					temp = pseudo_random_get_byte();
					temp = temp >> 2;
					led_set_color_single(1, LED_CHANNEL_BLUE, temp);
					led_set_color_single(2, LED_CHANNEL_BLUE, temp);
					led_update();
					//led_update(NUM_LEDS);
					break;
				case 0x05:
					uc1701_init();
					break;
				case 0x06:
					uc1701_update_display();
					break;
				case 0x07:
					uc1701_clear_display(0x00);
					break;
				case 0x08:
					uc1701_clear_display(0xAA);
					break;
				case 0x09:
					uc1701_clear_display(0x55);
					break;
				case 0x0A:
					uc1701_clear_display(0xFF);
					break;
				case 0x0B:
					uc1701_set_pixel(20, 20, 0x01);
					break;
				case 0x0C:
					uc1701_set_pixel(20, 20, 0x00);
					break;
				case 0x0D:
					uc1701_draw_text(0, 0, str_unicorn);
					break;
				case 0x0E:
					uc1701_draw_text(1, 0, str_dragon);
					break;
				case 0x0F:
					uc1701_draw_text(2, 0, str_wolf);
					break;
				case 0x10:
					uc1701_draw_text(3, 0, str_horse);
					break;
				case 0x11:
					uc1701_draw_text(4, 0, str_lion);
					break;
				case 0x12:
					uc1701_draw_text(5, 0, str_bear);
					break;
				case 0x13:
					uc1701_clear_page(0, 0x00);
					break;
				case 0x14:
					uc1701_clear_page(1, 0x00);
					break;
				case 0x15:
					uc1701_clear_page(2, 0x00);
					break;
				case 0x16:
					uc1701_clear_page(3, 0x00);
					break;
				case 0x17:
					uc1701_clear_page(4, 0x00);
					break;
				case 0x18:
					uc1701_clear_page(5, 0x00);
					break;
				case 0x19:
					uc1701_update_page(0);
					break;
				case 0x1A:
					uc1701_update_page(1);
					break;
				case 0x1B:
					uc1701_update_page(2);
					break;
				case 0x1C:
					uc1701_update_page(3);
					break;
				case 0x1D:
					uc1701_update_page(4);
					break;
				case 0x1E:
					uc1701_update_page(5);
					break;
				case 0x1F:
					uc1701_scroll_text_up(1);
					break;
				case 0x20:
					uc1701_scroll_text_up(2);
					break;
				case 0x21:
					for(count = 0; count < UC1701_NUM_COLUMNS; ++count)
					{
						temp = count >> 2;
						if(temp >= UC1701_NUM_ROWS)
							temp = UC1701_NUM_ROWS - 1;
						uc1701_set_pixel(temp, count, 0x01);
					}
					uc1701_update_display();
					break;
				case 0x22:
					for(count = 0; count < UC1701_NUM_COLUMNS; ++count)
					{
						temp = count >> 1;
						if(temp >= UC1701_NUM_ROWS)
							temp = UC1701_NUM_ROWS - 1;
						uc1701_set_pixel(temp, count, 0x01);
					}
					uc1701_update_display();
					break;
				case 0x23:
					for(count = 0; count < UC1701_NUM_COLUMNS; ++count)
					{
						temp = count;
						if(temp >= UC1701_NUM_ROWS)
							temp = UC1701_NUM_ROWS - 1;
						uc1701_set_pixel(temp, count, 0x01);
					}
					uc1701_update_display();
					break;
				case 0x24:
					uc1701_send_command(UC1701_COM_SET_ALL_PIX_ON | UC1701_ALL_PIX_ON);
					break;
				case 0x25:
					uc1701_send_command(UC1701_COM_SET_ALL_PIX_ON | UC1701_ALL_PIX_OFF);
					break;
				case 0x26:
					uc1701_send_command(UC1701_COM_SET_ELEC_VOL);
					uc1701_send_command(0x20);
					break;
				case 0x27:
					uc1701_send_command(UC1701_COM_SET_ELEC_VOL);
					uc1701_send_command(0x30);
					break;
				case 0x28:
					uc1701_send_command(UC1701_COM_SET_ELEC_VOL);
					uc1701_send_command(0x38);
					break;
				case 0x29:
					uc1701_send_command(UC1701_COM_SET_RES_RATIO | 0x03);
					break;
				case 0x2A:
					uc1701_send_command(UC1701_COM_SET_RES_RATIO | 0x04);
					break;
				case 0x2B:
					uc1701_send_command(UC1701_COM_SET_RES_RATIO | 0x05);
					break;
				case 0x2C:
					led_init(15, LED_MODE_1_CHANNEL | LED_POL_NORMAL | LED_ORDER_MSB_FIRST);
					break;
				case 0x2D:
					led_send_bytes(3 * 4);
					break;
				case 0x2E:
					led_buf[0] = 0x00;
					led_buf[1] = 0x00;
					led_send_bytes(1);
					break;
				case 0x2F:
					led_buf[0] = 0x00;
					led_buf[1] = 0xFF;
					led_send_bytes(1);
					break;
				case 0x30:
					for(count = 0; count < LED_NUM_LEDS; ++count)
					{
						led_set_color(count, 0x20, 0x00, 0x00);
					}
					led_update();
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
