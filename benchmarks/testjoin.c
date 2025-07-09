#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct thread_args {
    int thread_num;
};

static int sum = 0;

static void *thread_entry(void *arg) {
    (void)arg;
    __atomic_add_fetch(&sum, 1, __ATOMIC_SEQ_CST);
    return NULL;
}

int main(int argc, char *argv[]) {
    fprintf(stderr, "=======> STARTING TEST JOIN\n");

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <num threds>\n", argv[0]);
        return -1;
    }

    int num_threads;
    if (1 != sscanf(argv[1], "%d", &num_threads)) {
        fprintf(
            stderr, "ERROR: %s is not a valid number of threads\n", argv[1]);
        return -2;
    }

    pthread_t *threads = calloc(sizeof(pthread_t), num_threads);
    if (NULL == threads) {
        fprintf(stderr, "ERROR: out of memory\n");
        return -3;
    }

    for (int i = 0; i < num_threads; i++) {
        if (0 != pthread_create(&threads[i], NULL, thread_entry, NULL)) {
            fprintf(stderr, "ERROR: unable to create thread %d\n", i);
            num_threads = i;
            break;
        }
    }

    for (int i = 0; i < num_threads; i++) {
        if (0 != pthread_join(threads[i], NULL)) {
            fprintf(stderr, "ERROR: unable to join thread %d", i);
            return -4;
        }
    }

    free(threads);
    fprintf(stderr, "Result: %d\n", sum);
    fprintf(stderr, "=======> EXITING TEST JOIN\n");
    return sum;
}
