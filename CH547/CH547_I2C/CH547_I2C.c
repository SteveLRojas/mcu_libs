#include "CH547.H"
#include "CH547_GPIO.h"
#include "CH547_I2C.h"

//HINT: user can access host_ack and agent_ack directly
UINT8 host_ack;
UINT8 agent_ack;

UINT8 sda_port;
UINT8 sda_pin;
UINT8 scl_port;
UINT8 scl_pin;

void i2c_init(UINT8 sda_port_sel, UINT8 sda_pin_sel, UINT8 scl_port_sel, UINT8 scl_pin_sel)
{
	sda_port = sda_port_sel;
	sda_pin = sda_pin_sel;
	scl_port = scl_port_sel;
	scl_pin = scl_pin_sel;
	
	gpio_set_pin(sda_port, sda_pin);
	gpio_set_pin(scl_port, scl_pin);
}

void i2c_start(void)
{
	gpio_clear_pin(scl_port, scl_pin);	//while clock is low
	gpio_set_pin(sda_port, sda_pin);	//make sure sda is high
	gpio_set_pin(scl_port, scl_pin);	//while clock is high
	gpio_clear_pin(sda_port, sda_pin);	//bring sda low
}

void i2c_stop(void)
{
	gpio_clear_pin(scl_port, scl_pin);	//while clock is low
	gpio_clear_pin(sda_port, sda_pin);	//make sure sda is low
	gpio_set_pin(scl_port, scl_pin);	//while sck is high
	gpio_set_pin(sda_port, sda_pin);	//bring sda high
}

void i2c_write(UINT8 val)
{
	UINT8 d;
	for(d = 0; d < 8; ++d)
	{
		gpio_clear_pin(scl_port, scl_pin);			//while clock is low
		gpio_write_pin(sda_port, sda_pin, val & 0x80);	//write sda
		gpio_set_pin(scl_port, scl_pin);			//bring sck high
		val = val << 1;
	}
	gpio_clear_pin(scl_port, scl_pin);
	gpio_set_pin(sda_port, sda_pin);	//release sda
	gpio_set_pin(scl_port, scl_pin);	//bring sck high
	agent_ack = gpio_read_pin(sda_port, sda_pin) ^ 0x01;
}

UINT8 i2c_read(void)
{
	UINT8 val = 0;
	UINT8 d;
	for(d = 0; d < 8; ++d)
	{
		gpio_clear_pin(scl_port, scl_pin);
		gpio_set_pin(sda_port, sda_pin);	//release sda
		gpio_set_pin(scl_port, scl_pin);
		val = val << 1;
		val = val | gpio_read_pin(sda_port, sda_pin);
	}
	gpio_clear_pin(scl_port, scl_pin);
	gpio_write_pin(sda_port, sda_pin, host_ack ^ 0x01);
	gpio_set_pin(scl_port, scl_pin);
	return val;
}
