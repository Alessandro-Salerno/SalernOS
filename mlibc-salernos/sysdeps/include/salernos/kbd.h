#ifndef _SALERNOS_KBD_H
#define _SALERNOS_KBD_H

// TAKEN: Mathewnd/Astral
#define KBD_KEY_RESERVED       0
#define KBD_KEY_ESCAPE         1
#define KBD_KEY_1              2
#define KBD_KEY_2              3
#define KBD_KEY_3              4
#define KBD_KEY_4              5
#define KBD_KEY_5              6
#define KBD_KEY_6              7
#define KBD_KEY_7              8
#define KBD_KEY_8              9
#define KBD_KEY_9              10
#define KBD_KEY_0              11
#define KBD_KEY_MINUS          12
#define KBD_KEY_EQUAL          13
#define KBD_KEY_BACKSPACE      14
#define KBD_KEY_TAB            15
#define KBD_KEY_Q              16
#define KBD_KEY_W              17
#define KBD_KEY_E              18
#define KBD_KEY_R              19
#define KBD_KEY_T              20
#define KBD_KEY_Y              21
#define KBD_KEY_U              22
#define KBD_KEY_I              23
#define KBD_KEY_O              24
#define KBD_KEY_P              25
#define KBD_KEY_LEFTBRACE      26
#define KBD_KEY_RIGHTBRACE     27
#define KBD_KEY_ENTER          28
#define KBD_KEY_LEFTCTRL       29
#define KBD_KEY_A              30
#define KBD_KEY_S              31
#define KBD_KEY_D              32
#define KBD_KEY_F              33
#define KBD_KEY_G              34
#define KBD_KEY_H              35
#define KBD_KEY_J              36
#define KBD_KEY_K              37
#define KBD_KEY_L              38
#define KBD_KEY_SEMICOLON      39
#define KBD_KEY_APOSTROPHE     40
#define KBD_KEY_GRAVE          41
#define KBD_KEY_LEFTSHIFT      42
#define KBD_KEY_BACKSLASH      43
#define KBD_KEY_Z              44
#define KBD_KEY_X              45
#define KBD_KEY_C              46
#define KBD_KEY_V              47
#define KBD_KEY_B              48
#define KBD_KEY_N              49
#define KBD_KEY_M              50
#define KBD_KEY_COMMA          51
#define KBD_KEY_DOT            52
#define KBD_KEY_SLASH          53
#define KBD_KEY_RIGHTSHIFT     54
#define KBD_KEY_KEYPADASTERISK 55
#define KBD_KEY_LEFTALT        56
#define KBD_KEY_SPACE          57
#define KBD_KEY_CAPSLOCK       58
#define KBD_KEY_F1             59
#define KBD_KEY_F2             60
#define KBD_KEY_F3             61
#define KBD_KEY_F4             62
#define KBD_KEY_F5             63
#define KBD_KEY_F6             64
#define KBD_KEY_F7             65
#define KBD_KEY_F8             66
#define KBD_KEY_F9             67
#define KBD_KEY_F10            68
#define KBD_KEY_NUMLOCK        69
#define KBD_KEY_SCROLLLOCK     70
#define KBD_KEY_KEYPAD7        71
#define KBD_KEY_KEYPAD8        72
#define KBD_KEY_KEYPAD9        73
#define KBD_KEY_KEYPADMINUS    74
#define KBD_KEY_KEYPAD4        75
#define KBD_KEY_KEYPAD5        76
#define KBD_KEY_KEYPAD6        77
#define KBD_KEY_KEYPADPLUS     78
#define KBD_KEY_KEYPAD1        79
#define KBD_KEY_KEYPAD2        80
#define KBD_KEY_KEYPAD3        81
#define KBD_KEY_KEYPAD0        82
#define KBD_KEY_KEYPADDOT      83
#define KBD_KEY_F11            87
#define KBD_KEY_F12            88
#define KBD_KEY_KEYPADENTER    96
#define KBD_KEY_RIGHTCTRL      97
#define KBD_KEY_KEYPADSLASH    98
#define KBD_KEY_SYSREQ         99
#define KBD_KEY_RIGHTALT       100
#define KBD_KEY_LINEFEED       101
#define KBD_KEY_HOME           102
#define KBD_KEY_UP             103
#define KBD_KEY_PAGEUP         104
#define KBD_KEY_LEFT           105
#define KBD_KEY_RIGHT          106
#define KBD_KEY_END            107
#define KBD_KEY_DOWN           108
#define KBD_KEY_PAGEDOWN       109
#define KBD_KEY_INSERT         110
#define KBD_KEY_DELETE         111

#define KBD_NUM_KEYS 111

#define KBD_KEYSTATE_PRESSED  1
#define KBD_KEYSTATE_RELEASED 2

#define KBD_GET_KEYCODE(pkt_ptr)  ((pkt_ptr)->keycode)
#define KBD_GET_KEYSTATE(pkt_ptr) ((pkt_ptr)->keystate)

#define KBD_IS_PRESSED(pkt_ptr, key) \
    ((pkt_ptr)->keycode == (key) && (pkt_ptr)->keystate == KBD_KEYSTATE_PRESSED)
#define KBD_IS_RELEASED(pkt_ptr, key) \
    ((pkt_ptr)->keycode == (key) &&   \
     (pkt_ptr)->keystate == KBD_KEYSTATE_RELEASED)

struct kbd_packet {
    void *rsvd1;
    int   keycode;
    int   keystate;
    char  rsvd2;
};

#endif
