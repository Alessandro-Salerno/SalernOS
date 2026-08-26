#include <stdio.h>
#include <stdlib.h>

static double from_long_double_literal(void) {
    double size;

    size = 12.0L;
    return size;
}

int main(void) {
    long double ld;
    double      d;
    double      size;
    volatile double dpi   = 95.916;
    volatile double scale = 1.0;
    double      pixelsize;

    printf("sizeof long double = %d\n", (int)sizeof(long double));
    printf("sizeof double      = %d\n", (int)sizeof(double));

    size = from_long_double_literal();
    printf("size_from_12.0L    = %ld/1000\n", (long)(size * 1000.0));

    d = 12.0L;
    printf("direct 12.0L       = %ld/1000\n", (long)(d * 1000.0));

    d = 12.0;
    printf("direct 12.0        = %ld/1000\n", (long)(d * 1000.0));

    ld = 12.0L;
    d  = (double)ld;
    printf("via long double    = %ld/1000\n", (long)(d * 1000.0));

    ld = 22.5L;
    d  = (double)ld;
    printf("22.5L to double    = %ld/1000\n", (long)(d * 1000.0));

    size      = 12.0L;
    pixelsize = size * scale;
    pixelsize *= dpi / 72.0;
    printf("pixelsize          = %ld/1000\n", (long)(pixelsize * 1000.0));

    printf("strtold(\"12.0\")    = %ld/1000\n", (long)((double)strtold("12.0", NULL) * 1000.0));

    return 0;
}
