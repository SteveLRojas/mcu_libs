#include "CH547.H"
#include "CH547_RCC.h"
#include "CH547_GPIO.h"
#include "CH547_TIMER.h"
#include "CH547_USB_CDC.h"
#include "CH547_FLASH.h"
#include "pseudo_random.h"

char code test_string[] = "Unicorn\n";
char code str_bad_command[] = "Bad command!\n";

//Pins:
// LED0 = P20
// LED1 = P21
// LED2 = P22
// LED3 = P23
// UDM  = P50
// UDP  = P51

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
	
	UINT8 test_buf[64];
	UINT8 code* prg_read_ptr;
	
	rcc_set_clk_freq(RCC_CLK_FREQ_24M);
	
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_2, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);	//LED0, LED1, LED2, LED3
	
	timer_init(TIMER_0, NULL);
	timer_set_period(TIMER_0, FREQ_SYS / 1000ul);	//period is 1ms
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	//Blink LED once
	gpio_clear_pin(GPIO_PORT_2, GPIO_PIN_0);
	timer_long_delay(TIMER_0, 250);
	gpio_set_pin(GPIO_PORT_2, GPIO_PIN_0);
	timer_long_delay(TIMER_0, 250);
	
	cdc_init();
	cdc_set_serial_state(CDC_SS_TXCARRIER | CDC_SS_RXCARRIER);
	prev_control_line_state = cdc_control_line_state;
	while(!cdc_config);
	timer_long_delay(TIMER_0, 250);
	cdc_write_string(test_string);
	
	byte_to_hex(RESET_KEEP, last_keep_str);
	last_keep_str[2] = '\n';
	last_keep_str[3] = '\0';
	cdc_write_string(last_keep_str);
	
	pseudo_random_seed(0xDEADBEEF);
	timer_start(TIMER_0);

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
					pseudo_random_seed(0xDEADBEEF);
					break;
				case 0x01:
					for(count = 0; count < 64; ++count)
					{
						pseudo_random_generate(8);
						test_buf[count] = pseudo_random_get_byte();
					}
					break;
				case 0x02:
					cdc_print_bytes(test_buf, 64);
					break;
				case 0x03:
					flash_write_data(0xF000, test_buf, 64);
					break;
				case 0x04:
					prg_read_ptr = 0xF000;
					for(count = 0; count < 64; ++count)
					{
						test_buf[count] = *prg_read_ptr;
						++prg_read_ptr;
					}
					break;
				case 0x05:
					flash_write_code(0xEF00, test_buf, 64);
					break;
				case 0x06:
					prg_read_ptr = 0xEF00;
					for(count = 0; count < 64; ++count)
					{
						test_buf[count] = *prg_read_ptr;
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
					flash_write_otp(0x3C, 0xDE);
					flash_write_otp(0x3D, 0xAD);
					flash_write_otp(0x3E, 0xBE);
					flash_write_otp(0x3F, 0xEF);
					break;
				case 0x0A:
					for(count = 0; count < 64; count += 4)
					{
						flash_read_otp(count, test_buf + count);
					}
					break;
				default:
					cdc_write_string(str_bad_command);
					break;
			}
			gpio_write_pin(GPIO_PORT_2, GPIO_PIN_1, gpio_read_pin(GPIO_PORT_2, GPIO_PIN_0));
		}
		
		if(timer_overflow_counts[TIMER_0] >= 200)
		{
			timer_overflow_counts[TIMER_0] = 0;
			gpio_toggle_pin(GPIO_PORT_2, GPIO_PIN_2);
		}
		
		if(prev_control_line_state != cdc_control_line_state)
		{
			cdc_set_serial_state(cdc_control_line_state & 3);
			prev_control_line_state = cdc_control_line_state;
		}
	}
}
