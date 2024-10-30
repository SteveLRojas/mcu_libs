#include "CH552.H"
#include "System.h"
#include "CH552_GPIO.h"
#include "CH552_TIMER.h"
#include "CH552_USB_CDC.h"
#include "CH552_FLASH.h"
#include "pseudo_random.h"
	
char code test_string[] = "Unicorn\n";
char code str_bad_command[] = "Bad command!\n";

//Pins:
// LED2 = P16
// LED3 = P17
// UDP = P36
// UDM = P37

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

void cdc_write_string(char* src)
{
	UINT16 len = 0;
	char* src_copy = src;
	while(*src_copy)
	{
		++len;
		++src_copy;
	}
	cdc_write_bytes(src, len);
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
	
	CfgFsys();
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_6 | GPIO_PIN_7);
	timer_init(TIMER_0, NULL);
	timer_set_period(TIMER_0, FREQ_SYS / 1000ul);	//period is 1ms
	pseudo_random_seed(0xDEADBEEF);
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	//Blink LED once
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_6);
	timer_long_delay(TIMER_0, 250);
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_6);
	timer_long_delay(TIMER_0, 250);
	
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
			
			gpio_write_pin(GPIO_PORT_1, GPIO_PIN_7, !gpio_read_pin(GPIO_PORT_1, GPIO_PIN_7));
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
					flash_write(0, test_buf_8, 128);
					break;
				case 0x04:
					flash_read(0, test_buf_8, 128);
					break;
				case 0x05:
					flash_write_code(0x3780, test_buf_16, 64);
					break;
				case 0x06:
					prg_read_ptr = 0x3780;
					for(count = 0; count < 128; ++count)
					{
						test_buf_8[count] = *prg_read_ptr;
						++prg_read_ptr;
					}
					break;
				case 0x07:
					cdc_write_byte(ROM_ADDR_H);
					cdc_write_byte(ROM_ADDR_L);
					break;
				default:
					cdc_write_string(str_bad_command);
					break;
			}
			gpio_write_pin(GPIO_PORT_1, GPIO_PIN_6, gpio_read_pin(GPIO_PORT_1, GPIO_PIN_7));
		}
		
		if(prev_control_line_state != cdc_control_line_state)
		{
			cdc_set_serial_state(cdc_control_line_state & 3);
			prev_control_line_state = cdc_control_line_state;
		}
	}
}

