#include <fontconfig/fontconfig.h>
#include <stdio.h>

#define FCD_FONT_NAME "DejaVu Sans:pixelsize=22"

int main(void) {
    FcPattern *pat;
    double     value;
    FcResult   res;

    if (FcTrue != FcInit()) {
        printf("FcInit failed\n");
        return 1;
    }

    printf("fc_version = %d\n", FcGetVersion());

    pat = FcPatternCreate();
    if (NULL == pat) {
        printf("FcPatternCreate failed\n");
        return 1;
    }

    FcPatternAddDouble(pat, FC_PIXEL_SIZE, 22.0);
    FcPatternAddDouble(pat, FC_DPI, 96.0);

    value = -1.0;
    res   = FcPatternGetDouble(pat, FC_PIXEL_SIZE, 0, &value);
    printf("add_pixelsize = %f (res %d)\n", value, res);

    value = -1.0;
    res   = FcPatternGetDouble(pat, FC_DPI, 0, &value);
    printf("add_dpi       = %f (res %d)\n", value, res);

    FcPatternDestroy(pat);

    pat = FcNameParse((const FcChar8 *)FCD_FONT_NAME);
    if (NULL == pat) {
        printf("FcNameParse failed\n");
        return 1;
    }

    value = -1.0;
    res   = FcPatternGetDouble(pat, FC_PIXEL_SIZE, 0, &value);
    printf("parsed_pixelsize = %f (res %d)\n", value, res);

    FcPatternPrint(pat);
    FcPatternDestroy(pat);

    return 0;
}
