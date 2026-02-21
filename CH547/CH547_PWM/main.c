#include "CH547.H"
#include "CH547_RCC.h"
#include "CH547_GPIO.h"
#include "CH547_TIMER.h"
#include "CH547_USB_CDC.h"
#include "CH547_PWM.h"

char code test_string[] = "Unicorn\n";
char code str_bad_command[] = "Bad command!\n";

//Pins:
// LED0 = P10
// LED1 = P11
// LED2 = P12
// PWM3 = P22
// PWM2 = P23
// PWM1 = P24
// PWM0 = P25
// UDM  = P50
// UDP  = P51

void byte_to_hex(UINT8 value, char* buff)
{
	const char table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	buff[0] = table[(value >> 4) & 0x0f];
	buff[1] = table[(value) & 0x0f];
	buff[2] = '\0';
}

int main()
{
	char last_keep_str[4];
	UINT8 count;
	UINT8 temp;
	UINT8 prev_control_line_state;
	UINT8 datagram[4];
	UINT16 datagram_val;
	
	rcc_set_clk_freq(RCC_CLK_FREQ_24M);
	
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2);	//LED0, LED1, LED2
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_2, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);	//PWM3, PWM2, PWM1, PWM0
	
	timer_init(TIMER_0, NULL);
	timer_set_period(TIMER_0, FREQ_SYS / 1000ul);	//period is 1ms
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	//Blink LED once
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_0);
	timer_long_delay(TIMER_0, 250);
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_0);
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
	
	pwm_init(PWM_3 | PWM_2 | PWM_1 | PWM_0, PWM_1_MODE_ACTIVE_LOW | PWM_0_MODE_ACTIVE_LOW | PWM_MODE_8_BIT, 1);
	pwm_set_output_enable(PWM_3 | PWM_2 | PWM_1 | PWM_0, 1);
	pwm_set_duty_cycle(PWM_3 | PWM_2 | PWM_1 | PWM_0, 0);
	timer_start(TIMER_0);

	while(TRUE)
	{
		if(cdc_bytes_available() >= 4)
		{
			temp = cdc_peek();
			if((temp == '\r') || (temp == '\n'))
			{
				(void)cdc_read_byte();
				continue;
			}
			
			cdc_read_bytes(datagram, 4);
			for(count = 0; count < 4; ++count)
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

				datagram_val = datagram_val << 4;
				datagram_val = datagram_val | temp;
			}
			
			gpio_toggle_pin(GPIO_PORT_1, GPIO_PIN_0);
			switch((UINT8)(datagram_val >> 8))
			{
				case 0x00:
					pwm_set_duty_cycle(PWM_0, (UINT8)datagram_val);
					break;
				case 0x01:
					pwm_set_duty_cycle(PWM_1, (UINT8)datagram_val);
					break;
				case 0x02:
					pwm_set_duty_cycle(PWM_2, (UINT8)datagram_val);
					break;
				case 0x03:
					pwm_set_duty_cycle(PWM_3, (UINT8)datagram_val);
					break;
				case 0x04:
					pwm_set_clk_div((UINT8)datagram_val);
					break;
				case 0x06:
					pwm_init(PWM_3 | PWM_2 | PWM_1 | PWM_0, PWM_1_MODE_ACTIVE_LOW | PWM_0_MODE_ACTIVE_LOW | PWM_MODE_8_BIT, (UINT8)datagram_val);
					break;
				case 0x07:
					pwm_init(PWM_3 | PWM_2 | PWM_1 | PWM_0, PWM_1_MODE_ACTIVE_LOW | PWM_0_MODE_ACTIVE_LOW | PWM_MODE_6_BIT, (UINT8)datagram_val);
					break;
				default:
					cdc_write_string(str_bad_command);
					break;
			}
			gpio_write_pin(GPIO_PORT_1, GPIO_PIN_1, gpio_read_pin(GPIO_PORT_1, GPIO_PIN_0));
		}
		
		if(timer_overflow_counts[TIMER_0] >= 200)
		{
			timer_overflow_counts[TIMER_0] = 0;
			gpio_toggle_pin(GPIO_PORT_1, GPIO_PIN_2);
		}

		if(prev_control_line_state != cdc_control_line_state)
		{
			cdc_set_serial_state(cdc_control_line_state & 3);
			prev_control_line_state = cdc_control_line_state;
		}
	}
}
