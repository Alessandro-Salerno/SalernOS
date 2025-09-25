#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

struct salernos_mouse_packet {
    void *reserved;
    int   buttons;
    int   dx;
    int   dy;
    int   dz;
};

int main(void) {
    int mouse_fd = open("/dev/mouse0", O_RDONLY);
    if (-1 == mouse_fd) {
        perror("open(\"/dev/mouse0\")");
        return -1;
    }

    struct salernos_mouse_packet pkt;
    while (read(mouse_fd, &pkt, sizeof(pkt))) {
        fprintf(stderr,
                "PACKET: buttons = %x, dx = %d, dy = %d, dz = %d\n",
                pkt.buttons,
                pkt.dx,
                pkt.dy,
                pkt.dz);
    }

    return 0;
}
