#include "CH559.H"
#include "CH559_RCC.h"
#include "CH559_GPIO.h"
#include "CH559_TIMER.h"
#include "CH559_USB_CDC.h"
#include "CH559_FLASH.h"
#include "pseudo_random.h"

#define USE_EXT_CLK 0

char code test_string[] = "Unicorn\n";
char code str_bad_command[] = "Bad command!\n";

//Pins:
// RXD0 = P02
// TXD0 = P03
// DBGLED1 = P14
// DBGLED2 = P15
// USB_DM = P50
// USB_DP = P51


char code hex_table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
void byte_to_hex(UINT8 value, char* buff)
{
	buff[0] = hex_table[(value >> 4) & 0x0f];
	buff[1] = hex_table[(value) & 0x0f];
	buff[2] = '\0';
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
	char last_keep_str[4];
	UINT8 prev_control_line_state;
	UINT8 datagram[2];
	UINT8 temp;
	UINT8 count;
	
	UINT16 test_buf_16[64];
	UINT8* test_buf_8 = (UINT8*)test_buf_16;
	UINT8 code* prg_read_ptr;
	
	rcc_set_clk_freq(RCC_CLK_FREQ_48M);
#if USE_EXT_CLK	
	rcc_set_clk_src(RCC_CLK_OSC_EN);
	rcc_delay_ms(30);
	rcc_set_clk_src(RCC_CLK_SRC_EXT);
#endif
	
	// Configure UART pins
	gpio_set_port_mode(GPIO_PORT_MODE_OC, GPIO_PORT_0);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_0);
	gpio_set_pin_mode(GPIO_MODE_OC_PU, GPIO_PORT_0, GPIO_PIN_2);
	gpio_set_pin_mode(GPIO_MODE_OC_PU_PULSE, GPIO_PORT_0, GPIO_PIN_3);
	
	// Configure debug LED pins
	gpio_set_port_mode(GPIO_PORT_MODE_PP, GPIO_PORT_1);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_1);
	gpio_set_pin_mode(GPIO_MODE_OUTPUT_PP, GPIO_PORT_1, GPIO_PIN_4 | GPIO_PIN_5);
	
	pseudo_random_seed(0xDEADBEEF);
	timer_init(TIMER_0, NULL);
	timer_set_period(TIMER_0, FREQ_SYS / 1000ul);	//period is 1ms
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	//Blink LED once
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_4);
	timer_long_delay(TIMER_0, 250);
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_4);
	timer_long_delay(TIMER_0, 250);
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_4);
	
	cdc_init();
	cdc_set_serial_state(0x03);
	prev_control_line_state = cdc_control_line_state;
	while(!cdc_config);
	timer_long_delay(TIMER_0, 250);
	cdc_write_string(test_string);
	
	byte_to_hex(RESET_KEEP, last_keep_str);
	last_keep_str[2] = '\n';
	last_keep_str[3] = '\0';
	cdc_write_string(last_keep_str);
	
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
					pseudo_random_seed(0xDEADBEEF);
					break;
				case 0x01:
					for(count = 0; count < 64; ++count)
					{
						pseudo_random_generate(16);
						test_buf_16[count] = pseudo_random_get_word();
					}
					break;
				case 0x02:
					cdc_print_bytes(test_buf_8, 128);
					break;
				case 0x03:
					flash_write_data(0xF000, test_buf_16, 64);
					break;
				case 0x04:
					prg_read_ptr = 0xF000;
					for(count = 0; count < 128; ++count)
					{
						test_buf_8[count] = *prg_read_ptr;
						++prg_read_ptr;
					}
					break;
				case 0x05:
					flash_write_code(0xEF00, test_buf_16, 64);
					break;
				case 0x06:
					prg_read_ptr = 0xEF00;
					for(count = 0; count < 128; ++count)
					{
						test_buf_8[count] = *prg_read_ptr;
						++prg_read_ptr;
					}
					break;
				case 0x07:
					flash_erase_data_page(0xF000);
					break;
				case 0x08:
					flash_erase_code_page(0xEF00);
					break;
				case 0x09:
					cdc_write_byte(ROM_ADDR_H);
					cdc_write_byte(ROM_ADDR_L);
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
