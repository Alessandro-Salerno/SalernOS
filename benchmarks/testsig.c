// THIS CODE WAS GENERATD BY CHAT GPT

#define _GNU_SOURCE
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_CHILDREN 5
#define NUM_SIGNALS  3

int test_signals[NUM_SIGNALS] = {SIGUSR1, SIGUSR2, SIGTERM};

// Signal handler
void signal_handler(int sig, siginfo_t *info, void *ucontext) {
    (void)ucontext;
    printf("[PID %d] Received signal %d from PID %d\n",
           getpid(),
           sig,
           info ? info->si_pid : -1);
}

// Set up signal handlers using sigaction
void setup_signal_handlers() {
    struct sigaction sa;
    sa.sa_sigaction = signal_handler;
    sa.sa_flags     = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    for (int i = 0; i < NUM_SIGNALS; ++i) {
        if (sigaction(test_signals[i], &sa, NULL) == -1) {
            perror("sigaction");
            exit(EXIT_FAILURE);
        }
    }
}

// Block test signals using sigprocmask
void block_test_signals(sigset_t *old_mask) {
    sigset_t new_mask;
    sigemptyset(&new_mask);
    for (int i = 0; i < NUM_SIGNALS; ++i) {
        sigaddset(&new_mask, test_signals[i]);
    }
    if (sigprocmask(SIG_BLOCK, &new_mask, old_mask) == -1) {
        perror("sigprocmask block");
        exit(EXIT_FAILURE);
    }
}

// Unblock signals
void unblock_test_signals(sigset_t *old_mask) {
    if (sigprocmask(SIG_SETMASK, old_mask, NULL) == -1) {
        perror("sigprocmask unblock");
        exit(EXIT_FAILURE);
    }
}

// Child: install handlers, signal readiness to parent, wait for signal
void child_process(int write_fd) {
    close(STDIN_FILENO);
    sigset_t old_mask;

    block_test_signals(&old_mask);
    setup_signal_handlers();
    unblock_test_signals(&old_mask);

    // Notify parent: ready
    if (write(write_fd, "R", 1) != 1) {
        perror("child write");
        exit(EXIT_FAILURE);
    }

    close(write_fd);

    // Wait indefinitely for signal
    pause();

    // Exit when signal is handled
    _exit(EXIT_SUCCESS);
}

// Parent sends all test signals to each child
void send_signals_to_children(pid_t *children) {
    for (int i = 0; i < NUM_CHILDREN; ++i) {
        for (int s = 0; s < NUM_SIGNALS; ++s) {
            if (kill(children[i], test_signals[s]) == -1) {
                fprintf(stderr,
                        "kill(%d, %d) failed: %s\n",
                        children[i],
                        test_signals[s],
                        strerror(errno));
            } else {
                printf("[Parent] Sent signal %d to child PID %d\n",
                       test_signals[s],
                       children[i]);
            }
        }
    }
}

int main() {
    pid_t children[NUM_CHILDREN];
    int   pipe_fds[NUM_CHILDREN][2];

    // Block signals during setup to avoid race conditions
    sigset_t old_mask;
    block_test_signals(&old_mask);

    for (int i = 0; i < NUM_CHILDREN; ++i) {
        if (pipe(pipe_fds[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            // Child
            close(pipe_fds[i][0]); // Close read end
            child_process(pipe_fds[i][1]);
        } else {
            // Parent
            close(pipe_fds[i][1]); // Close write end
            children[i] = pid;
        }
    }

    // Wait for all children to signal readiness
    for (int i = 0; i < NUM_CHILDREN; ++i) {
        char buf;
        if (read(pipe_fds[i][0], &buf, 1) != 1 || buf != 'R') {
            fprintf(stderr, "Failed to read readiness from child %d\n", i);
            exit(EXIT_FAILURE);
        }
        close(pipe_fds[i][0]);
    }

    // Unblock signals in parent (not needed here but good practice)
    unblock_test_signals(&old_mask);

    send_signals_to_children(children);

    // Wait for all children to exit
    for (int i = 0; i < NUM_CHILDREN; ++i) {
        int status;
        if (waitpid(children[i], &status, 0) == -1) {
            perror("waitpid");
        } else if (WIFEXITED(status)) {
            printf("[Parent] Child %d exited with status %d\n",
                   children[i],
                   WEXITSTATUS(status));
        }
    }

    printf("All children handled signals and exited.\n");
    return 0;
}
