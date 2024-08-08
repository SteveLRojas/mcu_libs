#ifndef _USB_HID_CC_PAN_MOUSE_H_
#define _USB_HID_CC_PAN_MOUSE_H_

#define HID_ENDP0_SIZE 8
#define HID_ENDP1_SIZE 8
#define HID_ENDP2_SIZE 4

// CH552 has a bug in the USB hardware: the buffer can be overflown by up to 2 bytes, but never past 64 bytes...
#define HID_ENDP0_BUF_SIZE ((HID_ENDP0_SIZE + 2) > 64) ? 64 : (HID_ENDP0_SIZE + 2)
#define HID_ENDP1_BUF_SIZE ((HID_ENDP1_SIZE + 2) > 64) ? 64 : (HID_ENDP1_SIZE + 2)
#define HID_ENDP2_BUF_SIZE ((HID_ENDP2_SIZE + 2) > 64) ? 64 : (HID_ENDP2_SIZE + 2)

#define HID_MOUSE_BTN_LEFT 0x01
#define HID_MOUSE_BTN_RIGHT 0x02
#define HID_MOUSE_BTN_WHEEL 0x04

#define HID_CC_BTN_NONE          0x00

#define HID_CC_BTN_SYS_POWER     0x30
#define HID_CC_BTN_SYS_RESET     0x31
#define HID_CC_BTN_SYS_SLEEP     0x32

#define HID_CC_BTN_MENU          0x40
#define HID_CC_BTN_MENU_PICK     0x41
#define HID_CC_BTN_MENU_UP       0x42
#define HID_CC_BTN_MENU_DOWN     0x43
#define HID_CC_BTN_MENU_LEFT     0x44
#define HID_CC_BTN_MENU_RIGHT    0x45
#define HID_CC_BTN_MENU_ESCAPE   0x46
#define HID_CC_BTN_MENU_INCR     0x47
#define HID_CC_BTN_MENU_DECR     0x48

#define HID_CC_BTN_MEDIA_PLAY    0xB0
#define HID_CC_BTN_MEDIA_PAUSE   0xB1
#define HID_CC_BTN_MEDIA_RECORD  0xB2
#define HID_CC_BTN_MEDIA_FORWARD 0xB3
#define HID_CC_BTN_MEDIA_REWIND  0xB4
#define HID_CC_BTN_MEDIA_NEXT    0xB5
#define HID_CC_BTN_MEDIA_PREV    0xB6
#define HID_CC_BTN_MEDIA_STOP    0xB7
#define HID_CC_BTN_MEDIA_EJECT   0xB8
#define HID_CC_BTN_MEDIA_RANDOM  0xB9

#define HID_CC_BTN_VOL_MUTE      0xE2
#define HID_CC_BTN_VOL_UP        0xE9
#define HID_CC_BTN_VOL_DOWN      0xEA

extern volatile UINT8 hid_mouse_report_pending;
extern volatile UINT8 hid_mouse_idle_rate;
extern volatile UINT8 hid_cc_report_pending;
extern volatile UINT8 hid_cc_idle_rate;

void hid_init(void);

void hid_mouse_send_report(void);
void hid_mouse_press(UINT8 buttons);
void hid_mouse_release(UINT8 buttons);
void hid_mouse_move(UINT8 x_rel, UINT8 y_rel);
void hid_mouse_scroll(UINT8 v_rel, UINT8 h_rel);

void hid_cc_send_report(void);
void hid_cc_press(UINT16 key);

#endif
