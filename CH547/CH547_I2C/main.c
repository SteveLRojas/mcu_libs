#include "CH547.H"
#include "CH547_RCC.h"
#include "CH547_GPIO.h"
#include "CH547_TIMER.h"
#include "CH547_USB_CDC.h"
#include "CH547_SPI.h"
#include "CH547_I2C.h"

#define CMD_NOP 0x00			//Expects 0 bytes, returns 0
#define CMD_TEST 0x01			//Expects 1 byte, returns 1
#define CMD_BAUD 0x02			//Expects 2 bytes, returns 0
	
#define CMD_SPI_SPEED 0x03		//Expects 1 byte, returns 0
#define CMD_SPI_MODE 0x04		//Expects 1 byte, returns 0
#define CMD_SPI_CHIPSEL 0x05	//Expects 1 byte, returns 0
#define CMD_SPI_TRANSFER 0x06	//Expects 1 byte, returns 1
	
#define CMD_I2C_SPEED 0x07		//Expects 2 bytes, returns 0
#define CMD_I2C_START 0x08		//Expects 0 bytes, returns 0
#define CMD_I2C_STOP 0x09		//Expects 0 bytes, returns 0
#define CMD_I2C_WRITE 0x0A		//Expects 1 byte, returns 0
#define CMD_I2C_READ 0x0B		//Expects 0 bytes, returns 1
#define CMD_I2C_HOST_ACK 0x0C	//Expects 1 byte, returns 0
#define CMD_I2C_AGENT_ACK 0x0D	//Expects 0 bytes, returns 1

char code test_string[] = "Unicorn";

//Pins:
// I2C_SDA = P12
// I2C_SCL = P13
// SPI_SCS = P14
// SPI_MOSI = P15
// SPI_MISO = P16
// SPI_SCK = P17
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

int main()
{
	char last_keep_str[4];
	UINT8 prev_control_line_state;
	UINT8 temp;
	UINT8 spi_speed = 4;
	UINT8 spi_mode = SPI_MODE_0;
	
	rcc_set_clk_freq(RCC_CLK_FREQ_24M);
	
	// Configure pins
	gpio_set_mode(GPIO_MODE_OD, GPIO_PORT_1, GPIO_PIN_2 | GPIO_PIN_3);	//I2C_SDA, I2C_SCL
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);	//SPI_SCS, SPI_MOSI, SPI_SCK
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_1, GPIO_PIN_6);	//SPI_MISO
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_2, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);	//LED0, LED1, LED2, LED3
	
	timer_init(TIMER_0, NULL);
	timer_set_period(TIMER_0, FREQ_SYS / 1000ul);	//period is 1ms
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	spi_init(4, SPI_MODE_0);
	i2c_init(GPIO_PORT_3, GPIO_PIN_3, GPIO_PORT_3, GPIO_PIN_4);
	
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

	while(TRUE)
	{	
		if(cdc_bytes_available())
		{
			temp = cdc_read_byte();
			gpio_toggle_pin(GPIO_PORT_2, GPIO_PIN_0);
			switch(temp)
			{
				case CMD_NOP:
					break;
				case CMD_TEST:
					temp = cdc_read_byte();
					cdc_write_byte(temp);
					break;
				case CMD_BAUD:
					(void)cdc_read_byte();
					(void)cdc_read_byte();
					break;
				case CMD_SPI_SPEED:
					spi_speed = cdc_read_byte();
					spi_init(spi_speed, spi_mode);
					break;
				case CMD_SPI_MODE:
					temp = cdc_read_byte();
					if(temp == 3)
						spi_mode = SPI_MODE_3;
					else
						spi_mode = SPI_MODE_0;
					spi_init(spi_speed, spi_mode);
					break;
				case CMD_SPI_CHIPSEL:
					temp = cdc_read_byte();
					gpio_write_pin(GPIO_PORT_1, GPIO_PIN_4, temp);
					break;
				case CMD_SPI_TRANSFER:
					temp = cdc_read_byte();
					temp = spi_transfer(temp);
					cdc_write_byte(temp);
					break;
				case CMD_I2C_SPEED:
					(void)cdc_read_byte();
					(void)cdc_read_byte();
					break;
				case CMD_I2C_START:
					i2c_start();
					break;
				case CMD_I2C_STOP:
					i2c_stop();
					break;
				case CMD_I2C_WRITE:
					temp = cdc_read_byte();
					i2c_write(temp);
					break;
				case CMD_I2C_READ:
					temp = i2c_read();
					cdc_write_byte(temp);
					break;
				case CMD_I2C_HOST_ACK:
					host_ack = cdc_read_byte();
					break;
				case CMD_I2C_AGENT_ACK:
					cdc_write_byte(agent_ack);
					break;
				default:
					break;
			}
			gpio_write_pin(GPIO_PORT_2, GPIO_PIN_1, gpio_read_pin(GPIO_PORT_2, GPIO_PIN_0));
		}
		
		if(prev_control_line_state != cdc_control_line_state)
		{
			cdc_set_serial_state(cdc_control_line_state & 3);
			prev_control_line_state = cdc_control_line_state;
		}
	}
}
