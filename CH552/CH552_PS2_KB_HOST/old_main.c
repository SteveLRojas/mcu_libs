#include "CH552.H"
#include "CH552_RCC.h"
#include "CH552_GPIO.h"
#include "CH552_UART.h"
#include "CH552_TIMER.h"
#include "CH552_PS2_HOST.h"

#define  BAUD_RATE  125000ul

char code test_string[] = "Unicorn\n";
char code str_bad_command[] = "Bad command!\n";
char code str_rx_error[] = "RX error flag set\n";
char code str_tx_error[] = "TX error flag set\n";
char code str_tx_complete[] = "TX complete\n";

UINT8 kb_has_received = 0;
UINT8 kb_received_data;
UINT8 kb_received_status;
UINT8 kb_has_transmitted = 0;
UINT8 kb_transmitted_status;

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

void on_kb_rx(void)
{
	kb_has_received = 1;
	kb_received_data = ps2_p0_shift;
	kb_received_status = ps2_p0_rx_error;
}

void on_kb_tx(void)
{
	kb_has_transmitted = 1;
	kb_transmitted_status = ps2_p0_tx_error;
}

void kb_send_byte(UINT8 val)
{
	ps2_p0_shift = val;
	ps2_p0_set_line_state(PS2_S_COM_DISABLED);
	rcc_delay_us(100);
	ps2_p0_set_line_state(PS2_S_TX_REQUEST);
}

int main()
{
	char last_keep_str[4];
	char rx_str[4];
	UINT8 datagram[2];
	UINT8 temp;
	UINT8 count;
	
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
	
	ps2_p0_rx_callback = on_kb_rx;
	ps2_p0_tx_callback = on_kb_tx;
	ps2_host_p0_init();
	
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
			kb_send_byte(RESET_KEEP);
			gpio_write_pin(GPIO_PORT_1, GPIO_PIN_6, gpio_read_pin(GPIO_PORT_1, GPIO_PIN_7));
		}
		
		if(kb_has_received)
		{
			byte_to_hex(kb_received_data, rx_str);
			rx_str[2] = '\n';
			rx_str[3] = '\0';
			uart_write_string(UART_0, rx_str);
			if(kb_received_status)
			{
				uart_write_string(UART_0, str_rx_error);
			}
			kb_has_received = 0;
		}
		
		if(kb_has_transmitted)
		{
			uart_write_string(UART_0, str_tx_complete);
			if(ps2_p0_tx_error)
			{
				uart_write_string(UART_0, str_tx_error);
			}
			kb_has_transmitted = 0;
		}
	}
}

