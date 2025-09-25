#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <threads.h>
#include <unistd.h>

struct init {
    int  sum;
    int  len;
    char first;
    char last;
};

char random_char(void) {
    static unsigned int seed  = 123456789; // You can vary this
    static bool         setup = false;

    if (!setup) {
        seed += getpid();
        setup = true;
    }

    seed                   = (1664525 * seed + 1013904223);
    unsigned int char_code = 32 + (seed % (126 - 32 + 1));
    return (char)char_code;
}

void consumer(int fd) {
    struct init initbuf;
    if (-1 == read(fd, &initbuf, sizeof(struct init))) {
        fprintf(stderr, "CONSUMER: unable to read init info\n");
        return;
    }

    char c;
    int  sum = 0;

    for (int i = 0; i < initbuf.len; i++) {
        if (-1 == read(fd, &c, 1)) {
            fprintf(stderr, "CONSUMER: error reading char %d\n", i);
            return;
        }

        if (0 == i && initbuf.first != c) {
            fprintf(stderr,
                    "CONSUMER: expected %c as first character, got %c\n",
                    initbuf.first,
                    c);
            return;
        }

        if (i == initbuf.len - 1 && initbuf.last != c) {
            fprintf(stderr,
                    "CONSUMER: expected %c as last character, got %c\n",
                    initbuf.last,
                    c);
            return;
        }

        sum += (int)c;
    }

    if (initbuf.sum != sum) {
        fprintf(stderr,
                "CONSUMER: expected sum = %d, got sum = %d\n",
                initbuf.sum,
                sum);
        return;
    }

    fprintf(stderr,
            "CONSUMER: received %d chars (sum=%d, first=%c, last=%c)...\tOk!\n",
            initbuf.len,
            initbuf.sum,
            initbuf.first,
            initbuf.last);
}

void producer(int fd, int block_sz) {
    struct init initbuf;
    initbuf.len   = block_sz;
    initbuf.first = random_char();
    initbuf.last  = random_char();
    initbuf.sum   = 0;

    char *chars = malloc(block_sz);

    if (NULL == chars) {
        fprintf(stderr, "PRODUCER: error while allocating memory\n");
        return;
    }

    chars[0]            = initbuf.first;
    chars[block_sz - 1] = initbuf.last;

    for (int i = 1; i < block_sz - 1; i++) {
        chars[i] = random_char();
        initbuf.sum += (int)chars[i];
    }

    initbuf.sum += (int)initbuf.first;
    initbuf.sum += (int)initbuf.last;

    if (-1 == write(fd, &initbuf, sizeof(struct init))) {
        fprintf(stderr, "PRODUCER: error while sending init struct\n");
        free(chars);
        return;
    }

    fprintf(stderr,
            "PRODUCER: sending %d chars (sum=%d, first=%c, last=%c)...\t",
            block_sz,
            initbuf.sum,
            initbuf.first,
            initbuf.last);
    if (-1 == write(fd, chars, block_sz)) {
        fprintf(stderr, "PRODUCER: error while sending characters\n");
        free(chars);
        return;
    }

    free(chars);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <bytes per loop>\n", argv[0]);
        return -1;
    }

    int block_sz;
    if (1 != sscanf(argv[1], "%d", &block_sz)) {
        fprintf(
            stderr, "ERROR: %s is not a valid value for block size\n", argv[1]);
        return -4;
    }

    int fildes[2];
    if (0 != pipe(fildes)) {
        fprintf(stderr, "ERROR: unable to open pipe\n");
        return -2;
    }

    int pid = fork();

    if (pid < 0) {
        fprintf(stderr, "ERROR: unable to fork\n");
        return -3;
    }

    if (0 == pid) {
        consumer(fildes[0]);
        exit(0);
    }

    producer(fildes[1], block_sz);
    int status;
    waitpid(pid, &status, 0);
    close(fildes[0]);
    close(fildes[1]);
    return 0;
}
