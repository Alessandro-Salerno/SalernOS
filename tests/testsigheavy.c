#define _POSIX_C_SOURCE 200809L
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_THREADS  4
#define NUM_CHILDREN 2

volatile sig_atomic_t waiting_signal[NUM_THREADS] = {0};
volatile sig_atomic_t signal_flags[NUM_THREADS]   = {0};
volatile sig_atomic_t child_signal_received       = 0;
volatile int          siglock                     = 0;

void acquire(volatile int *lock) {
    while (1) {
        // this is before the spinning since hopefully the lock is uncontended
        if (!__atomic_exchange_n(lock, 1, __ATOMIC_ACQUIRE)) {
            return;
        }
        // spin with no ordering constraints
        while (__atomic_load_n(lock, __ATOMIC_RELAXED)) {
        }
    }
}

void release(volatile int *lock) {
    __atomic_store_n(lock, 0, __ATOMIC_RELEASE);
}

void signal_handler(int signo) {
    acquire(&siglock);
    fprintf(stderr, "[HANDLER] recevied signal %d\n", signo);
    int i;
    for (i = 0; i < NUM_THREADS; ++i) {
        if (waiting_signal[i] == signo) {
            signal_flags[i]   = signo;
            waiting_signal[i] = 0;
            break;
        }
    }
    child_signal_received = 1;
    release(&siglock);
    fprintf(stderr, "signal_flags[%d] = %d\n", i, signo);
}

void *thread_func(void *arg) {
    int      id = *(int *)arg;
    sigset_t blockset;
    sigemptyset(&blockset);
    sigaddset(&blockset, SIGUSR1);
    sigaddset(&blockset, SIGUSR2);

    // Block SIGUSR1 and SIGUSR2
    if (pthread_sigmask(SIG_BLOCK, &blockset, NULL) != 0) {
        perror("pthread_sigmask (block)");
        exit(1);
    }

    // Check pending signals (none expected)
    sigset_t pending;
    if (sigpending(&pending) != 0) {
        perror("sigpending");
        exit(1);
    }

    // Send signal to self (thread)
    int sig            = (id % 2 == 0) ? SIGUSR1 : SIGUSR2;
    waiting_signal[id] = sig;
    fprintf(stderr, "sending signal %d to thread %d\n", sig, id);
    if (pthread_kill(pthread_self(), sig) != 0) {
        perror("pthread_kill");
        exit(1);
    }

    // Unblock signal
    sigset_t unblock;
    sigemptyset(&unblock);
    sigaddset(&unblock, sig);
    if (pthread_sigmask(SIG_UNBLOCK, &unblock, NULL) != 0) {
        perror("pthread_sigmask (unblock)");
        exit(1);
    }

    // Wait for signal handler to set flag
    fprintf(stderr, "waiting for signal_flags[%d] to be %d\n", id, sig);
    while (signal_flags[id] != sig) {
        // Busy-wait (no sleep/pause allowed)
    }
    fprintf(stderr, "done waiting, signal_flags[%d] = %d\n", id, sig);

    return NULL;
}

void setup_sigaction(void (*handler)(int)) {
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction SIGUSR1");
        exit(1);
    }
    if (sigaction(SIGUSR2, &sa, NULL) == -1) {
        perror("sigaction SIGUSR2");
        exit(1);
    }
}

void child_process(int group) {
    if (setpgid(0, group) != 0) {
        perror("setpgid");
        _exit(1);
    }

    setup_sigaction(signal_handler);

    // Wait for signal to be received
    while (!child_signal_received) {
        // Busy-wait
    }

    _exit(0);
}

int main(void) {
    printf("[main] Setting up signal handlers in main process...\n");
    setup_sigaction(signal_handler);

    pthread_t threads[NUM_THREADS];
    int       thread_ids[NUM_THREADS];

    printf("[main] Creating %d threads...\n", NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]) !=
            0) {
            perror("pthread_create");
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            return 1;
        }
        if (signal_flags[i] != SIGUSR1 && signal_flags[i] != SIGUSR2) {
            fprintf(stderr,
                    "[main] Thread %d received incorrect signal: %d\n",
                    i,
                    signal_flags[i]);
            return 1;
        }
    }

    printf("[main] All threads completed successfully.\n");

    printf("[main] Blocking SIGUSR1 in main process using sigprocmask...\n");
    sigset_t blockset;
    sigemptyset(&blockset);
    sigaddset(&blockset, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &blockset, NULL) != 0) {
        perror("sigprocmask");
        return 1;
    }

    pid_t children[NUM_CHILDREN];
    pid_t child_pgids[NUM_CHILDREN];

    printf("[main] Forking %d child processes...\n", NUM_CHILDREN);
    child_signal_received = 0;
    int pgid              = 0;
    for (int i = 0; i < NUM_CHILDREN; ++i) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            return 1;
        } else if (pid == 0) {
            child_process(pgid);
        } else {
            if (0 == pgid) {
                pgid = pid;
            }
            children[i] = pid;
            pid_t pgid  = getpgid(pid);
            if (pgid == -1) {
                perror("getpgid");
                return 1;
            }
            child_pgids[i] = pgid;
        }
    }

    // Send SIGUSR1 to each child's process group
    printf("[main] Sending SIGUSR1 to child PGID...\n");
    if (kill(-pgid, SIGUSR1) != 0) {
        perror("kill to process group");
        return 1;
    }

    for (int i = 0; i < NUM_CHILDREN; ++i) {
        int status = 0;
        if (waitpid(children[i], &status, 0) == -1) {
            perror("waitpid");
            return 1;
        }
        /*if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            fprintf(stderr, "[main] Child %d exited abnormally\n", children[i]);
            return 1;
        }*/
    }

    printf("[main] All child processes exited successfully.\n");
    return 0;
}
