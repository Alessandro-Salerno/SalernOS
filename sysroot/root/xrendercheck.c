#include <X11/Xlib.h>
#include <X11/extensions/Xrender.h>
#include <stdio.h>

int main(void) {
    Display *dpy;
    int      event_base;
    int      error_base;
    int      major;
    int      minor;

    dpy = XOpenDisplay(NULL);
    if (NULL == dpy) {
        printf("XOpenDisplay failed\n");
        return 1;
    }

    if (0 == XRenderQueryExtension(dpy, &event_base, &error_base)) {
        printf("render_ext = 0\n");
        XCloseDisplay(dpy);
        return 1;
    }

    printf("render_ext = 1\n");

    if (0 != XRenderQueryVersion(dpy, &major, &minor)) {
        printf("render_ver = %d.%d\n", major, minor);
    } else {
        printf("XRenderQueryVersion failed\n");
    }

    XCloseDisplay(dpy);
    return 0;
}
