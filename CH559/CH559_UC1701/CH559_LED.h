#ifndef _CH559_LED_H_
#define _CH559_LED_H_

//HINT: The CH559 LED hardware simply shifts out data one bit at a time like a shift register, the software needs to pre-process the data if a different encoding is desired.
//This library is designed for LEDs such as WS2812, and will pre-process the data before writing it to the buffer.
//For WS2811 the order of the color channels may be wrong, as the encoder assumes GRB order.
//Set the encoder bit order and the peripheral bit order to match. Either order works.
#define LED_ENCODER_MSB_FIRST 0
#define LED_NUM_LEDS 3
#define LED_BUF_SIZE (LED_NUM_LEDS * 3 * 4 + 1)

#define LED_MODE_1_CHANNEL 0x00
#define LED_MODE_2_CHANNEL 0x40
#define LED_MODE_4_CHANNEL 0x80

#define LED_POL_NORMAL 0x00
#define LED_POL_INVERT 0x04

#define LED_ORDER_LSB_FIRST 0x00
#define LED_ORDER_MSB_FIRST 0x01

#define LED_CHANNEL_GREEN 0x00
#define LED_CHANNEL_RED 0x04
#define LED_CHANNEL_BLUE 0x08

extern UINT8 xdata led_buf[LED_BUF_SIZE];
extern volatile UINT8 led_transfering;

#define led_update() led_send_bytes(LED_BUF_SIZE)

void led_init(UINT8 clk_div, UINT8 led_mode);
void led_send_bytes(UINT8 num_bytes);
void led_set_color(UINT8 led, UINT8 red, UINT8 green, UINT8 blue);
void led_set_color_single(UINT8 led, UINT8 channel, UINT8 value);
UINT8 led_get_color_single(UINT8 led, UINT8 channel);

#endif