#ifndef _CH559_ST7920_H_
#define _CH559_ST7920_H_

//HINT: Pin options. Use defines from GPIO and SPI libraries.
#define ST7920_PORT_CS	GPIO_PORT_2
#define ST7920_PIN_CS	GPIO_PIN_0

//HINT: The maximum clock speed is 2.5 MHz.
#define ST7920_SPI_CLK_DIV	20
#define ST7920_SPI_MODULE	SPI_1

#define ST7920_NUM_COLUMNS	128
#define ST7920_NUM_ROWS		64
#define ST7920_ROW_SIZE		((ST7920_NUM_COLUMNS + 7) / 8)
#define ST7920_BUF_SIZE		ST7920_ROW_SIZE * ST7920_NUM_ROWS

#define ST7920_FN_SET_INIT0_TIM	100
#define ST7920_FN_SET_INIT1_TIM	37
#define ST7920_CLEAR_DISP_TIM	1600
#define ST7920_DEFAULT_CMD_TIM	72

#define ST7920_COM_CLEAR_DISP		0x01
#define ST7920_COM_RETURN_HOME		0x02
#define ST7920_COM_ENTRY_MODE		0x04
#define ST7920_COM_DISP_ON_OFF		0x08
#define ST7920_COM_CURS_DISP_SHIFT	0x10
#define ST7920_COM_FUNCTION_SET		0x20
#define ST7920_COM_SET_CGRAM_ADDR	0x40
#define ST7920_COM_SET_DDRAM_ADDR	0x80

#define ST7920_EXT_COM_STANDBY		0x01
#define ST7920_EXT_COM_SCRL_RAM_SEL	0x02
#define ST7920_EXT_COM_REVERSE		0x04
#define ST7920_EXT_COM_FUNCTION_SET	0x20
#define ST7920_EXT_COM_IRAM_ADDR	0x40
#define ST7920_EXT_COM_GDRAM_ADDR	0x80

#define ST7920_ENTRY_MODE_INC		0x02
#define ST7920_ENTRY_MODE_DEC		0x00
#define ST7920_ENTRY_MODE_SHIFT_EN	0x01
#define ST7920_ENTRY_MODE_SHIFT_DIS	0x00

#define ST7920_DISP_ON_OFF_DISP_ON	0x04
#define ST7920_DISP_ON_OFF_CURS_ON	0x02
#define ST7920_DISP_ON_OFF_BLINK_ON	0x01

#define ST7920_CURS_DISP_SHIFT_RL	0x04
#define ST7920_CURS_DISP_SHIFT_SC	0x08

#define ST7920_FUNCTION_SET_8_BIT	0x10
#define ST7920_FUNCTION_SET_4_BIT	0x00
#define ST7920_FUNCTION_SET_EXT		0x04
#define ST7920_FUNCTION_SET_GRAPHIC	0x02

#define ST7920_SCRL_RAM_SEL_SCROLL	0x01
#define ST7920_SCRL_RAM_SEL_RAM		0x00

#define st7920_update_display() st7920_update_rows(0, ST7920_NUM_ROWS)

void st7920_init(void);
void st7920_send_command(UINT8 command);
void st7920_send_data(UINT8 val);
void st7920_set_pixel(UINT8 row, UINT8 col, UINT8 state);
void st7920_draw_text(UINT8 row, UINT8 text_col, char* str);
void st7920_scroll_up(UINT8 num_rows);
void st7920_clear_display(UINT8 fill);
void st7920_clear_rows(UINT8 start, UINT8 end, UINT8 fill);
void st7920_update_rows(UINT8 start, UINT8 end);

#endif
