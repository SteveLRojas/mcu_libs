#include "CH552.H"
#include "CH552_RCC.h"
#include "CH552_GPIO.h"
#include "CH552_PWM.h"
#include "CH552_USB_CDC.h"
#include "CH552_ST7066.h"

char code str_unicorn[] = "Unicorn\n";
char code str_dragon[] = "Dragon\n";
char code str_wolf[] = "Wolf\n";
char code str_horse[] = "Horse\n";
char code str_lion[] = "Lion\n";
char code str_bear[] = "Bear\n";
char code str_bad_command[] = "Bad command!\n";

//Pins:
// LCD_E = P14
// LCD_DATA = P15
// LCD_CLK = P17
// R_PWM = P30
// LCD_CONTRAST = P31
// LCD_RS = P32
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

int main()
{
	UINT8 prev_control_line_state;
	UINT8 datagram[2];
	UINT8 temp;
	UINT8 count;
	signed short contrast = 45;
	signed short brightness = 127;
	
	rcc_set_clk_freq(RCC_CLK_FREQ_24M);
	
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_3, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2);
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_4);
	gpio_clear_pin(GPIO_PORT_3, GPIO_PIN_2);
	
	pwm_init(PWM_1 | PWM_2, PWM_ACTIVE_HIGH, 1, PWM_1_P30 | PWM_2_P31);
	pwm_set_duty_cycle(PWM_1, (UINT8)brightness);
	pwm_set_duty_cycle(PWM_2, (UINT8)contrast);
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	cdc_init();
	cdc_set_serial_state(0x03);
	prev_control_line_state = cdc_control_line_state;
	while(!cdc_config);
	rcc_delay_ms(250);
	cdc_write_string(str_unicorn);
	
	st7066_init();
	
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
			
			switch(RESET_KEEP)
			{
				case 0x00:
					st7066_init();
					break;
				case 0x01:
					st7066_set_cursor(0, 0);
					break;
				case 0x02:
					st7066_set_cursor(1, 0);
					break;
				case 0x03:
					st7066_set_cursor(2, 0);
					break;
				case 0x04:
					st7066_set_cursor(3, 0);
					break;
				case 0x05:
					st7066_write_string(str_unicorn);
					break;
				case 0x06:
					st7066_write_string(str_dragon);
					break;
				case 0x07:
					st7066_write_string(str_wolf);
					break;
				case 0x08:
					st7066_write_string(str_horse);
					break;
				case 0x09:
					st7066_write_string(str_lion);
					break;
				case 0x0A:
					st7066_write_string(str_bear);
					break;
				case 0x0B:
					st7066_clear_display();
					break;
				case 0x0C:
					st7066_clear_line(0);
					break;
				case 0x0D:
					st7066_clear_line(1);
					break;
				case 0x0E:
					st7066_clear_line(2);
					break;
				case 0x0F:
					st7066_clear_line(3);
					break;
				case 0x10:
					brightness += 10;
					if(brightness > 255)
						brightness = 255;
					pwm_set_duty_cycle(PWM_1, (UINT8)brightness);
					break;
				case 0x11:
					brightness -= 10;
					if(brightness < 0)
						brightness = 0;
					pwm_set_duty_cycle(PWM_1, (UINT8)brightness);
					break;
				case 0x12:
					contrast += 10;
					if(contrast > 255)
						contrast = 255;
					pwm_set_duty_cycle(PWM_2, (UINT8)contrast);
					break;
				case 0x13:
					contrast -= 10;
					if(contrast < 0)
						contrast = 0;
					pwm_set_duty_cycle(PWM_2, (UINT8)contrast);
					break;
				case 0x14:
					st7066_send_command(ST7066_COM_DISP_ON_OFF);
					break;
				case 0x15:
					st7066_send_command(ST7066_COM_DISP_ON_OFF | ST7066_DISP_ON_OFF_DISP_ON);
					break;
				case 0x16:
					st7066_send_command(ST7066_COM_DISP_ON_OFF | ST7066_DISP_ON_OFF_DISP_ON | ST7066_DISP_ON_OFF_CURS_ON);
					break;
				case 0x17:
					st7066_send_command(ST7066_COM_DISP_ON_OFF | ST7066_DISP_ON_OFF_DISP_ON | ST7066_DISP_ON_OFF_CURS_ON | ST7066_DISP_ON_OFF_BLINK_ON);
					break;
				case 0x18:
					st7066_send_command(ST7066_COM_FUNCTION_SET | ST7066_FUNCTION_SET_8_BIT | ST7066_FUNCTION_SET_2_LINE);
					break;
				case 0x19:
					st7066_send_command(ST7066_COM_ENTRY_MODE | ST7066_ENTRY_MODE_INC | ST7066_ENTRY_MODE_SHIFT_DIS);
					break;
				default:
					cdc_write_string(str_bad_command);
					break;
			}
		}
		
		if(prev_control_line_state != cdc_control_line_state)
		{
			cdc_set_serial_state(cdc_control_line_state & 3);
			prev_control_line_state = cdc_control_line_state;
		}
	}
}

