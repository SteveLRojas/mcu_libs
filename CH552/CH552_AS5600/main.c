#include "CH552.H"
#include "CH552_RCC.h"
#include "CH552_GPIO.h"
#include "CH552_TIMER.h"
#include "CH552_USB_CDC.h"
#include "CH552_I2C.h"
#include "CH552_AS5600.h"

char code test_string[] = "Unicorn\n";
char code str_bad_command[] = "Bad command!\n";

//Pins:
// I2C_SCL = P14
// DIR = P15
// LED2 = P16
// LED3 = P17
// RXD = P30
// TXD = P31
// I2C_SDA = P32
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
	char last_keep_str[6];
	UINT8 prev_control_line_state;
	UINT8 datagram[2];
	UINT8 temp;
	UINT8 count;
	UINT16 as_read;
	
	rcc_set_clk_freq(RCC_CLK_FREQ_24M);
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
	gpio_set_mode(GPIO_MODE_OD, GPIO_PORT_1, GPIO_PIN_4);
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_3, GPIO_PIN_1);
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_3, GPIO_PIN_0);
	gpio_set_mode(GPIO_MODE_OD, GPIO_PORT_3, GPIO_PIN_2);
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_5);
	
	i2c_init(GPIO_PORT_3, GPIO_PIN_2, GPIO_PORT_1, GPIO_PIN_4);
	
	timer_init(TIMER_0, NULL);
	timer_set_period(TIMER_0, FREQ_SYS / 1000ul);	//period is 1ms
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
					as_read = as5600_read_word(AS_REG_CONF);
					byte_to_hex((UINT8)(as_read >> 8), last_keep_str);
					byte_to_hex((UINT8)as_read, last_keep_str + 2);
					last_keep_str[4] = '\n';
					last_keep_str[5] = '\0';
					cdc_write_string(last_keep_str);
					break;
				case 0x01:
					as_read = as5600_read_word(AS_REG_RAW_ANGLE);
					byte_to_hex((UINT8)(as_read >> 8), last_keep_str);
					byte_to_hex((UINT8)as_read, last_keep_str + 2);
					last_keep_str[4] = '\n';
					last_keep_str[5] = '\0';
					cdc_write_string(last_keep_str);
					break;
				case 0x02:
					as_read = as5600_read_word(AS_REG_ANGLE);
					byte_to_hex((UINT8)(as_read >> 8), last_keep_str);
					byte_to_hex((UINT8)as_read, last_keep_str + 2);
					last_keep_str[4] = '\n';
					last_keep_str[5] = '\0';
					cdc_write_string(last_keep_str);
					break;
				case 0x03:
					temp = as5600_read_byte(AS_REG_STATUS);
					byte_to_hex(temp, last_keep_str);
					last_keep_str[2] = '\n';
					last_keep_str[3] = '\0';
					cdc_write_string(last_keep_str);
					break;
				case 0x04:
					temp = as5600_read_byte(AS_REG_AGC);
					byte_to_hex(temp, last_keep_str);
					last_keep_str[2] = '\n';
					last_keep_str[3] = '\0';
					cdc_write_string(last_keep_str);
					break;
				case 0x05:
					as_read = as5600_read_word(AS_REG_MAGNITUDE);
					byte_to_hex((UINT8)(as_read >> 8), last_keep_str);
					byte_to_hex((UINT8)as_read, last_keep_str + 2);
					last_keep_str[4] = '\n';
					last_keep_str[5] = '\0';
					cdc_write_string(last_keep_str);
					break;
				case 0x06:
					timer_start(TIMER_0);
					break;
				case 0x07:
					timer_stop(TIMER_0);
					break;
				default:
					cdc_write_string(str_bad_command);
					break;
			}
			gpio_write_pin(GPIO_PORT_1, GPIO_PIN_6, gpio_read_pin(GPIO_PORT_1, GPIO_PIN_7));
		}
		
		if(timer_overflow_counts[TIMER_0] >= 100)
		{
			timer_overflow_counts[TIMER_0] = 0;
			as_read = as5600_read_word(AS_REG_RAW_ANGLE);
			byte_to_hex((UINT8)(as_read >> 8), last_keep_str);
			byte_to_hex((UINT8)as_read, last_keep_str + 2);
			last_keep_str[4] = '\n';
			last_keep_str[5] = '\0';
			cdc_write_string(last_keep_str);
		}
		
		if(prev_control_line_state != cdc_control_line_state)
		{
			cdc_set_serial_state(cdc_control_line_state & 3);
			prev_control_line_state = cdc_control_line_state;
		}
	}
}

