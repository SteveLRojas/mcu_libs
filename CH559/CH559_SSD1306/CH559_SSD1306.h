#ifndef _CH559_SSD1306_H_
#define _CH559_SSD1306_H_

//HINT: Defualt settings
#define SSD1306_DEF_OSC_FREQ_DIV	0x80
#define SSD1306_DEF_MUX_RATIO		0x2F
#define SSD1306_DEF_DISPLAY_OFFSET	0x00
#define SSD1306_DEF_START_LINE		0x00
#define SSD1306_DEF_ADDR_MODE		0x00
#define SSD1306_DEF_SEGMENT_REMAP	0x01
#define SSD1306_DEF_SCAN_DIRECTION	0x08
#define SSD1306_DEF_PINS_CONFIG		0x12
#define SSD1306_DEF_SET_CONTRAST	0x7F
#define SSD1306_DEF_PRE_PERIOD		0x22
#define SSD1306_DEF_COL_START_ADDR	0x20
#define SSD1306_DEF_COL_END_ADDR	0x5F
#define SSD1306_DEF_PAGE_START_ADDR	0x00
#define SSD1306_DEF_PAGE_END_ADDR	0x05

#define SSD1306_BUS_ADDR	0x78
#define SSD1306_NUM_COLUMNS	64
#define SSD1306_NUM_ROWS	48
#define SSD1306_BUF_SIZE	((SSD1306_NUM_ROWS + 7) / 8) * SSD1306_NUM_COLUMNS

#define SSD1306_CB_COMMAND	0x00
#define SSD1306_CB_DATA		0x40

#define SSD1306_COM_DISPLAY_OFF		0xAE
#define SSD1306_COM_DISPLAY_ON		0xAF
#define SSD1306_COM_SET_OSC_FREQ	0xD5
#define SSD1306_COM_SET_MUX_RATIO	0xA8
#define SSD1306_COM_SET_OFFSET		0xD3
#define SSD1306_COM_SET_START_LINE	0x40
#define SSD1306_COM_SET_ADDR_MODE	0x20
#define SSD1306_COM_SEGMENT_REMAP	0xA0
#define SSD1306_COM_SCAN_DIRECTION	0xC0
#define SSD1306_COM_PINS_CONFIG		0xDA
#define SSD1306_COM_SET_CONTRAST	0x81
#define SSD1306_COM_SET_PERIOD		0xD9
#define SSD1306_COM_ENTIRE_DISP_ON	0xA4
#define SSD1306_COM_SET_NORMAL_DISP	0xA6
#define SSD1306_COM_SET_INV_DISP	0xA7
#define SSD1306_COM_SET_CHARGE_PUMP	0x8D
#define SSD1306_COM_SET_COLUMN_ADDR	0x21
#define SSD1306_COM_SET_PAGE_ADDR	0x22

extern UINT8 ssd1306_frame_buf[SSD1306_BUF_SIZE];
extern UINT8 code ssd1306_8x8_font[1024];

void ssd1306_init(UINT8 sda_port_sel, UINT8 sda_pin_sel, UINT8 scl_port_sel, UINT8 scl_pin_sel);
void ssd1306_command_1_byte(UINT8 command);
void ssd1306_command_2_byte(UINT8 command, UINT8 arg);
void ssd1306_command_3_byte(UINT8 command, UINT8 arg1, UINT8 arg2);
void ssd1306_set_pixel(UINT8 row, UINT8 col, UINT8 state);
void ssd1306_draw_text(UINT8 line, UINT8 col, char* str);
void ssd1306_scroll_text_up(UINT8 num_lines);
void ssd1306_clear_display(UINT8 fill);
void ssd1306_clear_page(UINT8 page, UINT8 fill);
void ssd1306_update_display(void);
void ssd1306_update_page(UINT8 page);

#endif
