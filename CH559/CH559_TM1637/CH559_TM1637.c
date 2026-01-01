#define NO_XSFR_DEFINE
#include "CH559.H"
#include "CH559_GPIO.h"
#include "CH559_TM1637.h"

UINT8 code tm1637_hex_tab[16] =
{
	TM1637_SEG_A | TM1637_SEG_B | TM1637_SEG_C | TM1637_SEG_D | TM1637_SEG_E | TM1637_SEG_F,
	TM1637_SEG_B | TM1637_SEG_C,
	TM1637_SEG_A | TM1637_SEG_B | TM1637_SEG_D | TM1637_SEG_E | TM1637_SEG_G,
	TM1637_SEG_A | TM1637_SEG_B | TM1637_SEG_C | TM1637_SEG_D | TM1637_SEG_G,
	TM1637_SEG_B | TM1637_SEG_C | TM1637_SEG_F | TM1637_SEG_G,
	TM1637_SEG_A | TM1637_SEG_C | TM1637_SEG_D | TM1637_SEG_F | TM1637_SEG_G,
	TM1637_SEG_A | TM1637_SEG_C | TM1637_SEG_D | TM1637_SEG_E | TM1637_SEG_F | TM1637_SEG_G,
	TM1637_SEG_A | TM1637_SEG_B | TM1637_SEG_C,
	TM1637_SEG_A | TM1637_SEG_B | TM1637_SEG_C | TM1637_SEG_D | TM1637_SEG_E | TM1637_SEG_F | TM1637_SEG_G,
	TM1637_SEG_A | TM1637_SEG_B | TM1637_SEG_C | TM1637_SEG_D | TM1637_SEG_F | TM1637_SEG_G,
	TM1637_SEG_A | TM1637_SEG_B | TM1637_SEG_C | TM1637_SEG_D | TM1637_SEG_E | TM1637_SEG_G,
	TM1637_SEG_C | TM1637_SEG_D | TM1637_SEG_E | TM1637_SEG_F | TM1637_SEG_G,
	TM1637_SEG_C | TM1637_SEG_D | TM1637_SEG_E | TM1637_SEG_F,
	TM1637_SEG_B | TM1637_SEG_C | TM1637_SEG_D | TM1637_SEG_E | TM1637_SEG_G,
	TM1637_SEG_A | TM1637_SEG_D | TM1637_SEG_E | TM1637_SEG_F | TM1637_SEG_G,
	TM1637_SEG_A | TM1637_SEG_E | TM1637_SEG_F | TM1637_SEG_G
};

UINT8 tm1637_buf[TM1637_NUM_DIGITS];

void tm1637_init(UINT8 pulse_width)
{
	UINT8 count;
	
	gpio_set_pin(TM1637_PORT_DIO, TM1637_PIN_DIO);
	gpio_set_pin(TM1637_PORT_CLK, TM1637_PIN_CLK);
    pulse_width = pulse_width & 0x07;
	
	for(count = 0; count < TM1637_NUM_DIGITS; ++count)
	{
		tm1637_buf[count] = TM1637_SEG_G;
	}
	tm1637_update();
	
	tm1637_start();
	tm1637_write(TM1637_CMD_CONTROL | TM1637_OPT_DISP_ON | pulse_width);
	tm1637_stop();
}

//HINT: The TM1637 has an interface that is similar to I2C, but the order of the bits is reversed and there is no slave address.
void tm1637_start(void)
{
	gpio_clear_pin(TM1637_PORT_CLK, TM1637_PIN_CLK);	//while clock is low
	gpio_set_pin(TM1637_PORT_DIO, TM1637_PIN_DIO);		//make sure dio is high
	gpio_set_pin(TM1637_PORT_CLK, TM1637_PIN_CLK);		//while clock is high
	gpio_clear_pin(TM1637_PORT_DIO, TM1637_PIN_DIO);	//bring dio low
}

void tm1637_stop(void)
{
	gpio_clear_pin(TM1637_PORT_CLK, TM1637_PIN_CLK);	//while clock is low
	gpio_clear_pin(TM1637_PORT_DIO, TM1637_PIN_DIO);	//make sure dio is low
	gpio_set_pin(TM1637_PORT_CLK, TM1637_PIN_CLK);		//while clk is high
	gpio_set_pin(TM1637_PORT_DIO, TM1637_PIN_DIO);		//bring dio high
}

void tm1637_write(UINT8 val)
{
	UINT8 d;
	for(d = 0; d < 8; ++d)
	{
		gpio_clear_pin(TM1637_PORT_CLK, TM1637_PIN_CLK);	//while clock is low
		gpio_write_pin(TM1637_PORT_DIO, TM1637_PIN_DIO, val & 0x01);	//write sda
		gpio_set_pin(TM1637_PORT_CLK, TM1637_PIN_CLK);		//bring sck high
		val = val >> 1;
	}
	gpio_clear_pin(TM1637_PORT_CLK, TM1637_PIN_CLK);
	gpio_set_pin(TM1637_PORT_DIO, TM1637_PIN_DIO);		//release sda
	gpio_set_pin(TM1637_PORT_CLK, TM1637_PIN_CLK);		//bring sck high
}

UINT8 tm1637_read(void)
{
	UINT8 val = 0;
	UINT8 d;
	for(d = 0; d < 8; ++d)
	{
		gpio_clear_pin(TM1637_PORT_CLK, TM1637_PIN_CLK);
		gpio_set_pin(TM1637_PORT_DIO, TM1637_PIN_DIO);	//release sda
		gpio_set_pin(TM1637_PORT_CLK, TM1637_PIN_CLK);
		val = val >> 1;
		val = val | (gpio_read_pin(TM1637_PORT_DIO, TM1637_PIN_DIO) << 7);
	}
	gpio_clear_pin(TM1637_PORT_CLK, TM1637_PIN_CLK);
	gpio_set_pin(TM1637_PORT_DIO, TM1637_PIN_DIO);
	gpio_set_pin(TM1637_PORT_CLK, TM1637_PIN_CLK);
	return val;
}

void tm1637_set_byte(UINT8 idx, UINT8 val)
{
	idx = idx << 1;
	tm1637_set_digit(idx, val & 0x0F);
	tm1637_set_digit(++idx, val >> 4);
}

void tm1637_update(void)
{
	UINT8 count;
	
	tm1637_start();
	tm1637_write(TM1637_CMD_DATA | TM1637_OPT_WRITE_REG | TM1637_OPT_AUTO_INC);
	tm1637_stop();
	tm1637_start();
	tm1637_write(TM1637_CMD_ADDRESS);

	for(count = 0; count < TM1637_NUM_DIGITS; count += 1)
	{
		if(count < 3)
			tm1637_write(tm1637_buf[count + 3]);
		else
			tm1637_write(tm1637_buf[count - 3]);
	}
	
	tm1637_stop();
}
