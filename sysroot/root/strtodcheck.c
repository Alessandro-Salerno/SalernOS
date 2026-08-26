#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void check(const char *text) {
    char  *end;
    double val;

    end = NULL;
    val = strtod(text, &end);

    printf("strtod(\"%s\") = %ld/1000, consumed %d chars\n",
           text,
           (long)(val * 1000.0),
           (int)(NULL == end ? -1 : end - text));
}

int main(void) {
    char buf[64];

    check("22");
    check("22.0");
    check("22.5");
    check("0.5");
    check("96.0");
    check("1.0");
    check("12.0");

    printf("literal    = %ld/1000\n", (long)(22.5 * 1000.0));
    printf("atof(22.5) = %ld/1000\n", (long)(atof("22.5") * 1000.0));

    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "%f", 22.5);
    printf("snprintf %%f = \"%s\"\n", buf);

    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "%g", 22.5);
    printf("snprintf %%g = \"%s\"\n", buf);

    return 0;
}
