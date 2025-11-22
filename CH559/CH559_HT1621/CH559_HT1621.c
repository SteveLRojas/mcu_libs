#define NO_XSFR_DEFINE
#include "CH559.H"
#include "CH559_GPIO.h"
#include "CH559_HT1621.h"

UINT8 code ht1621_hex_tab[16] =
{
	HT1621_SEG_A | HT1621_SEG_B | HT1621_SEG_C | HT1621_SEG_D | HT1621_SEG_E | HT1621_SEG_F,
	HT1621_SEG_B | HT1621_SEG_C,
	HT1621_SEG_A | HT1621_SEG_B | HT1621_SEG_D | HT1621_SEG_E | HT1621_SEG_G,
	HT1621_SEG_A | HT1621_SEG_B | HT1621_SEG_C | HT1621_SEG_D | HT1621_SEG_G,
	HT1621_SEG_B | HT1621_SEG_C | HT1621_SEG_F | HT1621_SEG_G,
	HT1621_SEG_A | HT1621_SEG_C | HT1621_SEG_D | HT1621_SEG_F | HT1621_SEG_G,
	HT1621_SEG_A | HT1621_SEG_C | HT1621_SEG_D | HT1621_SEG_E | HT1621_SEG_F | HT1621_SEG_G,
	HT1621_SEG_A | HT1621_SEG_B | HT1621_SEG_C,
	HT1621_SEG_A | HT1621_SEG_B | HT1621_SEG_C | HT1621_SEG_D | HT1621_SEG_E | HT1621_SEG_F | HT1621_SEG_G,
	HT1621_SEG_A | HT1621_SEG_B | HT1621_SEG_C | HT1621_SEG_D | HT1621_SEG_F | HT1621_SEG_G,
	HT1621_SEG_A | HT1621_SEG_B | HT1621_SEG_C | HT1621_SEG_D | HT1621_SEG_E | HT1621_SEG_G,
	HT1621_SEG_C | HT1621_SEG_D | HT1621_SEG_E | HT1621_SEG_F | HT1621_SEG_G,
	HT1621_SEG_C | HT1621_SEG_D | HT1621_SEG_E | HT1621_SEG_F,
	HT1621_SEG_B | HT1621_SEG_C | HT1621_SEG_D | HT1621_SEG_E | HT1621_SEG_G,
	HT1621_SEG_A | HT1621_SEG_D | HT1621_SEG_E | HT1621_SEG_F | HT1621_SEG_G,
	HT1621_SEG_A | HT1621_SEG_E | HT1621_SEG_F | HT1621_SEG_G
};

UINT8 ht1621_buf[HT1621_BUF_SIZE];
	
void ht1621_init(void)
{
	UINT8 count;
	UINT8 addr;
	
	gpio_set_pin(HT1621_PORT_CS, HT1621_PIN_CS);
	gpio_set_pin(HT1621_PORT_WR, HT1621_PIN_WR);
	gpio_set_pin(HT1621_PORT_DATA, HT1621_PIN_DATA);
	
	ht1621_send_command(HT1621_CMD_SYS_EN);
	ht1621_send_command(HT1621_CMD_RC_256K);
	ht1621_send_command(HT1621_CMD_BIAS_COM | HT1621_COM_4 | HT1621_BIAS_1_2);
	ht1621_send_command(HT1621_CMD_LCD_ON);
	
	for(count = 0; count < HT1621_BUF_SIZE; ++count)
	{
		addr = count << 1;
		ht1621_send_data(addr, 0x00);
		ht1621_send_data(++addr, 0x02);
	}
}

void ht1621_send_command(UINT8 command)
{
	UINT8 count;
	
	gpio_clear_pin(HT1621_PORT_CS, HT1621_PIN_CS);
	
	gpio_set_pin(HT1621_PORT_DATA, HT1621_PIN_DATA);
	gpio_clear_pin(HT1621_PORT_WR, HT1621_PIN_WR);
	gpio_set_pin(HT1621_PORT_WR, HT1621_PIN_WR);
	
	gpio_clear_pin(HT1621_PORT_DATA, HT1621_PIN_DATA);
	gpio_clear_pin(HT1621_PORT_WR, HT1621_PIN_WR);
	gpio_set_pin(HT1621_PORT_WR, HT1621_PIN_WR);
	gpio_clear_pin(HT1621_PORT_WR, HT1621_PIN_WR);
	gpio_set_pin(HT1621_PORT_WR, HT1621_PIN_WR);
	
	for(count = 0; count < 8; count += 1)
	{
		gpio_write_pin(HT1621_PORT_DATA, HT1621_PIN_DATA, command & 0x80);
		gpio_clear_pin(HT1621_PORT_WR, HT1621_PIN_WR);
		gpio_set_pin(HT1621_PORT_WR, HT1621_PIN_WR);
		command = command << 1;
	}
	
	gpio_clear_pin(HT1621_PORT_WR, HT1621_PIN_WR);
	gpio_set_pin(HT1621_PORT_WR, HT1621_PIN_WR);
	
	gpio_set_pin(HT1621_PORT_CS, HT1621_PIN_CS);
}

void ht1621_send_data(UINT8 addr, UINT8 val)
{
	UINT8 count;
	
	gpio_clear_pin(HT1621_PORT_CS, HT1621_PIN_CS);
	
	gpio_set_pin(HT1621_PORT_DATA, HT1621_PIN_DATA);
	gpio_clear_pin(HT1621_PORT_WR, HT1621_PIN_WR);
	gpio_set_pin(HT1621_PORT_WR, HT1621_PIN_WR);
	
	gpio_clear_pin(HT1621_PORT_DATA, HT1621_PIN_DATA);
	gpio_clear_pin(HT1621_PORT_WR, HT1621_PIN_WR);
	gpio_set_pin(HT1621_PORT_WR, HT1621_PIN_WR);
	
	gpio_set_pin(HT1621_PORT_DATA, HT1621_PIN_DATA);
	gpio_clear_pin(HT1621_PORT_WR, HT1621_PIN_WR);
	gpio_set_pin(HT1621_PORT_WR, HT1621_PIN_WR);
	
	for(count = 0; count < 6; ++count)
	{
		gpio_write_pin(HT1621_PORT_DATA, HT1621_PIN_DATA, addr & 0x20);
		gpio_clear_pin(HT1621_PORT_WR, HT1621_PIN_WR);
		gpio_set_pin(HT1621_PORT_WR, HT1621_PIN_WR);
		addr = addr << 1;
	}
	
	for(count = 0; count < 4; ++count)
	{
		gpio_write_pin(HT1621_PORT_DATA, HT1621_PIN_DATA, val & 0x08);
		gpio_clear_pin(HT1621_PORT_WR, HT1621_PIN_WR);
		gpio_set_pin(HT1621_PORT_WR, HT1621_PIN_WR);
		val = val << 1;
	}
	
	gpio_clear_pin(HT1621_PORT_WR, HT1621_PIN_WR);
	gpio_set_pin(HT1621_PORT_WR, HT1621_PIN_WR);
	
	gpio_set_pin(HT1621_PORT_CS, HT1621_PIN_CS);
}

void ht1621_set_byte(UINT8 idx, UINT8 val)
{
	idx = idx << 1;
	ht1621_set_digit(idx, val & 0x0F);
	ht1621_set_digit(++idx, val >> 4);
}

void ht1621_update(void)
{
	UINT8 count;
	UINT8 digit;
	UINT8 addr;
	
	for(count = 0; count < HT1621_BUF_SIZE; count += 1)
	{
		digit = ht1621_buf[count];
		addr = count << 1;
		ht1621_send_data(addr, digit & 0x0F);
		ht1621_send_data(addr + 1, digit >> 4);
	}
}
