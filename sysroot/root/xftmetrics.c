#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include <X11/extensions/Xrender.h>
#include <fontconfig/fontconfig.h>
#include <stdio.h>

#define XFTM_FONT_NAME "DejaVu Sans:pixelsize=22"

int main(void) {
    Display   *dpy;
    int        screen;
    int        event_base;
    int        error_base;
    int        major;
    int        minor;
    FcPattern *pat;
    FcPattern *match;
    FcResult   result;
    XftFont   *font;

    dpy = XOpenDisplay(NULL);
    if (NULL == dpy) {
        printf("XOpenDisplay failed\n");
        return 1;
    }

    screen = DefaultScreen(dpy);

    printf("render_ext  = %d\n", XRenderQueryExtension(dpy, &event_base, &error_base));
    if (0 != XRenderQueryVersion(dpy, &major, &minor)) {
        printf("render_ver  = %d.%d\n", major, minor);
    }

    printf("xft_version = %d\n", XftVersion);
    printf("fc_version  = %d\n", FcGetVersion());
    printf("has_render  = %d\n", XftDefaultHasRender(dpy));

    pat = FcNameParse((const FcChar8 *)XFTM_FONT_NAME);
    if (NULL == pat) {
        printf("FcNameParse failed\n");
        return 1;
    }

    XftDefaultSubstitute(dpy, screen, pat);
    FcDefaultSubstitute(pat);

    result = FcResultMatch;
    match  = FcFontMatch(NULL, pat, &result);
    if (NULL == match) {
        printf("FcFontMatch failed: %d\n", result);
        return 1;
    }

    FcPatternPrint(match);

    font = XftFontOpenPattern(dpy, match);
    if (NULL == font) {
        printf("XftFontOpenPattern failed\n");
    } else {
        printf("pattern_height  = %d\n", font->height);
        printf("pattern_ascent  = %d\n", font->ascent);
        printf("pattern_descent = %d\n", font->descent);
        printf("pattern_maxadv  = %d\n", font->max_advance_width);
    }

    font = XftFontOpenName(dpy, screen, XFTM_FONT_NAME);
    if (NULL == font) {
        printf("XftFontOpenName failed\n");
        return 1;
    }

    printf("name_height  = %d\n", font->height);
    printf("name_ascent  = %d\n", font->ascent);
    printf("name_descent = %d\n", font->descent);
    printf("name_maxadv  = %d\n", font->max_advance_width);

    XCloseDisplay(dpy);
    return 0;
}
