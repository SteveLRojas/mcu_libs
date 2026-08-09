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
	UINT16 bytes_available;
	UINT8 temp;
	UINT8 count;
	UINT8 row;
	UINT8 col;
	UINT8 start;
	UINT8 end;
	
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
		bytes_available = cdc_bytes_available();
		temp = cdc_peek();
		if((bytes_available >= 2) && (temp & 0x80))	//Handle write datagram
		{
			cdc_read_bytes(datagram, 2);
			
			gpio_toggle_pin(GPIO_PORT_1, GPIO_PIN_5);
			switch(datagram[0] & 0x7F)
			{
				case 0x08:
					row = datagram[1];
					break;
				case 0x09:
					col = datagram[1];
					break;
				case 0x0A:
					start = datagram[1];
					break;
				case 0x0B:
					end = datagram[1];
					break;
				case 0x0C:
					ra6963_init();
					break;
				case 0x0D:
					ra6963_update_display();
					break;
				case 0x0E:
					ra6963_clear_display(datagram[1]);
					break;
				case 0x0F:
					ra6963_set_pixel(row, col, datagram[1]);
					break;
				case 0x10:
					ra6963_draw_text(row, datagram[1], str_unicorn);
					break;
				case 0x11:
					ra6963_draw_text(row, datagram[1], str_dragon);
					break;
				case 0x12:
					ra6963_draw_text(row, datagram[1], str_wolf);
					break;
				case 0x13:
					ra6963_draw_text(row, datagram[1], str_horse);
					break;
				case 0x14:
					ra6963_draw_text(row, datagram[1], str_lion);
					break;
				case 0x15:
					ra6963_draw_text(row, datagram[1], str_bear);
					break;
				case 0x16:
					ra6963_clear_rows(start, end, datagram[1]);
					break;
				case 0x17:
					ra6963_update_rows(start, end);
					break;
				case 0x18:
					ra6963_scroll_up(datagram[1]);
					ra6963_clear_rows(RA6963_NUM_ROWS - datagram[1], RA6963_NUM_ROWS, 0x00);
					break;		
				case 0x19:
					ra6963_clear_display(0x00);
					ra6963_set_pixel(row, col + 4, 1);
					ra6963_set_pixel(row, col + 12, 1);
					ra6963_set_pixel(row, col + 20, 1);
					ra6963_set_pixel(row, col + 28, 1);
					ra6963_set_pixel(row, col + 36, 1);
					ra6963_set_pixel(row, col + 44, 1);
					ra6963_set_pixel(row, col + 52, 1);
					ra6963_set_pixel(row, col + 60, 1);
					ra6963_update_display();
					break;
				case 0x1A:
					for(count = start; count < end; ++count)
					{
						ra6963_set_pixel(count, count, 0x01);
					}
					ra6963_update_display();
					break;
				case 0x1B:
					for(count = start; count < end; ++count)
					{
						ra6963_set_pixel(count >> 1, count, 0x01);
					}
					ra6963_update_display();
					break;
				case 0x1C:
					for(count = start; count < end; ++count)
					{
						ra6963_set_pixel(count >> 2, count, 0x01);
					}
					ra6963_update_display();
					break;
				case 0x1D:
					for(count = start; count < end; ++count)
					{
						ra6963_set_pixel(count, datagram[1], 0x01);
					}
					ra6963_update_display();
					break;
				case 0x1E:
					for(count = start; count < end; ++count)
					{
						ra6963_set_pixel(datagram[1], count, 0x01);
					}
					ra6963_update_display();
					break;
				case 0x1F:
					ra6963_status_mask = 0x03;
					ra6963_send_data(datagram[1]);
					ra6963_send_data(0x00);
					ra6963_send_command(RA6963_COM_SET_GRAPHIC_HOME_ADDR);
					break;
				case 0x20:
					ra6963_status_mask = 0x03;
					ra6963_send_data(datagram[1]);
					ra6963_send_data(0x00);
					ra6963_send_command(RA6963_COM_SET_GRAPHIC_AREA);
					break;
				default:
					cdc_write_string(str_bad_command);
					break;
			}
			gpio_write_pin(GPIO_PORT_1, GPIO_PIN_4, gpio_read_pin(GPIO_PORT_1, GPIO_PIN_5));
		}
		else if(bytes_available && !(temp & 0x80))	//handle read datagram
		{
			datagram[0] = cdc_read_byte();
			
			switch(datagram[0])
			{
				case 0x00:	//device_id[0]
					datagram[0] = '6';
					break;
				case 0x01:	//device_id[1]
					datagram[0] = '9';
					break;
				case 0x02:	//device_id[2]
					datagram[0] = '6';
					break;
				case 0x03:	//device_id[3]
					datagram[0] = '3';
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
					datagram[0] = row;
					break;
				case 0x09:
					datagram[0] = col;
					break;
				case 0x0A:
					datagram[0] = start;
					break;
				case 0x0B:
					datagram[0] = end;
					break;
			}
			
			cdc_write_byte(datagram[0]);
		}
		else if(bytes_available)	//datagrams not received in a single transfer are ignored
		{
			cdc_read_bytes(datagram, bytes_available);	//get rid of the extra bytes
		}
		
		if(prev_control_line_state != cdc_control_line_state)
		{
			cdc_set_serial_state(cdc_control_line_state & 3);
			prev_control_line_state = cdc_control_line_state;
		}
	}
}
