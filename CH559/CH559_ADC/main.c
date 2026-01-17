#include "CH559.H"
#include "CH559_RCC.h"
#include "CH559_GPIO.h"
#include "CH559_TIMER.h"
#include "CH559_USB_CDC.h"
#include "CH559_ADC.h"

#define USE_EXT_CLK 0

char code test_string[] = "Unicorn\n";
char code str_bad_command[] = "Bad command!\n";

volatile UINT8 adc_done_flag = 0;
UINT8 monitor_adc = 0;

//Pins:
// AIN0 = P10
// AIN1 = P11
// AIN2 = P12
// AIN3 = P13
// LED1 = P14
// LED2 = P15
// AIN6 = P16
// AIN7 = P17
// UDM = P50
// UDP = P51

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

void on_adc_done(void)
{
	gpio_toggle_pin(GPIO_PORT_1, GPIO_PIN_5);
	adc_done_flag = 1;
}

int main()
{
	char last_keep_str[10];
	UINT8 prev_control_line_state;
	UINT8 reset_type;
	UINT8 datagram[2];
	UINT8 temp;
	UINT8 count;
	UINT16 adc_val;
	
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
	
	// Configure LED and ADC pins
	gpio_set_port_mode(GPIO_PORT_MODE_PP, GPIO_PORT_1);
	gpio_set_port_strength(GPIO_PORT_STRENGTH_20, GPIO_PORT_1);
	gpio_set_pin_mode(GPIO_MODE_INPUT, GPIO_PORT_1, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_6 | GPIO_PIN_7);
	gpio_set_pin_mode(GPIO_MODE_OUTPUT_PP, GPIO_PORT_1, GPIO_PIN_4 | GPIO_PIN_5);
	
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
	adc_init(ADC_SLOW | 4, ADC_AIN0 | ADC_AIN1 | ADC_AIN2 | ADC_AIN3 | ADC_AIN6 | ADC_AIN7);
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
			
			gpio_toggle_pin(GPIO_PORT_1, GPIO_PIN_4);
			switch(RESET_KEEP)
			{
				case 0x00:
					adc_init(ADC_SLOW | 24, ADC_AIN0 | ADC_AIN1 | ADC_AIN2 | ADC_AIN3 | ADC_AIN6 | ADC_AIN7);
					break;
				case 0x01:
					adc_init(ADC_SLOW | 4, ADC_AIN0 | ADC_AIN1 | ADC_AIN2 | ADC_AIN3 | ADC_AIN6 | ADC_AIN7);
					break;
				case 0x02:
					adc_schedule[0] = ADC_AIN0;
					adc_schedule[1] = ADC_AIN1;
					adc_schedule_len = 2;
					adc_init_schedule();
					break;
				case 0x03:
					adc_schedule[0] = ADC_AIN0;
					adc_schedule[1] = ADC_AIN1;
					adc_schedule[2] = ADC_AIN2;
					adc_schedule[3] = ADC_AIN3;
					adc_schedule[4] = ADC_AIN6;
					adc_schedule[5] = ADC_AIN7;
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
					adc_val = adc_read_single(ADC_AIN0);
					word_to_hex(adc_val, last_keep_str);
					cdc_write_string(last_keep_str);
					break;
				case 0x07:
					adc_val = adc_read_single(ADC_AIN1);
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
					adc_digital_in_disable(ADC_AIN0 | ADC_AIN1 | ADC_AIN2 | ADC_AIN3 | ADC_AIN6 | ADC_AIN7);
					break;
				case 0x0F:
					adc_digital_in_enable(ADC_AIN0 | ADC_AIN1 | ADC_AIN2 | ADC_AIN3 | ADC_AIN6 | ADC_AIN7);
					break;
				
				default:
					cdc_write_string(str_bad_command);
					break;
			}
		}
		
		if(timer_overflow_counts[TIMER_0] >= 200)
		{
			timer_overflow_counts[TIMER_0] = 0;
			if(monitor_adc)
			{
				adc_run_schedule(adc_schedule_len);
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
		
		if(prev_control_line_state != cdc_control_line_state)
		{
			cdc_set_serial_state(cdc_control_line_state & 3);
			prev_control_line_state = cdc_control_line_state;
		}
	}
}
