#include <ft2build.h>
#include FT_FREETYPE_H
#include <stdio.h>

#define FTM_FONT_PATH "/usr/share/fonts/dejavu/DejaVuSans.ttf"
#define FTM_PIXEL_SIZE 22

int main(void) {
    FT_Library lib;
    FT_Face    face;
    FT_Error   err;

    err = FT_Init_FreeType(&lib);
    if (0 != err) {
        printf("FT_Init_FreeType failed: %d\n", err);
        return 1;
    }

    err = FT_New_Face(lib, FTM_FONT_PATH, 0, &face);
    if (0 != err) {
        printf("FT_New_Face failed: %d\n", err);
        return 1;
    }

    printf("family     = %s\n", face->family_name);
    printf("num_glyphs = %ld\n", face->num_glyphs);
    printf("num_fixed  = %d\n", face->num_fixed_sizes);
    printf("scalable   = %d\n", 0 != (FT_FACE_FLAG_SCALABLE & face->face_flags));
    printf("units_em   = %d\n", face->units_per_EM);
    printf("num_cmaps  = %d\n", face->num_charmaps);

    err = FT_Set_Pixel_Sizes(face, 0, FTM_PIXEL_SIZE);
    if (0 != err) {
        printf("FT_Set_Pixel_Sizes failed: %d\n", err);
        return 1;
    }

    printf("height     = %ld\n", face->size->metrics.height >> 6);
    printf("ascender   = %ld\n", face->size->metrics.ascender >> 6);
    printf("descender  = %ld\n", face->size->metrics.descender >> 6);
    printf("x_ppem     = %d\n", face->size->metrics.x_ppem);
    printf("y_ppem     = %d\n", face->size->metrics.y_ppem);

    err = FT_Load_Char(face, 'A', FT_LOAD_RENDER);
    if (0 != err) {
        printf("FT_Load_Char failed: %d\n", err);
        return 1;
    }

    printf("glyph_w    = %d\n", face->glyph->bitmap.width);
    printf("glyph_h    = %d\n", face->glyph->bitmap.rows);
    printf("advance_x  = %ld\n", face->glyph->advance.x >> 6);

    FT_Done_Face(face);
    FT_Done_FreeType(lib);
    return 0;
}
