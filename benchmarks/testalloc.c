#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <bytes>\n", argv[0]);
        return -1;
    }

    unsigned long bytes;
    if (1 != sscanf(argv[1], "%lu", &bytes)) {
        fprintf(stderr, "ERROR: %s is not a valid number\n", argv[1]);
        return -2;
    }

    void *buffer = malloc(bytes);
    if (NULL == buffer) {
        fprintf(stderr, "ERROR: malloc(%lu) returned NULL: ", bytes);
        switch (errno) {
        case ENOMEM:
            fprintf(stderr, "ENOMEM");
            break;
        default:
            fprintf(stderr, "(unknown errno)");
            break;
        }
        fprintf(stderr, "\n");
        return -3;
    }

    fprintf(stderr, "malloc(%lu) = %p\n", bytes, buffer);

    free(buffer);
    return 0;
}
