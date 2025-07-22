#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <num_procs> <command> <args>\n", argv[0]);
        return -1;
    }

    int num_times = 0;
    sscanf(argv[1], "%d", &num_times);

    int *waitlist = malloc(sizeof(int) * num_times);

    if (NULL == waitlist) {
        fprintf(stderr, "ERROR: out of memory\n");
        return -2;
    }

    setpgid(0, 0);
    pid_t mpgid = getpgid(getpid());
    ioctl(0, 0x5410, &mpgid);

    for (int i = 0; i < num_times; i++) {
        int pid = fork();

        if (pid < 0) {
            fprintf(stderr, "ERROR: unable to fork at %d\n", i);
            num_times = i;
            break;
        }

        if (0 == pid) {
            if (0 != execv(argv[2], &argv[2])) {
                fprintf(stderr, "ERROR: unable to run program\n");
                exit(-3);
            }
        } else {
            waitlist[i] = pid;
        }
    }

    for (int i = 0; i < num_times; i++) {
        int status;
        waitpid(waitlist[i], &status, 0);
        fprintf(stderr,
                "==> process %d (pid = %d) exited with code %d\n",
                i,
                waitlist[i],
                status);
    }
}
