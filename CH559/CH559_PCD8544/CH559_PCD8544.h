#ifndef _CH559_PCD8544_H_
#define _CH559_PCD8544_H_

//HINT: Pin options. Use defines from GPIO and SPI libraries.
#define PCD8544_PORT_RST	GPIO_PORT_0
#define PCD8544_PORT_CE		GPIO_PORT_1
#define PCD8544_PORT_DC		GPIO_PORT_0
#define PCD8544_PIN_RST		GPIO_PIN_4
#define PCD8544_PIN_CE		GPIO_PIN_4
#define PCD8544_PIN_DC		GPIO_PIN_5

//HINT: The maximum clock speed is 4 MHz.
#define PCD8544_SPI_CLK_DIV	12
#define PCD8544_SPI_MODULE	SPI_0

//HINT: Default settings
#define PCD8544_DEF_CONTRAST	0x31
#define PCD8544_DEF_BIAS		0x03
#define PCD8544_DEF_T_COEF		0x00
#define PCD8544_DEF_COL_START	0x00
#define PCD8544_DEF_PAGE_START	0x00

#define PCD8544_NUM_COLUMNS	84
#define PCD8544_NUM_ROWS	48
#define PCD8544_BUF_SIZE	((PCD8544_NUM_ROWS + 7) / 8) * PCD8544_NUM_COLUMNS

#define PCD8544_COM_EXT_ISTR	0x21
#define PCD8544_COM_BAS_ISTR	0x20
#define PCD8544_COM_SET_VOP		0x80
#define PCD8544_COM_SET_BIAS	0x10
#define PCD8544_COM_SET_T_COEF	0x04
#define PCD8544_COM_SET_NORMAL	0x0C
#define PCD8544_COM_SET_INVERT	0x0D
#define PCD8544_COM_SET_COLUMN	0x80
#define PCD8544_COM_SET_PAGE	0x40

extern UINT8 pcd8544_frame_buf[PCD8544_BUF_SIZE];
extern UINT8 code pcd8544_8x8_font[1024];

void pcd8544_init(void);
void pcd8544_send_command(UINT8 command);
void pcd8544_send_data(UINT8 val);
void pcd8544_set_pixel(UINT8 row, UINT8 col, UINT8 state);
void pcd8544_draw_text(UINT8 line, UINT8 col, char* str);
void pcd8544_scroll_text_up(UINT8 num_lines);
void pcd8544_clear_display(UINT8 fill);
void pcd8544_clear_page(UINT8 page, UINT8 fill);
void pcd8544_update_display(void);
void pcd8544_update_page(UINT8 page);

#endif
