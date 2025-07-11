#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct thread_args {
    int thread_num;
};

static inline void *get_rsp(void) {
    void *rsp;
    __asm__ volatile("mov %%rsp, %0" : "=r"(rsp));
    return rsp;
}

static inline void *get_rip(void) {
    void *rip;
    __asm__ volatile("leaq (%%rip), %0" : "=r"(rip));
    return rip;
}

static void *thread_entry(void *arg) {
    struct thread_args *real_arg = arg;
    printf("Entering and exiting new thread with num=%d, rsp=%p, arg=%p\n",
           real_arg->thread_num,
           get_rsp(),
           arg);
    return NULL;
}

int main(int argc, char *argv[]) {
    fprintf(stderr, "=======> STARTING TEST THREADS\n");

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

    pthread_t          *threads = calloc(sizeof(pthread_t), num_threads);
    struct thread_args *all_args =
        calloc(sizeof(struct thread_args), num_threads);
    if (NULL == threads || NULL == all_args) {
        fprintf(stderr, "ERROR: out of memory\n");
        return -3;
    }

    for (int i = 0; i < num_threads; i++) {
        all_args[i].thread_num = i;
        fprintf(stderr,
                "Creating thread %d with entry %p and arg %p\n",
                i,
                thread_entry,
                &all_args[i]);
        if (0 !=
            pthread_create(&threads[i], NULL, thread_entry, &all_args[i])) {
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
    free(all_args);
    fprintf(stderr, "=======> EXITING TEST THREADS\n");
    return 0;
}
