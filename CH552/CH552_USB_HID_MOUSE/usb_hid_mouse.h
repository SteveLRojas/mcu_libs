#ifndef _USB_HID_MOUSE_H_
#define _USB_HID_MOUSE_H_

#define HID_ENDP0_SIZE 8
#define HID_ENDP1_SIZE 4

// CH552 has a bug in the USB hardware: the buffer can be overflown by up to 2 bytes, but never past 64 bytes...
#define HID_ENDP0_BUF_SIZE ((HID_ENDP0_SIZE + 2) > 64) ? 64 : (HID_ENDP0_SIZE + 2)
#define HID_ENDP1_BUF_SIZE ((HID_ENDP1_SIZE + 2) > 64) ? 64 : (HID_ENDP1_SIZE + 2)

#define HID_MOUSE_BTN_LEFT 0x01
#define HID_MOUSE_BTN_RIGHT 0x02
#define HID_MOUSE_BTN_WHEEL 0x04

extern volatile UINT8 hid_report_pending;
extern volatile UINT8 hid_idle_rate;

void hid_init(void);
void hid_mouse_send_report(void);
void hid_mouse_press(UINT8 buttons);
void hid_mouse_release(UINT8 buttons);
void hid_mouse_move(UINT8 x_rel, UINT8 y_rel);
void hid_mouse_scroll(UINT8 scroll_rel);

#endif
