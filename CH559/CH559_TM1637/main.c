#include "CH559.H"
#include "CH559_RCC.h"
#include "CH559_GPIO.h"
#include "CH559_TIMER.h"
#include "CH559_USB_CDC.h"
#include "CH559_TM1637.h"

#define USE_EXT_CLK 0

char code str_bad_command[] = "Bad command!\n";

//Pins:
// RXD0 = P02
// TXD0 = P03
// TM_DIO = P06
// TM_CLK = P07
// USB_DM = P50
// USB_DP = P51
// LED1 = P14
// LED2 = P15

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
	gpio_set_pin_mode(GPIO_MODE_OC_PU, GPIO_PORT_0, GPIO_PIN_6 | GPIO_PIN_7);
	
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
	
	tm1637_init(TM1637_OPT_PW_10_16);

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
					for(count = 0; count < TM1637_NUM_DIGITS; ++count)
					{
						tm1637_buf[count] = TM1637_SEG_A;
					}
					tm1637_update();
					break;
				case 0x01:
					for(count = 0; count < TM1637_NUM_DIGITS; ++count)
					{
						tm1637_buf[count] = TM1637_SEG_B;
					}
					tm1637_update();
					break;
				case 0x02:
					for(count = 0; count < TM1637_NUM_DIGITS; ++count)
					{
						tm1637_buf[count] = TM1637_SEG_C;
					}
					tm1637_update();
					break;
				case 0x03:
					for(count = 0; count < TM1637_NUM_DIGITS; ++count)
					{
						tm1637_buf[count] = TM1637_SEG_D;
					}
					tm1637_update();
					break;	
				case 0x04:
					for(count = 0; count < TM1637_NUM_DIGITS; ++count)
					{
						tm1637_buf[count] = TM1637_SEG_E;
					}
					tm1637_update();
					break;
				case 0x05:
					for(count = 0; count < TM1637_NUM_DIGITS; ++count)
					{
						tm1637_buf[count] = TM1637_SEG_F;
					}
					tm1637_update();
					break;
				case 0x06:
					for(count = 0; count < TM1637_NUM_DIGITS; ++count)
					{
						tm1637_buf[count] = TM1637_SEG_G;
					}
					tm1637_update();
					break;
				case 0x07:
					for(count = 0; count < TM1637_NUM_DIGITS; ++count)
					{
						tm1637_buf[count] = TM1637_SEG_DP;
					}
					tm1637_update();
					break;
				
				case 0x08:
					tm1637_set_dp(0, 1);
					tm1637_update();
					break;
				case 0x09:
					tm1637_set_dp(1, 1);
					tm1637_update();
					break;
				case 0x0A:
					tm1637_set_dp(2, 1);
					tm1637_update();
					break;
				case 0x0B:
					tm1637_set_dp(3, 1);
					tm1637_update();
					break;
				case 0x0C:
					tm1637_set_dp(4, 1);
					tm1637_update();
					break;
				case 0x0D:
					tm1637_set_dp(5, 1);
					tm1637_update();
					break;
				case 0x0E:
					tm1637_init(TM1637_OPT_PW_14_16);
					tm1637_update();
					break;
				
				
				case 0x10:
					for(count = 0; count < TM1637_NUM_DIGITS; ++count)
					{
						tm1637_set_digit(count, count);
					}
					break;
				case 0x11:
					for(count = 0; count < TM1637_NUM_DIGITS; ++count)
					{
						tm1637_set_digit(count, 0x00);
					}
					break;
				case 0x12:
					for(count = 0; count < TM1637_NUM_DIGITS; ++count)
					{
						tm1637_digit_off(count);
					}
					break;
				case 0x13:
					tm1637_update();
					break;
				
				case 0x14:
					tm1637_set_byte(0, 0xEF);
					tm1637_set_byte(1, 0xBE);
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
