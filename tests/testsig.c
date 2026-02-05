// THIS CODE WAS GENERATD BY CHAT GPT

#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define NUM_CHILDREN 5
#define NUM_SIGNALS  500
#define TEST_SIGNAL  SIGUSR1

volatile sig_atomic_t signal_count = 0;

// Signal handler
void handler(int sig) {
    signal_count++;
}

double elapsed_time(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000 +
           (end.tv_nsec - start.tv_nsec) / 1e6;
}

int main() {
    struct sigaction sa;
    sigset_t         block_set, old_set, pending;
    pid_t            children[NUM_CHILDREN];

    pid_t root_pid = getpid();
    printf("root process is pid=%d\n", root_pid);

    // Setup signal handler for TEST_SIGNAL
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    sigaction(TEST_SIGNAL, &sa, NULL);

    // Block TEST_SIGNAL
    sigemptyset(&block_set);
    sigaddset(&block_set, TEST_SIGNAL);
    sigprocmask(SIG_BLOCK, &block_set, &old_set);

    // Fork children
    for (int i = 0; i < NUM_CHILDREN; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            exit(1);
        } else if (pid == 0) {
            // Child: send signals to parent rapidly
            pid_t ppid = getppid();
            if (root_pid != ppid) {
                fprintf(stderr,
                        "somehow child process pid=%d has ppid=%d\n",
                        getpid(),
                        ppid);
            }
            assert(root_pid == ppid);
            for (int j = 0; j < NUM_SIGNALS; j++) {
                kill(ppid, TEST_SIGNAL);
            }
            exit(0);
        } else {
            children[i] = pid;
        }
    }

    // Check pending signals
    sigpending(&pending);
    if (sigismember(&pending, TEST_SIGNAL)) {
        printf("Signal %d is pending as expected.\n", TEST_SIGNAL);
    } else {
        printf("Signal %d is NOT pending. Possible loss.\n", TEST_SIGNAL);
    }

    // Unblock the signal
    sigprocmask(SIG_SETMASK, &old_set, NULL);

    // Spin until we have received all expected signals
    int expected = NUM_CHILDREN * NUM_SIGNALS;
    int mills    = 0;
    while (signal_count < expected && mills < 5000) {
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        poll(NULL, 0, 100);
        for (int j = 0; j < 100000; j++)
            ;
        clock_gettime(CLOCK_MONOTONIC, &end);
        mills += elapsed_time(start, end);
    }

    printf("Received %d/%d signals.\n", signal_count, expected);

    return 0;
}
