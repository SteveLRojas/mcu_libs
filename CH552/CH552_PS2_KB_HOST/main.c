#include "CH552.H"
#include "CH552_RCC.h"
#include "CH552_GPIO.h"
#include "CH552_UART.h"
#include "CH552_TIMER.h"
#include "CH552_PS2H_KB.h"

#define  BAUD_RATE  125000ul

char code test_string[] = "Unicorn\n";
char code str_bad_command[] = "Bad command!\n";
char code str_rx_error[] = "RX error flag set\n";
char code str_tx_error[] = "TX error flag set\n";
char code str_tx_complete[] = "TX complete\n";
char code str_bat_ok[] = "BAT passed\n";
char code str_data_error[] = "data error\n";
char code str_prot_error[] = "protocol error\n";
char code str_tim_error[] = "timeout error\n";
char code str_rollover[] = "rollover error\n";
char code str_pause_pressed[] = "pause pressed\n";
char code str_key_pressed[] = "Key pressed: ";
char code str_key_released[] = "Key released: ";

extern volatile UINT8 ps2h_kb_rx_state;
extern volatile UINT8 ps2h_kb_timer_state;
extern volatile UINT8 ps2h_kb_scan_state;

//Pins:
// LED2 = P16
// LED3 = P17
// RXD = P30
// TXD = P31
// PS2_CLK = P32
// PS2_DATA = P33
// UDP = P36
// UDM = P37

char code hex_table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};

void byte_to_hex(UINT8 value, char* buff)
{
	buff[0] = hex_table[(value >> 4) & 0x0f];
	buff[1] = hex_table[(value) & 0x0f];
	buff[2] = '\0';
}

int main()
{
	char last_keep_str[4];
	char rx_str[6];
	UINT8 datagram[2];
	UINT8 temp;
	UINT8 count;
	UINT16 pressed_keys_prev[PS2H_KB_MAX_NUM_PRESSED];
	
	rcc_set_clk_freq(RCC_CLK_FREQ_24M);
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_6 | GPIO_PIN_7);
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_3, GPIO_PIN_1);
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_3, GPIO_PIN_0);
	gpio_set_mode(GPIO_MODE_OD, GPIO_PORT_3, GPIO_PIN_2 | GPIO_PIN_3);
	
	uart0_init(TIMER_1, BAUD_RATE, UART_0_P30_P31);
	timer_init(TIMER_0, NULL);
	timer_set_period(TIMER_0, FREQ_SYS / 1000ul);	//period is 1ms
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	//Blink LED once
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_6);
	timer_long_delay(TIMER_0, 250);
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_6);
	timer_long_delay(TIMER_0, 250);
	uart_write_string(UART_0, test_string);
	
	byte_to_hex(RESET_KEEP, last_keep_str);
	last_keep_str[2] = '\n';
	last_keep_str[3] = '\0';
	uart_write_string(UART_0, last_keep_str);
	
	ps2h_kb_init(TIMER_2);
	for(count = 0; count < PS2H_KB_MAX_NUM_PRESSED; ++count)
	{
		pressed_keys_prev[count] = 0;
	}
	
	while(TRUE)
	{
		if(uart_bytes_available(UART_0) >= 2)
		{
			temp = uart_peek(UART_0);
			if((temp == '\r') || (temp == '\n'))
			{
				(void)uart_read_byte(UART_0);
				continue;
			}
			
			uart_read_bytes(UART_0, datagram, 2);
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
			if(RESET_KEEP < 8)
				ps2h_kb_set_led(RESET_KEEP);
			else if(RESET_KEEP == 0x80)
				ps2h_kb_reset();
			else if(RESET_KEEP == 0x81)
			{
				byte_to_hex(ps2h_kb_rx_state, last_keep_str);
				last_keep_str[2] = '\n';
				last_keep_str[3] = '\0';
				uart_write_string(UART_0, last_keep_str);
			}
			else if(RESET_KEEP == 0x82)
			{
				byte_to_hex(ps2h_kb_timer_state, last_keep_str);
				last_keep_str[2] = '\n';
				last_keep_str[3] = '\0';
				uart_write_string(UART_0, last_keep_str);
			}
			else if(RESET_KEEP == 0x83)
			{
				byte_to_hex(ps2h_kb_scan_state, last_keep_str);
				last_keep_str[2] = '\n';
				last_keep_str[3] = '\0';
				uart_write_string(UART_0, last_keep_str);
			}
			else if(RESET_KEEP == 0x84)
			{
				for(count = 0; count < PS2H_KB_MAX_NUM_PRESSED; ++count)
				{
					byte_to_hex((UINT8)(ps2h_kb_pressed_keys[count] >> 8), rx_str);
					byte_to_hex((UINT8)(ps2h_kb_pressed_keys[count]), rx_str + 2);
					rx_str[4] = '\n';
					rx_str[5] = '\0';
					uart_write_string(UART_0, rx_str);
				}
			}
			else
				uart_write_string(UART_0, str_bad_command);
			gpio_write_pin(GPIO_PORT_1, GPIO_PIN_6, gpio_read_pin(GPIO_PORT_1, GPIO_PIN_7));
		}
		
		ps2h_kb_update_keys();
		
		if(ps2h_kb_status & PS2H_KB_STAT_BAT_PASSED)
		{
			uart_write_string(UART_0, str_bat_ok);
			ps2h_kb_status &= ~PS2H_KB_STAT_BAT_PASSED;
		}
		
		if(ps2h_kb_status & PS2H_KB_STAT_DATA_ERROR)
		{
			uart_write_string(UART_0, str_data_error);
			ps2h_kb_status &= ~PS2H_KB_STAT_DATA_ERROR;
		}
		
		if(ps2h_kb_status & PS2H_KB_STAT_PROT_ERROR)
		{
			uart_write_string(UART_0, str_prot_error);
			ps2h_kb_status &= ~PS2H_KB_STAT_PROT_ERROR;
		}
		
		if(ps2h_kb_status & PS2H_KB_STAT_TIM_ERROR)
		{
			uart_write_string(UART_0, str_tim_error);
			ps2h_kb_status &= ~PS2H_KB_STAT_TIM_ERROR;
		}
		
		if(ps2h_kb_status & PS2H_KB_STAT_ROLLOVER)
		{
			uart_write_string(UART_0, str_rollover);
			ps2h_kb_status &= ~PS2H_KB_STAT_ROLLOVER;
		}
		
		if(ps2h_kb_status & PS2H_KB_STAT_PAUSE_KEY)
		{
			uart_write_string(UART_0, str_pause_pressed);
			ps2h_kb_status &= ~PS2H_KB_STAT_PAUSE_KEY;
		}
		
		for(count = 0; count < PS2H_KB_MAX_NUM_PRESSED; ++count)
		{
			if(ps2h_kb_pressed_keys[count] && !pressed_keys_prev[count])
			{
				uart_write_string(UART_0, str_key_pressed);
				byte_to_hex((UINT8)(ps2h_kb_pressed_keys[count] >> 8), rx_str);
				byte_to_hex((UINT8)(ps2h_kb_pressed_keys[count]), rx_str + 2);
				rx_str[4] = '\n';
				rx_str[5] = '\0';
				uart_write_string(UART_0, rx_str);
			}
			
			if(pressed_keys_prev[count] && !ps2h_kb_pressed_keys[count])
			{
				uart_write_string(UART_0, str_key_released);
				byte_to_hex((UINT8)(pressed_keys_prev[count] >> 8), rx_str);
				byte_to_hex((UINT8)(pressed_keys_prev[count]), rx_str + 2);
				rx_str[4] = '\n';
				rx_str[5] = '\0';
				uart_write_string(UART_0, rx_str);
			}
			
			pressed_keys_prev[count] = ps2h_kb_pressed_keys[count];
		}
	}
}

