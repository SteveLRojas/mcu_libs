/*
 * usbfsd_hid_mouse.h
 *
 *  Created on: Aug 25, 2024
 *      Author: Steve
 */

#ifndef _USBFSD_HID_MOUSE_H_
#define _USBFSD_HID_MOUSE_H_

#define HID_ENDP0_SIZE 8
#define HID_ENDP1_SIZE 4

// For RX endpoints the HW writes the CRC after the data, up to 64 bytes. Up to 2 extra bytes may be needed.
#define HID_ENDP0_BUF_SIZE ((HID_ENDP0_SIZE + 2) > 64) ? 64 : (HID_ENDP0_SIZE + 2)

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

#endif /* _USBFSD_HID_MOUSE_H_ */
