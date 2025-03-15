#ifndef _USB_HID_KEYBOARD_H_
#define _USB_HID_KEYBOARD_H_

#define HID_ENDP0_SIZE 8
#define HID_ENDP1_SIZE 8

// CH552 has a bug in the USB hardware: the buffer can be overflown by up to 2 bytes, but never past 64 bytes...
#define HID_ENDP0_BUF_SIZE ((HID_ENDP0_SIZE + 2) > 64) ? 64 : (HID_ENDP0_SIZE + 2)
#define HID_ENDP1_BUF_SIZE ((HID_ENDP1_SIZE + 2) > 64) ? 64 : (HID_ENDP1_SIZE + 2)

#define HID_KB_KEY_ROLL_OVER		0x01
#define HID_KB_KEY_POST_FAIL		0x02
#define HID_KB_KEY_ERROR_UNDEFINED	0x03
//HINT: refer to HID usage tables (HUT) section Keyboard/Keypad Page (0x07) for additional key codes.

#define HID_KB_MOD_LEFT_CONTROL		0x01
#define HID_KB_MOD_LEFT_SHIFT		0x02
#define HID_KB_MOD_LEFT_ALT			0x04
#define HID_KB_MOD_LEFT_GUI			0x08
#define HID_KB_MOD_RIGHT_CONTROL	0x10
#define HID_KB_MOD_RIGHT_SHIFT		0x20
#define HID_KB_MOD_RIGHT_ALT		0x40
#define HID_KB_MOD_RIGHT_GUI		0x80

#define HID_KB_LED_NUM_LOCK		0x01
#define HID_KB_LED_CAPS_LOCK	0x02
#define HID_KB_LED_SCROLL_LOCK	0x04
#define HID_KB_LED_COMPOSE		0x08
#define HID_KB_LED_KANA			0x10

extern volatile UINT8 hid_kb_report_pending;
extern volatile UINT8 hid_kb_idle_rate;
extern volatile UINT8 hid_kb_indicators;

void hid_kb_init(void);
void hid_kb_send_report(void);
void hid_kb_press_modifier(UINT8 modifiers);
void hid_kb_release_modifier(UINT8 modifiers);
void hid_kb_press_key(UINT8 key);
void hid_kb_release_key(UINT8 key);
void hid_kb_release_all_keys(void);

#endif