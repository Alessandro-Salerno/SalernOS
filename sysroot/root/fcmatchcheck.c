#include <fontconfig/fontconfig.h>
#include <stdio.h>

#define FCM_FONT_NAME "DejaVu Sans:pixelsize=22"

static void show(const char *label, FcPattern *pat) {
    double   val;
    FcResult res;

    val = -1.0;
    res = FcPatternGetDouble(pat, FC_PIXEL_SIZE, 0, &val);
    printf("%s pixelsize = %ld/1000 (res %d)\n", label, (long)(val * 1000.0), res);

    val = -1.0;
    res = FcPatternGetDouble(pat, FC_SIZE, 0, &val);
    printf("%s size      = %ld/1000 (res %d)\n", label, (long)(val * 1000.0), res);

    val = -1.0;
    res = FcPatternGetDouble(pat, FC_SCALE, 0, &val);
    printf("%s scale     = %ld/1000 (res %d)\n", label, (long)(val * 1000.0), res);

    val = -1.0;
    res = FcPatternGetDouble(pat, FC_DPI, 0, &val);
    printf("%s dpi       = %ld/1000 (res %d)\n", label, (long)(val * 1000.0), res);
}

int main(void) {
    FcPattern *pat;
    FcPattern *match;
    FcResult   res;

    if (FcTrue != FcInit()) {
        printf("FcInit failed\n");
        return 1;
    }

    pat = FcNameParse((const FcChar8 *)FCM_FONT_NAME);
    if (NULL == pat) {
        printf("FcNameParse failed\n");
        return 1;
    }

    show("parsed   ", pat);

    if (FcTrue != FcConfigSubstitute(NULL, pat, FcMatchPattern)) {
        printf("FcConfigSubstitute failed\n");
        return 1;
    }

    show("substd   ", pat);

    FcDefaultSubstitute(pat);
    show("defaulted", pat);

    res   = FcResultMatch;
    match = FcFontMatch(NULL, pat, &res);
    if (NULL == match) {
        printf("FcFontMatch failed: %d\n", res);
        return 1;
    }

    show("matched  ", match);

    FcPatternDestroy(match);
    FcPatternDestroy(pat);
    return 0;
}
