#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

struct salernos_kbd_packet {
    void *reserved;
    int   keycode;
    int   keystate;
    char  c;
};

int main(void) {
    int kbd_fd = open("/dev/keyboard0", O_RDONLY);
    if (-1 == kbd_fd) {
        perror("open(\"/dev/keyboard0\")");
        return -1;
    }

    struct salernos_kbd_packet pkt;
    while (read(kbd_fd, &pkt, sizeof(pkt))) {
        fprintf(stderr,
                "PACKET: keycode=0x%x, keystate=%s, kchar=%c\n",
                pkt.keycode,
                (1 == pkt.keystate) ? "PRESSED" : "RELEASED",
                pkt.c);
        ;
    }

    return 0;
}
