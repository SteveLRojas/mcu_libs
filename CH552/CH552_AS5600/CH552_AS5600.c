#include "CH552.H"
#include "CH552_I2C.h"
#include "CH552_AS5600.h"

UINT8 as5600_read_byte(UINT8 addr)
{
	UINT8 temp;
	i2c_start();
	i2c_write(AS_DEV_ADDR_WRITE);
	i2c_write(addr);
	i2c_start();
	i2c_write(AS_DEV_ADDR_READ);
	host_ack = 0;
	temp = i2c_read();
	i2c_stop();
	return temp;
}

UINT16 as5600_read_word(UINT8 addr)
{
	UINT16 temp;
	i2c_start();
	i2c_write(AS_DEV_ADDR_WRITE);
	i2c_write(addr);
	i2c_start();
	i2c_write(AS_DEV_ADDR_READ);
	host_ack = 1;
	temp = (UINT16)i2c_read();
	temp = temp << 8;
	host_ack = 0;
	temp = temp | (UINT16)i2c_read();
	i2c_stop();
	return temp;
}

void as5600_write_byte(UINT8 addr, UINT8 val)
{
	i2c_start();
	i2c_write(AS_DEV_ADDR_WRITE);
	i2c_write(addr);
	i2c_write(val);
	i2c_stop();
}

void as5600_write_word(UINT8 addr, UINT16 val)
{
	i2c_start();
	i2c_write(AS_DEV_ADDR_WRITE);
	i2c_write(addr);
	i2c_write((UINT8)(val >> 8));
	i2c_write((UINT8)val);
	i2c_stop();
}
