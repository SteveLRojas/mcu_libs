#define NO_XSFR_DEFINE
#include "CH559.H"
#include "CH559_GPIO.h"
#include "CH559_MAX7219.h"

UINT8 code max7219_hex_tab[16] =
{
	MAX7219_SEG_A | MAX7219_SEG_B | MAX7219_SEG_C | MAX7219_SEG_D | MAX7219_SEG_E | MAX7219_SEG_F,
	MAX7219_SEG_B | MAX7219_SEG_C,
	MAX7219_SEG_A | MAX7219_SEG_B | MAX7219_SEG_D | MAX7219_SEG_E | MAX7219_SEG_G,
	MAX7219_SEG_A | MAX7219_SEG_B | MAX7219_SEG_C | MAX7219_SEG_D | MAX7219_SEG_G,
	MAX7219_SEG_B | MAX7219_SEG_C | MAX7219_SEG_F | MAX7219_SEG_G,
	MAX7219_SEG_A | MAX7219_SEG_C | MAX7219_SEG_D | MAX7219_SEG_F | MAX7219_SEG_G,
	MAX7219_SEG_A | MAX7219_SEG_C | MAX7219_SEG_D | MAX7219_SEG_E | MAX7219_SEG_F | MAX7219_SEG_G,
	MAX7219_SEG_A | MAX7219_SEG_B | MAX7219_SEG_C,
	MAX7219_SEG_A | MAX7219_SEG_B | MAX7219_SEG_C | MAX7219_SEG_D | MAX7219_SEG_E | MAX7219_SEG_F | MAX7219_SEG_G,
	MAX7219_SEG_A | MAX7219_SEG_B | MAX7219_SEG_C | MAX7219_SEG_D | MAX7219_SEG_F | MAX7219_SEG_G,
	MAX7219_SEG_A | MAX7219_SEG_B | MAX7219_SEG_C | MAX7219_SEG_D | MAX7219_SEG_E | MAX7219_SEG_G,
	MAX7219_SEG_C | MAX7219_SEG_D | MAX7219_SEG_E | MAX7219_SEG_F | MAX7219_SEG_G,
	MAX7219_SEG_C | MAX7219_SEG_D | MAX7219_SEG_E | MAX7219_SEG_F,
	MAX7219_SEG_B | MAX7219_SEG_C | MAX7219_SEG_D | MAX7219_SEG_E | MAX7219_SEG_G,
	MAX7219_SEG_A | MAX7219_SEG_D | MAX7219_SEG_E | MAX7219_SEG_F | MAX7219_SEG_G,
	MAX7219_SEG_A | MAX7219_SEG_E | MAX7219_SEG_F | MAX7219_SEG_G
};

UINT8 max7219_buf[MAX7219_NUM_DIGITS];

void max7219_init(UINT8 intensity)
{
	UINT8 count;
    intensity = intensity & 0x0F;

    max7219_shift_out(MAX7219_CMD_DECODE_MODE);  //bypass decoding for all digits
    max7219_shift_out(MAX7219_CMD_INTENSITY | (UINT16)intensity);
    max7219_shift_out(MAX7219_CMD_SCAN_LIMIT | (MAX7219_NUM_DIGITS - 1));
    max7219_shift_out(MAX7219_CMD_SHUTDOWN | MAX7219_NORM_MODE);
    max7219_shift_out(MAX7219_CMD_DISP_TEST | MAX7219_TEST_DIS);
	
	for(count = 0; count < MAX7219_NUM_DIGITS; ++count)
	{
		max7219_buf[count] = MAX7219_SEG_G;
	}
	max7219_update();
}

void max7219_shift_out(UINT16 val)
{
    UINT8 count;
	gpio_clear_pin(MAX7219_PORT_CS, MAX7219_PIN_CS);

    for(count = 0; count < 16; ++count)
    {
        gpio_write_pin(MAX7219_PORT_DIN, MAX7219_PIN_DIN, val >> 15);
        val = val << 1;
        gpio_set_pin(MAX7219_PORT_CLK, MAX7219_PIN_CLK);
        gpio_clear_pin(MAX7219_PORT_CLK, MAX7219_PIN_CLK);
    }

    gpio_set_pin(MAX7219_PORT_CS, MAX7219_PIN_CS);
}

void max7219_set_byte(UINT8 idx, UINT8 val)
{
	idx = idx << 1;
	max7219_set_digit(idx, val & 0x0F);
	max7219_set_digit(++idx, val >> 4);
}

void max7219_update(void)
{
	UINT8 count;
	UINT16 digit;

	for(count = 0; count < MAX7219_NUM_DIGITS; count += 1)
	{
		digit = count + 1;
		digit = (digit << 8) | (UINT16)(max7219_buf[count]);
		max7219_shift_out(digit);
	}
}
