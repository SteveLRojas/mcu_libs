/*
 * usb_hid_mouse.h
 *
 *  Created on: Jul 29, 2024
 *      Author: Steve
 */

#ifndef _USBD_HID_MOUSE_H_
#define _USBD_HID_MOUSE_H_

#define HID_ENDP0_SIZE 8
#define HID_ENDP1_SIZE 4

#define HID_MOUSE_BTN_LEFT 0x01
#define HID_MOUSE_BTN_RIGHT 0x02
#define HID_MOUSE_BTN_WHEEL 0x04

extern volatile uint8_t hid_report_pending;
extern volatile uint8_t hid_idle_rate;

void hid_init(void);
void hid_mouse_send_report(void);
void hid_mouse_press(uint8_t buttons);
void hid_mouse_release(uint8_t buttons);
void hid_mouse_move(uint8_t x_rel, uint8_t y_rel);
void hid_mouse_scroll(uint8_t scroll_rel);

#endif /* USER_LIBS_USB_HID_MOUSE_H_ */
