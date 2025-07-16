#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <num children>\n", argv[0]);
        return -1;
    }

    unsigned long num_children;
    if (1 != sscanf(argv[1], "%lu", &num_children)) {
        fprintf(stderr, "%s is not a valid number of children\n", argv[1]);
        return -2;
    }

    int    root_pid = getpid();
    pid_t *children = malloc(num_children * sizeof(pid_t));

    fprintf(stderr, "startin from root pid %d\n", root_pid);

    for (unsigned long i = 0; i < num_children; i++) {
        pid_t fork_ret = fork();

        if (fork_ret < 0) {
            fprintf(stderr, "fork() failed\n");
            return -3;
        }

        if (0 == fork_ret) {
            assert(root_pid == getppid());
            assert(root_pid != getpid());
            exit(0);
        }

        children[i] = fork_ret;
    }

    for (unsigned int i = 0; i < num_children; i++) {
        int status;
        waitpid(children[i], &status, 0);
    }
}
