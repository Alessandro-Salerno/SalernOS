#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <num iterations>\n", argv[0]);
        return -1;
    }
    int num_times = 0;
    if (1 != sscanf(argv[1], "%d", &num_times)) {
        fprintf(stderr, "ERROR: %s is not a valid number\n", argv[1]);
        return -2;
    }

    for (int i = 0; i < num_times; i++)
        ;
}
