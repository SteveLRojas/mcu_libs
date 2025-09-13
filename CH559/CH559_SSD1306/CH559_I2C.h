#ifndef _CH559_I2C_H_
#define _CH559_I2C_H_

extern UINT8 host_ack;
extern UINT8 agent_ack;

void i2c_init(UINT8 sda_port_sel, UINT8 sda_pin_sel, UINT8 scl_port_sel, UINT8 scl_pin_sel);
void i2c_start(void);
void i2c_stop(void);
void i2c_write(UINT8 val);
UINT8 i2c_read(void);

#endif
