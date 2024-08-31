#include "CH552.H"
#include "System.h"
#include "CH552_GPIO.h"
#include "CH552_UART.h"
#include "CH552_TIMER.h"
#include "CH552_TKEY.h"

extern signed short tkey_offsets[6];

char code test_string[] = "Unicorn\n";
char code str_offsets[] = "Offsets: \n";
char code str_keys_raw[] = "Raw Keys: \n";
char code str_keys[] = "Keys: \n";

//Pins:
// KEY1 = P14
// KEY2 = P15
// KEY3 = P16
// KEY4 = P17
// RXD = P30
// TXD = P31
// UDP = P36
// UDM = P37

char line_buf[16];

char code hex_table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
void byte_to_hex(UINT8 value, char* buff)
{
	buff[0] = hex_table[(value >> 4) & 0x0f];
	buff[1] = hex_table[(value) & 0x0f];
	buff[2] = '\0';
}

void print_bytes_as_hex(UINT8* source, UINT8 num_bytes)
{
	UINT8 idx;
	
	for(idx = 0; idx < num_bytes; ++idx)
	{
		line_buf[0] = hex_table[idx >> 4];
		line_buf[1] = hex_table[idx & 0x0F];
		line_buf[2] = ':';
		line_buf[3] = ' ';
		line_buf[4] = hex_table[source[idx] >> 4];
		line_buf[5] = hex_table[source[idx] & 0x0F];
		line_buf[6] = '\n';
		line_buf[7] = '\0';
		uart_write_string(UART_0, line_buf);
	}
}

void print_words_as_hex(UINT16* source, UINT8 num_words)
{
	UINT8 idx;
	
	for(idx = 0; idx < num_words; ++idx)
	{
		line_buf[0] = hex_table[idx >> 4];
		line_buf[1] = hex_table[idx & 0x0F];
		line_buf[2] = ':';
		line_buf[3] = ' ';
		line_buf[4] = hex_table[source[idx] >> 12];
		line_buf[5] = hex_table[(source[idx] >> 8) & 0x0F];
		line_buf[6] = hex_table[(source[idx] >> 4) & 0x0F];
		line_buf[7] = hex_table[source[idx] & 0x0F];
		line_buf[8] = '\n';
		line_buf[9] = '\0';
		uart_write_string(UART_0, line_buf);
	}
}

int main()
{
	char last_keep_str[4];
	
	CfgFsys();
	
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_1, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_3, GPIO_PIN_1);
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_3, GPIO_PIN_0);
	
	uart0_init(TIMER_1, BAUD_RATE, UART_0_P30_P31);
	timer_init(TIMER_0, NULL);
	timer_set_period(TIMER_0, FREQ_SYS / 1000ul);	//period is 1ms
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	timer_long_delay(TIMER_0, 250);
	uart_write_string(UART_0, test_string);
	byte_to_hex(RESET_KEEP, last_keep_str);
	last_keep_str[2] = '\n';
	last_keep_str[3] = '\0';
	uart_write_string(UART_0, last_keep_str);
	
	tkey_init(TKEY_FAST);
	tkey_schedule[0] = TKEY_TIN2_P14;
	tkey_schedule[1] = TKEY_TIN3_P15;
	tkey_schedule[2] = TKEY_TIN4_P16;
	tkey_schedule[3] = TKEY_TIN5_P17;
	tkey_schedule_len = 4;
	tkey_init_schedule();
	tkey_calibrate();
	
	uart_write_string(UART_0, str_offsets);
	print_words_as_hex((UINT16*)tkey_offsets, 6);
	
	while(TRUE)
	{
		tkey_run_schedule(4);
		timer_long_delay(TIMER_0, 250);
		uart_write_string(UART_0, str_keys_raw);
		print_words_as_hex(tkey_results_raw, 4);
		uart_write_string(UART_0, str_keys);
		print_bytes_as_hex(tkey_results, 4);
	}
}

