#include "CH547.H"
#include "CH547_RCC.h"
#include "CH547_GPIO.h"
#include "CH547_TIMER.h"
#include "CH547_USB_CDC.h"
#include "CH547_ADC.h"

char code test_string[] = "Unicorn\n";
char code str_bad_command[] = "Bad command!\n";
char code str_comp_changed[] = "Comparator output changed!\n";

volatile UINT8 adc_done_flag = 0;
volatile UINT8 cmp_change_flag = 0;
UINT8 monitor_adc = 0;
UINT8 monitor_cmp = 0;

//Pins:
// AIN0 = P10
// AIN1 = P11
// AIN2 = P12
// AIN3 = P13
// LED0 = P20
// LED1 = P21
// LED2 = P22
// LED3 = P23
// UDM  = P50
// UDP  = P51

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

void print_temp(UINT16 temp_raw)
{
	UINT16 temp_cc;
	char temp_str[10];
	
	temp_cc = adc_convert_temp_cc(temp_raw);
	temp_str[0] = 'T';
	temp_str[1] = ':';
	temp_str[6] = hex_table[temp_cc % 10];
	temp_cc = temp_cc / 10;
	temp_str[5] = hex_table[temp_cc % 10];
	temp_str[4] = '.';
	temp_cc = temp_cc / 10;
	temp_str[3] = hex_table[temp_cc % 10];
	temp_cc = temp_cc / 10;
	temp_str[2] = hex_table[temp_cc % 10];
	temp_str[7] = '\n';
	temp_str[8] = '\0';
	cdc_write_string(temp_str);
}

void on_adc_done(void)
{
	gpio_toggle_pin(GPIO_PORT_2, GPIO_PIN_2);
	adc_done_flag = 1;
}

void on_cmp_change(void)
{
	gpio_toggle_pin(GPIO_PORT_2, GPIO_PIN_3);
	cmp_change_flag = 1;
}

int main()
{
	char last_keep_str[10];
	UINT8 prev_control_line_state;
	UINT8 datagram[2];
	UINT8 temp;
	UINT8 count;
	UINT16 adc_val;
	UINT16 adc_max;
	UINT16 adc_min;
	
	rcc_set_clk_freq(RCC_CLK_FREQ_24M);
	
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_1, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);	//AIN0, AIN1, AIN2, AIN3
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
	
	adc_done_callback = on_adc_done;
	cmp_change_callback = on_cmp_change;
	adc_init(ADC_CLK_750K);
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
					adc_init(ADC_CLK_750K);
					break;
				case 0x01:
					adc_init(ADC_CLK_6M);
					break;
				case 0x02:
					adc_schedule[0] = ADC_CHAN_AIN0_P10;
					adc_schedule[1] = ADC_CHAN_AIN1_P11;
					adc_schedule_len = 2;
					adc_init_schedule();
					break;
				case 0x03:
					adc_schedule[0] = ADC_CHAN_AIN0_P10;
					adc_schedule[1] = ADC_CHAN_AIN1_P11;
					adc_schedule[2] = ADC_CHAN_TEMP;
					adc_schedule[3] = ADC_CHAN_NOISE;
					adc_schedule[4] = ADC_CHAN_VDD50;
					adc_schedule[5] = ADC_CHAN_V33;
					adc_schedule_len = 6;
					adc_init_schedule();
					break;
				
				case 0x04:
					adc_run_schedule(adc_schedule_len);
					break;
				case 0x05:
					last_keep_str[1] = ':';
					for(count = 0; count < adc_schedule_len; ++count)
					{
						last_keep_str[0] = hex_table[count];
						word_to_hex(adc_results[count], last_keep_str + 2);
						last_keep_str[6] = '\n';
						last_keep_str[7] = '\0';
						cdc_write_string(last_keep_str);
					}
					cdc_write_byte('\n');
					break;
				case 0x06:
					adc_val = adc_read_single(ADC_CHAN_AIN0_P10);
					word_to_hex(adc_val, last_keep_str);
					cdc_write_string(last_keep_str);
					break;
				case 0x07:
					adc_val = adc_read_single(ADC_CHAN_TEMP);
					word_to_hex(adc_val, last_keep_str);
					last_keep_str[4] = '\n';
					last_keep_str[5] = '\0';
					cdc_write_string(last_keep_str);
					break;
				
				case 0x08:
					adc_interrupt_enable();
					break;
				case 0x09:
					adc_interrupt_disable();
					break;
				case 0x0A:
					adc_enable();
					break;
				case 0x0B:
					adc_disable();
					break;
				
				case 0x0C:
					monitor_adc = 1;
					break;
				case 0x0D:
					monitor_adc = 0;
					break;
				case 0x0E:
					monitor_cmp = 1;
					break;
				case 0x0F:
					monitor_cmp = 0;
					break;
				
				case 0x10:
					cmp_init(ADC_CHAN_AIN0_P10, CMP_INN_AIN1_P11);
					break;
				case 0x11:
					temp = cmp_get_result();
					byte_to_hex(temp, last_keep_str);
					cdc_write_string(last_keep_str);
					break;
				case 0x12:
					cmp_enable();
					break;
				case 0x13:
					cmp_disable();
					break;
				
				case 0x14:
					adc_digital_in_disable(ADC_DI_AIN0_AIN1 | ADC_DI_AIN2_AIN3);
					break;
				case 0x15:
					adc_digital_in_enable(ADC_DI_AIN0_AIN1 | ADC_DI_AIN2_AIN3);
					break;
				case 0x16:
					adc_val = 0;
					for(count = 0; count < 16; ++count)
					{
						adc_val += adc_read_single(ADC_CHAN_TEMP);
					}
					word_to_hex(adc_val, last_keep_str);
					last_keep_str[4] = '\n';
					last_keep_str[5] = '\0';
					cdc_write_string(last_keep_str);
					break;
				case 0x17:
					adc_val = 0;
					adc_max = 0;
					adc_min = 0xFFFF;
					for(count = 0; count < 64; ++count)
					{
						adc_val = adc_read_single(ADC_CHAN_NOISE);
						if(adc_val > adc_max)
							adc_max = adc_val;
						if(adc_val < adc_min)
							adc_min = adc_val;
					}
					last_keep_str[0] = 'X';
					last_keep_str[1] = ':';
					word_to_hex(adc_max, last_keep_str + 2);
					last_keep_str[6] = '\n';
					last_keep_str[7] = '\0';
					cdc_write_string(last_keep_str);
					
					last_keep_str[0] = 'N';
					word_to_hex(adc_min, last_keep_str + 2);
					last_keep_str[6] = '\n';
					last_keep_str[7] = '\0';
					cdc_write_string(last_keep_str);
					break;
					
				case 0x18:
					adc_val = 0;
					for(count = 0; count < 16; ++count)
					{
						adc_val += adc_read_single(ADC_CHAN_TEMP);
					}
					adc_val = adc_val >> 4;
					print_temp(adc_val);
					break;
				case 0x19:
					adc_val = adc_read_single(ADC_CHAN_TEMP);
					print_temp(adc_val);
					break;
				case 0x1A:
					print_temp(2087);
					break;
				case 0x1B:
					print_temp(2244);
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
			if(monitor_adc)
			{
				adc_run_schedule(adc_schedule_len);
			}
			if(monitor_cmp)
			{
				last_keep_str[0] = 'R';
				last_keep_str[1] = ':';
				temp = cmp_get_result();
				byte_to_hex(temp, last_keep_str + 2);
				last_keep_str[4] = '\n';
				last_keep_str[5] = '\0';
				cdc_write_string(last_keep_str);
				
				last_keep_str[0] = 'C';
				temp = cmp_get_change();
				byte_to_hex(temp, last_keep_str + 2);
				last_keep_str[4] = '\n';
				last_keep_str[5] = '\0';
				cdc_write_string(last_keep_str);
			}
		}
		
		if(adc_done_flag)
		{
			adc_done_flag = 0;
			last_keep_str[1] = ':';
			for(count = 0; count < adc_schedule_len; ++count)
			{
				last_keep_str[0] = hex_table[count];
				word_to_hex(adc_results[count], last_keep_str + 2);
				last_keep_str[6] = '\n';
				last_keep_str[7] = '\0';
				cdc_write_string(last_keep_str);
			}
			cdc_write_byte('\n');
		}
		
		if(cmp_change_flag)
		{
			cmp_change_flag = 0;
			cdc_write_string(str_comp_changed);
		}
		
		if(prev_control_line_state != cdc_control_line_state)
		{
			cdc_set_serial_state(cdc_control_line_state & 3);
			prev_control_line_state = cdc_control_line_state;
		}
	}
}
