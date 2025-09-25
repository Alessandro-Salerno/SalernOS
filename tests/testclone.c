#include <pthread.h>
#include <stdint.h>
#include <stdio.h>

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

void *entry_pthread_craete(void *arg) {
    (void)arg;
    printf("Entered thread with pthread_create (rip=%p, rsp=%p)\n",
           get_rip(),
           get_rsp());
    // pthread_exit(NULL);
    return NULL;
}

int main(void) {
    printf("Entered in main (rip=%p, rsp=%p)\n", get_rip(), get_rsp());
    pthread_t thread;

    if (0 != pthread_create(&thread, NULL, entry_pthread_craete, NULL)) {
        printf("ERROR: unable to launch thread with pthread_create\n");
        return -1;
    }

    if (0 != pthread_join(thread, NULL)) {
        printf("ERROR: unable to join threads\n");
        return -2;
    }

    printf("exited successfully\n");
    return 0;
}
