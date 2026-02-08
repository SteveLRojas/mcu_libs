#ifndef _CH552_ST7066_H_
#define _CH552_ST7066_H_

//HINT: RW is assumed to be always low
#define ST7066_PORT_RS	GPIO_PORT_3
#define ST7066_PORT_E	GPIO_PORT_1
#define ST7066_PIN_RS	GPIO_PIN_2
#define ST7066_PIN_E	GPIO_PIN_4

#define ST7066_SPI_CLK_DIV	12

#define ST7066_NUM_COLUMNS	16
#define ST7066_NUM_ROWS		2

//HINT: All timing parameters are in us
#define ST7066_E_PULSE_TIM		0
#define ST7066_FN_SET_INIT0_TIM	4100
#define ST7066_FN_SET_INIT1_TIM	100
#define ST7066_CLEAR_DISP_TIM	1520
#define ST7066_RETURN_HOME_TIM	1520
#define ST7066_WRITE_DATA_TIM	43
#define ST7066_DEFAULT_CMD_TIM	37

#define ST7066_COM_CLEAR_DISP		0x01
#define ST7066_COM_RETURN_HOME		0x02
#define ST7066_COM_ENTRY_MODE		0x04
#define ST7066_COM_DISP_ON_OFF		0x08
#define ST7066_COM_CURS_DISP_SHIFT	0x10
#define ST7066_COM_FUNCTION_SET		0x20
#define ST7066_COM_SET_CGRAM_ADDR	0x40
#define ST7066_COM_SET_DDRAM_ADDR	0x80

#define ST7066_ENTRY_MODE_INC		0x02
#define ST7066_ENTRY_MODE_DEC		0x00
#define ST7066_ENTRY_MODE_SHIFT_EN	0x01
#define ST7066_ENTRY_MODE_SHIFT_DIS	0x00

#define ST7066_DISP_ON_OFF_DISP_ON	0x04
#define ST7066_DISP_ON_OFF_CURS_ON	0x02
#define ST7066_DISP_ON_OFF_BLINK_ON	0x01

#define ST7066_CURS_DISP_SHIFT_RL	0x04
#define ST7066_CURS_DISP_SHIFT_SC	0x08

#define ST7066_FUNCTION_SET_8_BIT	0x10
#define ST7066_FUNCTION_SET_4_BIT	0x00
#define ST7066_FUNCTION_SET_2_LINE	0x08
#define ST7066_FUNCTION_SET_1_LINE	0x00
#define ST7066_FUNCTION_SET_11_PIX	0x04
#define ST7066_FUNCTION_SET_8_PIX	0x00

void st7066_init(void);
void st7066_send_command(UINT8 command);
void st7066_send_data(UINT8 val);
void st7066_set_cursor(UINT8 row, UINT8 col);
void st7066_write_char(char val);
void st7066_write_string(char* str);
void st7066_clear_display(void);
void st7066_clear_line(UINT8 row);

#endif
