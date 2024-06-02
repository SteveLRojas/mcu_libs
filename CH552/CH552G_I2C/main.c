#include "CH552.H"
#include "System.h"
#include "CH552_GPIO.h"
#include "CH552_UART.h"
#include "CH552_SPI.h"
#include "CH552_TIMER.h"
#include "CH552_I2C.h"

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
	
char code test_string[] = "Booba";

//Pins:
// LED = P11
// SCS = P14
// MOSI = P15
// MISO = P16
// SCK = P17
// RXD = P30
// TXD = P31
// SDA = P33
// SCL = P34

int main()
{
	UINT8 temp;
	UINT32 baud_temp;
	UINT8 spi_speed = 4;
	UINT8 spi_mode = SPI_MODE_0;
	UINT8 cmd_count = 0;
	
	CfgFsys();	//CH559 clock selection configuration
	
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_1, GPIO_PIN_6);
	spi_init(4, SPI_MODE_0);
	
	gpio_set_mode(GPIO_MODE_OD_PU, GPIO_PORT_3, GPIO_PIN_3 | GPIO_PIN_4);
	i2c_init(GPIO_PORT_3, GPIO_PIN_3, GPIO_PORT_3, GPIO_PIN_4);
	
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_3, GPIO_PIN_1);
	gpio_set_mode(GPIO_MODE_INPUT, GPIO_PORT_3, GPIO_PIN_0);
	uart0_init(TIMER_1, BAUD_RATE, UART_0_P30_P31);
	
	timer_init(TIMER_0, NULL);
	timer_set_period(TIMER_0, FREQ_SYS / 1000ul);	//period is 1ms
	EA = 1;	//enable interupts
	E_DIS = 0;
	
	//Blink LED once
	gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_1);
	timer_long_delay(TIMER_0, 250);
	gpio_set_pin(GPIO_PORT_1, GPIO_PIN_1);
	timer_long_delay(TIMER_0, 250);
	uart_write_string(UART_0, test_string);
	
	while(TRUE)
	{	
		if(uart_bytes_available(UART_0))
		{
			temp = uart_read_byte(UART_0);
			switch(temp)
			{
				case CMD_NOP:
					break;
				case CMD_TEST:
					temp = uart_read_byte(UART_0);
					uart_write_byte(UART_0, temp);
					break;
				case CMD_BAUD:
					baud_temp = 0;
					baud_temp |= uart_read_byte(UART_0);
					baud_temp = baud_temp << 8;
					baud_temp |= uart_read_byte(UART_0);
					baud_temp = baud_temp << 7;
					uart0_init(TIMER_1, baud_temp, UART_0_P30_P31);
					break;
				case CMD_SPI_SPEED:
					spi_speed = uart_read_byte(UART_0);
					spi_init(spi_speed, spi_mode);
					break;
				case CMD_SPI_MODE:
					temp = uart_read_byte(UART_0);
					if(temp == 3)
						spi_mode = SPI_MODE_3;
					else
						spi_mode = SPI_MODE_0;
					spi_init(spi_speed, spi_mode);
					break;
				case CMD_SPI_CHIPSEL:
					temp = uart_read_byte(UART_0);
					gpio_write_pin(GPIO_PORT_1, GPIO_PIN_4, temp);
					break;
				case CMD_SPI_TRANSFER:
					temp = uart_read_byte(UART_0);
					temp = spi_transfer(temp);
					uart_write_byte(UART_0, temp);
					break;
				case CMD_I2C_SPEED:
					(void)uart_read_byte(UART_0);
					(void)uart_read_byte(UART_0);
					break;
				case CMD_I2C_START:
					i2c_start();
					break;
				case CMD_I2C_STOP:
					i2c_stop();
					break;
				case CMD_I2C_WRITE:
					temp = uart_read_byte(UART_0);
					i2c_write(temp);
					break;
				case CMD_I2C_READ:
					temp = i2c_read();
					uart_write_byte(UART_0, temp);
					break;
				case CMD_I2C_HOST_ACK:
					host_ack = uart_read_byte(UART_0);
					break;
				case CMD_I2C_AGENT_ACK:
					uart_write_byte(UART_0, agent_ack);
					break;
				default:
					break;
			}
			++cmd_count;
			gpio_write_pin(GPIO_PORT_1, GPIO_PIN_1, ~cmd_count & 0x01);
		}
	}
}

