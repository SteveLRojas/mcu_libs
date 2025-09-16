#include "CH547.H"
#include "CH547_RCC.h"
#include "CH547_GPIO.h"
#include "CH547_TIMER.h"
#include "CH547_USB_CDC.h"
#include "CH547_SPI.h"

char code unicorn_string[] = "Unicorn\n";
char code dragon_string[] = "Dragon\n";

//Pins:
// LED1 = P11
// SPI_NCS = P14
// SPI_MOSI = P15
// SPI_MISO = P16
// SPI_SCK = P17
// LED2 = P22
// LED3 = P34
// UDM  = P50
// UDP  = P51

void byte_to_hex(UINT8 value, char* buff)
{
	const char table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	buff[0] = table[(value >> 4) & 0x0f];
	buff[1] = table[(value) & 0x0f];
	buff[2] = '\0';
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
	char last_keep_str[4];
	UINT8 prev_control_line_state;
	UINT8 time;
	
	rcc_set_clk_freq(RCC_CLK_FREQ_24M);
	
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);	//LED1, SPI_NCS, SPI_MOSI, SPI_SCK
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_1, GPIO_PIN_6);	//SPI_MISO
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_2, GPIO_PIN_2);	//LED2
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_3, GPIO_PIN_4);	//LED3
	spi_init(4, SPI_MODE_3);
	
	timer_init(TIMER_0, NULL);
	timer_set_period(TIMER_0, FREQ_SYS / 1000ul);	//period is 1ms
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_1);
	timer_long_delay(TIMER_0, 250);
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_1);
	timer_long_delay(TIMER_0, 250);
	
	cdc_init();
	cdc_set_serial_state(CDC_SS_TXCARRIER | CDC_SS_RXCARRIER);
	prev_control_line_state = cdc_control_line_state;
	while(!cdc_config);
	timer_long_delay(TIMER_0, 250);
	
	if(RESET_KEEP)
		cdc_write_string(dragon_string);
	else
		cdc_write_string(unicorn_string);
	
	byte_to_hex(RESET_KEEP, last_keep_str);
	last_keep_str[2] = '\n';
	last_keep_str[3] = '\0';
	cdc_write_string(last_keep_str);

	while(TRUE)
	{
		++time;
		gpio_write_pin(GPIO_PORT_1, GPIO_PIN_1, time & 0x80);
		
		if(cdc_bytes_available())
		{
			RESET_KEEP = cdc_read_byte();
			RESET_KEEP = spi_transfer(RESET_KEEP);
			cdc_write_byte(RESET_KEEP);
		}
		
		if(prev_control_line_state != cdc_control_line_state)
		{
			cdc_set_serial_state(cdc_control_line_state & 3);
			prev_control_line_state = cdc_control_line_state;
		}
		
		rcc_delay_us(1000);
	}
}
