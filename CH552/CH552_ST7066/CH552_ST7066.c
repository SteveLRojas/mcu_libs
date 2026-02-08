#include "CH552.H"
#include "CH552_GPIO.h"
#include "CH552_RCC.h"
#include "CH552_SPI.h"
#include "CH552_ST7066.h"

UINT8 code st7066_line_map[4] = {0x00, 0x40, 0x14, 0x54};

void st7066_init(void)
{
	UINT8 row;
	UINT8 col;
	
	spi_init(ST7066_SPI_CLK_DIV, SPI_MODE_3);
#if ST7066_FN_SET_INIT0_TIM
	st7066_send_command(ST7066_COM_FUNCTION_SET | ST7066_FUNCTION_SET_8_BIT | ST7066_FUNCTION_SET_2_LINE);
	rcc_delay_us(ST7066_FN_SET_INIT0_TIM);
#endif
#if ST7066_FN_SET_INIT1_TIM
	st7066_send_command(ST7066_COM_FUNCTION_SET | ST7066_FUNCTION_SET_8_BIT | ST7066_FUNCTION_SET_2_LINE);
	rcc_delay_us(ST7066_FN_SET_INIT1_TIM);
#endif
	st7066_send_command(ST7066_COM_FUNCTION_SET | ST7066_FUNCTION_SET_8_BIT | ST7066_FUNCTION_SET_2_LINE);
	rcc_delay_us(ST7066_DEFAULT_CMD_TIM);
	st7066_send_command(ST7066_COM_FUNCTION_SET | ST7066_FUNCTION_SET_8_BIT | ST7066_FUNCTION_SET_2_LINE);
	rcc_delay_us(ST7066_DEFAULT_CMD_TIM);
	st7066_send_command(ST7066_COM_DISP_ON_OFF);
	rcc_delay_us(ST7066_DEFAULT_CMD_TIM);
	st7066_clear_display();
	st7066_send_command(ST7066_COM_ENTRY_MODE | ST7066_ENTRY_MODE_INC | ST7066_ENTRY_MODE_SHIFT_DIS);
	rcc_delay_us(ST7066_DEFAULT_CMD_TIM);
	st7066_send_command(ST7066_COM_DISP_ON_OFF | ST7066_DISP_ON_OFF_DISP_ON);
	rcc_delay_us(ST7066_DEFAULT_CMD_TIM);
	
	for(row = 0; row < ST7066_NUM_ROWS; ++row)
	{
		st7066_send_command(ST7066_COM_SET_DDRAM_ADDR | st7066_line_map[row]);
		rcc_delay_us(ST7066_DEFAULT_CMD_TIM);
		for(col = 0; col < ST7066_NUM_COLUMNS; ++col)
		{
			st7066_send_data(0xEB);
			rcc_delay_us(ST7066_WRITE_DATA_TIM);
		}
	}
	
	st7066_send_command(ST7066_COM_RETURN_HOME);
	rcc_delay_us(ST7066_RETURN_HOME_TIM);
}

void st7066_send_command(UINT8 command)
{
	gpio_clear_pin(ST7066_PORT_RS, ST7066_PIN_RS);
	spi_transfer(command);
	gpio_set_pin(ST7066_PORT_E, ST7066_PIN_E);
#if ST7066_E_PULSE_TIM
	rcc_delay_us(ST7066_E_PULSE_TIM);
#endif
	gpio_clear_pin(ST7066_PORT_E, ST7066_PIN_E);
}

void st7066_send_data(UINT8 val)
{
	gpio_set_pin(ST7066_PORT_RS, ST7066_PIN_RS);
	spi_transfer(val);
	gpio_set_pin(ST7066_PORT_E, ST7066_PIN_E);
#if ST7066_E_PULSE_TIM
	rcc_delay_us(ST7066_E_PULSE_TIM);
#endif
	gpio_clear_pin(ST7066_PORT_E, ST7066_PIN_E);
}

void st7066_set_cursor(UINT8 row, UINT8 col)
{
	UINT8 ddram_addr;
	ddram_addr = st7066_line_map[row] + col;
	st7066_send_command(ST7066_COM_SET_DDRAM_ADDR | ddram_addr);
	rcc_delay_us(ST7066_DEFAULT_CMD_TIM);
}

//HINT: Increments cursor position but does not handle wrap-around
void st7066_write_char(char val)
{
	st7066_send_data(val);
	rcc_delay_us(ST7066_WRITE_DATA_TIM);
}

//HINT: Does not handle wrap-around
void st7066_write_string(char* str)
{
	while(*str)
	{
		st7066_send_data(*str);
		rcc_delay_us(ST7066_WRITE_DATA_TIM);
		str += 1;
	}
}

//HINT: Clears the entire display and sets the cursor to 0,0
void st7066_clear_display(void)
{
	st7066_send_command(ST7066_COM_CLEAR_DISP);
	rcc_delay_us(ST7066_CLEAR_DISP_TIM);
}

//HINT: Clears the specified line and sets the cursor to the start of that line
void st7066_clear_line(UINT8 row)
{
	UINT8 col;
	
	st7066_send_command(ST7066_COM_SET_DDRAM_ADDR | st7066_line_map[row]);
	rcc_delay_us(ST7066_DEFAULT_CMD_TIM);
	for(col = 0; col < ST7066_NUM_COLUMNS; ++col)
	{
		st7066_send_data(' ');
		rcc_delay_us(ST7066_WRITE_DATA_TIM);
	}
	st7066_send_command(ST7066_COM_SET_DDRAM_ADDR | st7066_line_map[row]);
	rcc_delay_us(ST7066_DEFAULT_CMD_TIM);
}
