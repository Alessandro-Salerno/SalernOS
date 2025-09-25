#include <fcntl.h>
#include <linux/fb.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
    int fb_fd = open("/dev/fb0", O_RDWR);
    if (fb_fd == -1) {
        perror("Error opening /dev/fb0");
        return 1;
    }

    // Get fixed screen information
    struct fb_fix_screeninfo finfo;
    if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        close(fb_fd);
        return 1;
    }

    // Get variable screen information
    struct fb_var_screeninfo vinfo;
    if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        close(fb_fd);
        return 1;
    }

    // Calculate screen size in bytes
    long screensize = vinfo.yres_virtual * finfo.line_length;

    // Map the framebuffer to memory
    uint8_t *fbp = (uint8_t *)mmap(
        0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
    if ((intptr_t)fbp == -1) {
        perror("Error mapping framebuffer device to memory");
        close(fb_fd);
        return 1;
    }

    printf("Framebuffer mapped. Resolution: %dx%d, %dbpp\n",
           vinfo.xres,
           vinfo.yres,
           vinfo.bits_per_pixel);

    // Draw a simple rectangle (100x100 px) at (50, 50)
    int x, y;
    int rect_width  = 100;
    int rect_height = 100;
    int start_x     = 50;
    int start_y     = 50;

    for (y = start_y; y < start_y + rect_height; y++) {
        for (x = start_x; x < start_x + rect_width; x++) {
            long location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) +
                            (y + vinfo.yoffset) * finfo.line_length;

            if (vinfo.bits_per_pixel == 32) {
                // RGBA format: 8 bits each
                fbp[location + 0] = 0x00; // Blue
                fbp[location + 1] = 0x00; // Green
                fbp[location + 2] = 0xFF; // Red
                fbp[location + 3] = 0x00; // Transparency (if used)
            } else if (vinfo.bits_per_pixel == 16) {
                // RGB565 format
                uint16_t color = (31 << 11) | (0 << 5) | (0); // Red
                *((uint16_t *)(fbp + location)) = color;
            }
        }
    }

    printf("Rectangle drawn.\n");

    while (1) {
        poll(NULL, 0, 1000);
    }

    return 0;
}
