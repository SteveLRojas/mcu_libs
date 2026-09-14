#ifndef _CH559_UC1701_H_
#define _CH559_UC1701_H_

//HINT: Pin options. Use defines from GPIO and SPI libraries.
#define UC1701_PORT_RST	GPIO_PORT_2
#define UC1701_PORT_CD	GPIO_PORT_2
#define UC1701_PORT_CS	GPIO_PORT_2
#define UC1701_PIN_RST 	GPIO_PIN_0
#define UC1701_PIN_CD	GPIO_PIN_2
#define UC1701_PIN_CS	GPIO_PIN_4

//HINT: The maximum clock speed is 33 MHz.
#define UC1701_SPI_CLK_DIV	48
#define UC1701_SPI_MODULE	SPI_1

#define UC1701_NUM_COLUMNS	128
#define UC1701_NUM_ROWS	64
#define UC1701_NUM_PAGES	((UC1701_NUM_ROWS + 7) / 8)
#define UC1701_BUF_SIZE	(UC1701_NUM_PAGES * UC1701_NUM_COLUMNS)

#define UC1701_COM_SET_COL_ADDR_L	0x00
#define UC1701_COM_SET_COL_ADDR_H	0x10
#define UC1701_COM_SET_POWER_CTRL	0x28
#define UC1701_COM_SET_SCROLL_LINE	0x40
#define UC1701_COM_SET_PAGE_ADDR	0xB0
#define UC1701_COM_SET_RES_RATIO	0x20
#define UC1701_COM_SET_ELEC_VOL		0x81
#define UC1701_COM_SET_ALL_PIX_ON	0xA4
#define UC1701_COM_SET_INVERT_DISP	0xA6
#define UC1701_COM_SET_DISP_ENABLE	0xAE
#define UC1701_COM_SET_SEG_DIR		0xA0
#define UC1701_COM_SET_COM_DIR		0xC0
#define UC1701_COM_SET_BIAS_RATIO	0xA2
#define UC1701_COM_SET_BOOST_RATIO	0xF8

#define UC1701_POWER_CTRL_BOOST		0x04
#define UC1701_POWER_CTRL_VREG		0x02
#define UC1701_POWER_CTRL_VFOLLOW	0x01
#define UC1701_ALL_PIX_ON			0x01
#define UC1701_ALL_PIX_OFF			0x00
#define UC1701_INVERT_DISP_ON		0x01
#define UC1701_INVERT_DISP_OFF		0x00
#define UC1701_DISP_ENABLE_ON		0x01
#define UC1701_DISP_ENABLE_OFF		0x00
#define UC1701_SEG_DIR_FORWARD		0x00
#define UC1701_SEG_DIR_REVERSE		0x01
#define UC1701_COM_DIR_FORWARD		0x00
#define UC1701_COM_DIR_REVERSE		0x08
#define UC1701_BIAS_RATIO_LOW		0x00
#define UC1701_BIAS_RATIO_HIGH		0x01

extern UINT8 uc1701_frame_buf[UC1701_BUF_SIZE];
extern UINT8 code uc1701_8x8_font[1024];

void uc1701_init(void);
void uc1701_send_command(UINT8 command);
void uc1701_send_command_2b(UINT8 command1, UINT8 command2);
void uc1701_send_data(UINT8 val);
void uc1701_set_pixel(UINT8 row, UINT8 col, UINT8 state);
void uc1701_draw_text(UINT8 line, UINT8 col, char* str);
void uc1701_scroll_text_up(UINT8 num_lines);
void uc1701_clear_display(UINT8 fill);
void uc1701_clear_page(UINT8 page, UINT8 fill);
void uc1701_update_display(void);
void uc1701_update_page(UINT8 page);

#endif

