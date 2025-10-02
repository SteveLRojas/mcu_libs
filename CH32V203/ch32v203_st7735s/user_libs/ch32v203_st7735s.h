/*
 * ch32v203_st7735s.h
 *
 *  Created on: Sep 15, 2025
 *      Author: dragomir
 */

#ifndef _CH32V203_ST7735S_H_
#define _CH32V203_ST7735S_H_

//HINT: Pin options. Use defines from GPIO and SPI libraries.
#define ST7735S_PORT_RST	GPIOA
#define ST7735S_PORT_DC		GPIOA
#define ST7735S_PORT_CS		GPIOA
#define ST7735S_PIN_RST		GPIO_PIN_0
#define ST7735S_PIN_DC		GPIO_PIN_1
#define ST7735S_PIN_CS		GPIO_PIN_4

//HINT: The maximum clock speed is 15 MHz.
#define ST7735S_SPI_CLK_DIV	SPI_CLK_DIV_8
#define ST7735S_SPI_MODULE	SPI1

#define ST7735S_NUM_COLUMNS	132
#define ST7735S_NUM_ROWS	162
//HINT: Leave the settings above set to the driver maximum, set the actual display area below
#define ST7735S_DA_COL_START	26
#define ST7735S_DA_COL_END		105
#define ST7735S_DA_ROW_START	1
#define ST7735S_DA_ROW_END		160
#define ST7735S_NUM_PIXELS	((1 + ST7735S_DA_COL_END - ST7735S_DA_COL_START) * (1 + ST7735S_DA_ROW_END - ST7735S_DA_ROW_START))

#define ST7735S_MLAYOUT_SWAP_ROWS	0x80
#define ST7735S_MLAYOUT_SWAP_COLS	0x40
#define ST7735S_MLAYOUT_EXCHANGE	0x20
#define ST7735S_MLAYOUT_BGR_ORDER	0x08

//HINT: Default settings
#define ST7735S_DEF_RTNA		0x01
#define ST7735S_DEF_FPA			0x2C
#define ST7735S_DEF_BPA			0x2D
#define ST7735S_DEF_RTNB		0x01
#define ST7735S_DEF_FPB			0x2C
#define ST7735S_DEF_BPB			0x2D
#define ST7735S_DEF_RTNC		0x01
#define ST7735S_DEF_FPC			0x2C
#define ST7735S_DEF_BPC			0x2D
#define ST7735S_DEF_RTND		0x01
#define ST7735S_DEF_FPD			0x2C
#define ST7735S_DEF_BPD			0x2D
#define ST7735S_DEF_INV_MODE	0x00
#define ST7735S_DEF_AVDD_VRHP	0xA2
#define ST7735S_DEF_VRHN		0x02
#define ST7735S_DEF_MODE		0x84
#define ST7735S_DEF_VGH_VGL		0xC5
#define ST7735S_DEF_SAPA_APA	0x0A
#define ST7735S_DEF_DCA			0x00
#define ST7735S_DEF_SAPB_APB	0x8A
#define ST7735S_DEF_DCB			0x2A
#define ST7735S_DEF_SAPC_APC	0x8A
#define ST7735S_DEF_DCC			0xEE
#define ST7735S_DEF_VCOMS		0x0E
#define ST7735S_DEF_MLAYOUT		0x00


#define ST7735S_COM_SLEEP_OUT	0x11
#define ST7735S_COM_NORON		0x13
#define ST7735S_COM_FRMCTR1		0xB1
#define ST7735S_COM_FRMCTR2		0xB2
#define ST7735S_COM_FRMCTR3		0xB3
#define ST7735S_COM_INVCTR		0xB4
#define ST7735S_COM_PWRCTR1		0xC0
#define ST7735S_COM_PWRCTR2		0xC1
#define ST7735S_COM_PWRCTR3		0xC2
#define ST7735S_COM_PWRCTR4		0xC3
#define ST7735S_COM_PWRCTR5		0xC4
#define ST7735S_COM_VMCTR1		0xC5
#define ST7735S_COM_INVOFF		0x20
#define ST7735S_COM_INVON		0x21
#define ST7735S_COM_GAMSET		0x26
#define ST7735S_COM_SCRLAR		0x33
#define ST7735S_COM_MADCTL		0x36
#define ST7735S_COM_VSCSAD		0x37
#define ST7735S_COM_COLMOD		0x3A
#define ST7735S_COM_CASET		0x2A
#define ST7735S_COM_RASET		0x2B
#define ST7735S_COM_RAMWR		0x2C
#define ST7735S_COM_GMCTRP1		0xE0
#define ST7735S_COM_GMCTRN1		0xE1
#define ST7735S_COM_DISPOFF		0x28
#define ST7735S_COM_DISPON		0x29

extern uint32_t fill_color;
extern uint32_t text_bg_color;
extern uint32_t text_fg_color;
extern const uint8_t st7735s_8x8_font[1024];

void st7735s_init(void);
void st7735s_send_command(uint8_t command);
void st7735s_send_data(uint8_t command);
void st7735s_set_pixel(uint8_t row, uint8_t col, uint32_t color);
uint8_t st7735s_draw_text(uint8_t row, uint8_t col, const char* str);
void st7735s_fill_display(void);
void st7735s_fill_rectangle(uint8_t start_row, uint8_t start_col, uint8_t end_row, uint8_t end_col);

#endif /* _CH32V203_ST7735S_H_ */
