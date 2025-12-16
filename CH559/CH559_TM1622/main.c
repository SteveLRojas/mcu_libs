#include "CH559.H"
#include "CH559_RCC.h"
#include "CH559_GPIO.h"
#include "CH559_TIMER.h"
#include "CH559_USB_CDC.h"
#include "CH559_TM1622.h"

#define USE_EXT_CLK 0

char code str_bad_command[] = "Bad command!\n";
char code str_dragons[] = "DRAGONS?\n";

//Pins:
// RXD0 = P02
// TXD0 = P03
// TM_DATA = P05
// TM_WR = P06
// TM_CS = P07
// USB_DM = P50
// USB_DP = P51
// LED1 = P14
// LED2 = P15

char test_buf[11];

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
	
	// Configure UART pins
	gpio_set_port_mode(GPIO_PORT_MODE_OC, GPIO_PORT_0);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_0);
	gpio_set_pin_mode(GPIO_MODE_OC_PU, GPIO_PORT_0, GPIO_PIN_2);
	gpio_set_pin_mode(GPIO_MODE_OC_PU_PULSE, GPIO_PORT_0, GPIO_PIN_3);
	
	// Configure LED pins
	gpio_set_port_mode(GPIO_PORT_MODE_PP, GPIO_PORT_1);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_1);
	gpio_set_pin_mode(GPIO_MODE_OUTPUT_PP, GPIO_PORT_1, GPIO_PIN_4 | GPIO_PIN_5);
	
	// Configure display pins
	gpio_set_pin_mode(GPIO_MODE_OC_PU_PULSE, GPIO_PORT_0, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
	
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
	
	tm1622_init();

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
					for(count = 0; count < 64; count += 4)
					{
						tm1622_send_data(count, 0x01);
					}
					break;
				case 0x01:
					for(count = 0; count < 64; count += 4)
					{
						tm1622_send_data(count, 0x02);
					}
					break;
				case 0x02:
					for(count = 0; count < 64; count += 4)
					{
						tm1622_send_data(count, 0x04);
					}
					break;
				case 0x03:
					for(count = 0; count < 64; count += 4)
					{
						tm1622_send_data(count, 0x08);
					}
					break;
					
				case 0x04:
					for(count = 1; count < 64; count += 4)
					{
						tm1622_send_data(count, 0x01);
					}
					break;
				case 0x05:
					for(count = 1; count < 64; count += 4)
					{
						tm1622_send_data(count, 0x02);
					}
					break;
				case 0x06:
					for(count = 1; count < 64; count += 4)
					{
						tm1622_send_data(count, 0x04);
					}
					break;
				case 0x07:
					for(count = 1; count < 64; count += 4)
					{
						tm1622_send_data(count, 0x08);
					}
					break;
				
				case 0x08:
					for(count = 2; count < 64; count += 4)
					{
						tm1622_send_data(count, 0x01);
					}
					break;
				case 0x09:
					for(count = 2; count < 64; count += 4)
					{
						tm1622_send_data(count, 0x02);
					}
					break;
				case 0x0A:
					for(count = 2; count < 64; count += 4)
					{
						tm1622_send_data(count, 0x04);
					}
					break;
				case 0x0B:
					for(count = 2; count < 64; count += 4)
					{
						tm1622_send_data(count, 0x08);
					}
					break;
				
				case 0x0C:
					for(count = 3; count < 64; count += 4)
					{
						tm1622_send_data(count, 0x01);
					}
					break;
				case 0x0D:
					for(count = 3; count < 64; count += 4)
					{
						tm1622_send_data(count, 0x02);
					}
					break;
				case 0x0E:
					for(count = 3; count < 64; count += 4)
					{
						tm1622_send_data(count, 0x04);
					}
					break;
				case 0x0F:
					for(count = 3; count < 64; count += 4)
					{
						tm1622_send_data(count, 0x08);
					}
					break;
				
				case 0x10:
					for(count = 0; count < 64; count += 4)
					{
						tm1622_send_data(count, 0x00);
					}
					break;
				case 0x11:
					for(count = 1; count < 64; count += 4)
					{
						tm1622_send_data(count, 0x00);
					}
					break;
				case 0x12:
					for(count = 2; count < 64; count += 4)
					{
						tm1622_send_data(count, 0x00);
					}
					break;
				case 0x13:
					for(count = 3; count < 64; count += 4)
					{
						tm1622_send_data(count, 0x00);
					}
					break;
				
				case 0x14:
					for(count = 0; count < 64; count += 4)
					{
						tm1622_send_data(count, 0x0F);
					}
					break;
				case 0x15:
					for(count = 1; count < 64; count += 4)
					{
						tm1622_send_data(count, 0x0F);
					}
					break;
				case 0x16:
					for(count = 2; count < 64; count += 4)
					{
						tm1622_send_data(count, 0x0F);
					}
					break;
				case 0x17:
					for(count = 3; count < 64; count += 4)
					{
						tm1622_send_data(count, 0x0F);
					}
					break;
					
				case 0x18:
					tm1622_send_data(41, 0x01);
					break;
				case 0x19:
					tm1622_send_data(41, 0x02);
					break;
				case 0x1A:
					tm1622_send_data(41, 0x04);
					break;
				case 0x1B:
					tm1622_send_data(41, 0x08);
					break;
				
				case 0x1C:
					tm1622_send_data(43, 0x01);
					break;
				case 0x1D:
					tm1622_send_data(43, 0x02);
					break;
				case 0x1E:
					tm1622_send_data(43, 0x04);
					break;
				case 0x1F:
					tm1622_send_data(43, 0x08);
					break;
				
				case 0x20:
					tm1622_send_data(45, 0x01);
					break;
				case 0x21:
					tm1622_send_data(45, 0x02);
					break;
				case 0x22:
					tm1622_send_data(45, 0x04);
					break;
				case 0x23:
					tm1622_send_data(45, 0x08);
					break;
				
				case 0x24:
					for(count = 0; count < 64; count += 1)
					{
						tm1622_send_data(count, 0x00);
					}
					break;
				case 0x25:
					for(count = 0; count < 64; count += 1)
					{
						tm1622_send_data(count, 0x0F);
					}
					break;
				case 0x26:
					for(count = 40; count < 50; count += 1)
					{
						tm1622_send_data(count, 0x00);
					}
					break;
				case 0x27:
					for(count = 40; count < 50; count += 1)
					{
						tm1622_send_data(count, 0x0F);
					}
					break;
				
				case 0x28:
					for(count = 0; count < 10; ++count)
					{
						test_buf[count] = count;
					}
					test_buf[0] = 0x01;	//little hack
					test_buf[10] = 0x00;
					tm1622_write_string(9, test_buf);
					tm1622_update();
					break;
				case 0x29:
					for(count = 0; count < 10; ++count)
					{
						test_buf[count] = count + 10;
					}
					test_buf[10] = 0x00;
					tm1622_write_string(9, test_buf);
					tm1622_update();
					break;
				case 0x2A:
					for(count = 0; count < 10; ++count)
					{
						test_buf[count] = count + 20;
					}
					test_buf[10] = 0x00;
					tm1622_write_string(9, test_buf);
					tm1622_update();
					break;
				case 0x2B:
					for(count = 0; count < 10; ++count)
					{
						test_buf[count] = count + 30;
					}
					test_buf[10] = 0x00;
					tm1622_write_string(9, test_buf);
					tm1622_update();
					break;
				
				case 0x2C:
					for(count = 0; count < 10; ++count)
					{
						test_buf[count] = count + 40;
					}
					test_buf[10] = 0x00;
					tm1622_write_string(9, test_buf);
					tm1622_update();
					break;
				case 0x2D:
					for(count = 0; count < 10; ++count)
					{
						test_buf[count] = count + 50;
					}
					test_buf[10] = 0x00;
					tm1622_write_string(9, test_buf);
					tm1622_update();
					break;
				case 0x2E:
					for(count = 0; count < 10; ++count)
					{
						test_buf[count] = count + 60;
					}
					test_buf[10] = 0x00;
					tm1622_write_string(9, test_buf);
					tm1622_update();
					break;
				case 0x2F:
					for(count = 0; count < 10; ++count)
					{
						test_buf[count] = count + 70;
					}
					test_buf[10] = 0x00;
					tm1622_write_string(9, test_buf);
					tm1622_update();
					break;
				
				case 0x30:
					for(count = 0; count < 10; ++count)
					{
						test_buf[count] = count + 80;
					}
					test_buf[10] = 0x00;
					tm1622_write_string(9, test_buf);
					tm1622_update();
					break;
				case 0x31:
					for(count = 0; count < 10; ++count)
					{
						test_buf[count] = count + 90;
					}
					test_buf[10] = 0x00;
					tm1622_write_string(9, test_buf);
					tm1622_update();
					break;
				case 0x32:
					for(count = 0; count < 10; ++count)
					{
						test_buf[count] = count + 100;
					}
					test_buf[10] = 0x00;
					tm1622_write_string(9, test_buf);
					tm1622_update();
					break;
				case 0x33:
					for(count = 0; count < 10; ++count)
					{
						test_buf[count] = count + 110;
					}
					test_buf[10] = 0x00;
					tm1622_write_string(9, test_buf);
					tm1622_update();
					break;
				
				case 0x34:
					for(count = 0; count < 8; ++count)
					{
						test_buf[count] = count + 120;
					}
					test_buf[10] = 0x00;
					tm1622_write_string(9, test_buf);
					tm1622_digit_off(0);
					tm1622_digit_off(1);
					tm1622_update();
					break;
				case 0x35:
					for(count = 0; count < 10; ++count)
					{
						tm1622_set_digit(count, count);
					}
					break;
				case 0x36:
					tm1622_update();
					break;
				case 0x37:
					for(count = 0; count < 10; ++count)
					{
						tm1622_digit_off(count);
					}
					tm1622_write_string(9, str_dragons);
					tm1622_update();
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
