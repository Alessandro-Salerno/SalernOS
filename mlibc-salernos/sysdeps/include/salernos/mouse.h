#ifndef _SALERNOS_MOUSE_H
#define _SALERNOS_MOUSE_H

#define MOUSE_BUTTON_RIGHT 0x01
#define MOUSE_BUTTON_MID   0x02
#define MOUSE_BUTTON_LEFT  0x04
#define MOUSE_BUTTON_4     0x08
#define MOUSE_BUTTON_5     0x10

#define MOUSE_GET_BUTTONS(pkt_ptr)        ((pkt_ptr)->buttons)
#define MOUSE_ARE_PRESSED(pkt_ptr, bttns) ((pkt_ptr)->buttons & (bttns))

#define MOUSE_GET_DX(pkt_ptr) ((pkt_ptr)->dx)
#define MOUSE_GET_DY(pkt_ptr) ((pkt_ptr)->dy)
#define MOUSE_GET_DZ(pkt_ptr) ((pkt_ptr)->dz)

struct mouse_packet {
    void *rsvd;
    int   buttons;
    int   dx;
    int   dy;
    int   dz;
};

#endif
