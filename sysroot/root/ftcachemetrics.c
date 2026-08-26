#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H
#include <stdio.h>

#define FTC_FONT_PATH  "/usr/share/fonts/dejavu/DejaVuSans.ttf"
#define FTC_PIXEL_SIZE 22

static FT_Error ftc_face_requester(FTC_FaceID face_id,
                                   FT_Library lib,
                                   FT_Pointer data,
                                   FT_Face   *face) {
    (void)data;
    return FT_New_Face(lib, (const char *)face_id, 0, face);
}

int main(void) {
    FT_Library    lib;
    FTC_Manager   mgr;
    FTC_ScalerRec scaler;
    FT_Face       face;
    FT_Size       size;
    FT_Error      err;

    err = FT_Init_FreeType(&lib);
    if (0 != err) {
        printf("FT_Init_FreeType failed: %d\n", err);
        return 1;
    }

    err = FTC_Manager_New(lib, 0, 0, 0, ftc_face_requester, NULL, &mgr);
    if (0 != err) {
        printf("FTC_Manager_New failed: %d\n", err);
        return 1;
    }

    err = FTC_Manager_LookupFace(mgr, (FTC_FaceID)FTC_FONT_PATH, &face);
    if (0 != err) {
        printf("FTC_Manager_LookupFace failed: %d\n", err);
        return 1;
    }

    printf("family     = %s\n", face->family_name);
    printf("num_glyphs = %ld\n", face->num_glyphs);

    scaler.face_id = (FTC_FaceID)FTC_FONT_PATH;
    scaler.width   = 0;
    scaler.height  = FTC_PIXEL_SIZE;
    scaler.pixel   = 1;
    scaler.x_res   = 0;
    scaler.y_res   = 0;

    err = FTC_Manager_LookupSize(mgr, &scaler, &size);
    if (0 != err) {
        printf("FTC_Manager_LookupSize failed: %d\n", err);
        return 1;
    }

    printf("height     = %ld\n", size->metrics.height >> 6);
    printf("ascender   = %ld\n", size->metrics.ascender >> 6);
    printf("descender  = %ld\n", size->metrics.descender >> 6);
    printf("x_ppem     = %d\n", size->metrics.x_ppem);
    printf("y_ppem     = %d\n", size->metrics.y_ppem);

    FTC_Manager_Done(mgr);
    FT_Done_FreeType(lib);
    return 0;
}
