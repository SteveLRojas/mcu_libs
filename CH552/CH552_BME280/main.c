#include "CH552.H"
#include "CH552_RCC.h"
#include "CH552_GPIO.h"
#include "CH552_TIMER.h"
#include "CH552_USB_CDC.h"
#include "CH552_I2C.h"
#include "CH552_BME280.h"

char code test_string[] = "Unicorn\n";
char code str_bad_command[] = "Bad command!\n";
char code str_raw_pressure[] = "Raw pressure: ";
char code str_raw_temperature[] = "Raw temperature: ";
char code str_raw_humidity[] = "Raw humidity: ";
char code str_converted_pressure[] = "Converted pressure: ";
char code str_converted_temperature[] = "Converted temperature: ";
char code str_converted_humidity[] = "Converted humidity: ";

//Pins:
// I2C_SDA = P11
// LED0 = P16
// LED1 = P17
// I2C_SCL = P33
// UDP = P36
// UDM = P37

char code hex_table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};

void byte_to_hex(UINT8 value, char* buff)
{
	buff[0] = hex_table[(value >> 4) & 0x0f];
	buff[1] = hex_table[(value) & 0x0f];
	buff[2] = '\n';
	buff[3] = '\0';
}

void word_to_hex(UINT16 value, char* buff)
{
	buff[0] = hex_table[(value >> 12) & 0x0F];
	buff[1] = hex_table[(value >> 8) & 0x0F];
	buff[2] = hex_table[(value >> 4) & 0x0F];
	buff[3] = hex_table[(value) & 0x0F];
	buff[4] = '\n';
	buff[5] = '\0';
}

void signed_long_to_decimal(signed long value, char* buff)
{
	UINT8 count;
	
	buff[0] = '+';
	if(value < 0)
	{
		buff[0] = '-';
		value = -value;
	}
	
	count = 10;
	do
	{
		buff[count] = hex_table[(UINT8)(value % 10)];
		value = value / 10;
		count -= 1;
	} while(count);
	buff[11] = '\n';
	buff[12] = '\0';
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
	char last_keep_str[13];
	UINT8 prev_control_line_state;
	UINT8 datagram[2];
	UINT8 temp;
	UINT8 count;
	UINT8 test_buf[26];
	signed long meas_val;
	
	rcc_set_clk_freq(RCC_CLK_FREQ_24M);
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_6 | GPIO_PIN_7);	//LED0, LED1
	gpio_set_mode(GPIO_MODE_OD, GPIO_PORT_1, GPIO_PIN_1);	//I2C_SDA
	gpio_set_mode(GPIO_MODE_OD, GPIO_PORT_3, GPIO_PIN_3);	//I2C_SCL
	
	i2c_init(GPIO_PORT_1, GPIO_PIN_1, GPIO_PORT_3, GPIO_PIN_3);
	
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
	cdc_set_serial_state(CDC_SS_TXCARRIER | CDC_SS_RXCARRIER);
	prev_control_line_state = cdc_control_line_state;
	while(!cdc_config);
	timer_long_delay(TIMER_0, 250);
	cdc_write_string(test_string);
	
	byte_to_hex(RESET_KEEP, last_keep_str);
	cdc_write_string(last_keep_str);
	
	bme280_init();
	
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
					temp = bme280_read_reg(BME280_REG_CHIP_ID);
					byte_to_hex(temp, last_keep_str);
					cdc_write_string(last_keep_str);
					break;
				case 0x01:
					bme280_read_regs(BME280_REG_DIG_T1_LSB, test_buf, 26);
					cdc_print_bytes(test_buf, 26);
					cdc_write_byte('\n');
					bme280_read_regs(BME280_REG_DIG_H2_LSB, test_buf, 16);
					cdc_print_bytes(test_buf, 16);
					cdc_write_byte('\n');
					break;
				case 0x02:
					cdc_print_bytes((UINT8*)&bme280_calib_data, 33);
					cdc_write_byte('\n');
					break;
				case 0x03:
					word_to_hex(bme280_calib_data.dig_T1, last_keep_str);
					cdc_write_string(last_keep_str);
					word_to_hex(bme280_calib_data.dig_T2, last_keep_str);
					cdc_write_string(last_keep_str);
					word_to_hex(bme280_calib_data.dig_T3, last_keep_str);
					cdc_write_string(last_keep_str);
				
					word_to_hex(bme280_calib_data.dig_P1, last_keep_str);
					cdc_write_string(last_keep_str);
					word_to_hex(bme280_calib_data.dig_P2, last_keep_str);
					cdc_write_string(last_keep_str);
					word_to_hex(bme280_calib_data.dig_P3, last_keep_str);
					cdc_write_string(last_keep_str);
					word_to_hex(bme280_calib_data.dig_P4, last_keep_str);
					cdc_write_string(last_keep_str);
					word_to_hex(bme280_calib_data.dig_P5, last_keep_str);
					cdc_write_string(last_keep_str);
					word_to_hex(bme280_calib_data.dig_P6, last_keep_str);
					cdc_write_string(last_keep_str);
					word_to_hex(bme280_calib_data.dig_P7, last_keep_str);
					cdc_write_string(last_keep_str);
					word_to_hex(bme280_calib_data.dig_P8, last_keep_str);
					cdc_write_string(last_keep_str);
					word_to_hex(bme280_calib_data.dig_P9, last_keep_str);
					cdc_write_string(last_keep_str);
					
					byte_to_hex(bme280_calib_data.dig_H1, last_keep_str);
					cdc_write_string(last_keep_str);
					word_to_hex(bme280_calib_data.dig_H2, last_keep_str);
					cdc_write_string(last_keep_str);
					byte_to_hex(bme280_calib_data.dig_H3, last_keep_str);
					cdc_write_string(last_keep_str);
					word_to_hex(bme280_calib_data.dig_H4, last_keep_str);
					cdc_write_string(last_keep_str);
					word_to_hex(bme280_calib_data.dig_H5, last_keep_str);
					cdc_write_string(last_keep_str);
					byte_to_hex(bme280_calib_data.dig_H6, last_keep_str);
					cdc_write_string(last_keep_str);
					break;

				case 0x04:
					bme280_set_meas_mode(BME280_MEAS_MODE_SLEEP);
					break;
				case 0x05:
					bme280_set_meas_mode(BME280_MEAS_MODE_NORMAL);
					break;
				case 0x06:
					bme280_read_raw_values();
					break;
				case 0x07:
					bme280_force_measurement();
					break;

				case 0x08:
					cdc_write_string(str_raw_pressure);
					meas_val = bme280_get_pressure_raw();
					signed_long_to_decimal(meas_val, last_keep_str);
					cdc_write_string(last_keep_str);
				
					cdc_write_string(str_raw_temperature);
					meas_val = bme280_get_temperature_raw();
					signed_long_to_decimal(meas_val, last_keep_str);
					cdc_write_string(last_keep_str);
				
					cdc_write_string(str_raw_humidity);
					meas_val = bme280_get_humidity_raw();
					signed_long_to_decimal(meas_val, last_keep_str);
					cdc_write_string(last_keep_str);
					break;
				case 0x09:
					cdc_write_string(str_converted_temperature);
					meas_val = bme280_convert_temperature();
					signed_long_to_decimal(meas_val, last_keep_str);
					cdc_write_string(last_keep_str);
				
					cdc_write_string(str_converted_pressure);
					meas_val = bme280_convert_pressure();
					signed_long_to_decimal(meas_val, last_keep_str);
					cdc_write_string(last_keep_str);
				
					cdc_write_string(str_converted_humidity);
					meas_val = bme280_convert_humidity();
					signed_long_to_decimal(meas_val, last_keep_str);
					cdc_write_string(last_keep_str);
					break;
				case 0x0A:
					timer_start(TIMER_0);
					break;
				case 0x0B:
					timer_stop(TIMER_0);
					break;
				default:
					cdc_write_string(str_bad_command);
					break;
			}
			gpio_write_pin(GPIO_PORT_1, GPIO_PIN_6, gpio_read_pin(GPIO_PORT_1, GPIO_PIN_7));
		}
		
		if(timer_overflow_counts[TIMER_0] >= 750)
		{
			timer_overflow_counts[TIMER_0] = 0;
			bme280_read_raw_values();
			
			cdc_write_string(str_converted_temperature);
			meas_val = bme280_convert_temperature();
			signed_long_to_decimal(meas_val, last_keep_str);
			cdc_write_string(last_keep_str);
		
			cdc_write_string(str_converted_pressure);
			meas_val = bme280_convert_pressure();
			signed_long_to_decimal(meas_val, last_keep_str);
			cdc_write_string(last_keep_str);
		
			cdc_write_string(str_converted_humidity);
			meas_val = bme280_convert_humidity();
			signed_long_to_decimal(meas_val, last_keep_str);
			cdc_write_string(last_keep_str);
			
			cdc_write_byte('\n');
		}
		
		if(prev_control_line_state != cdc_control_line_state)
		{
			cdc_set_serial_state(cdc_control_line_state & 3);
			prev_control_line_state = cdc_control_line_state;
		}
	}
}

