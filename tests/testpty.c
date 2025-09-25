#define _XOPEN_SOURCE 600
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

int main() {
    int ptm_fd = posix_openpt(O_RDWR | O_NOCTTY);
    if (ptm_fd < 0) {
        perror("posix_openpt");
        return 1;
    }

    if (grantpt(ptm_fd) < 0 || unlockpt(ptm_fd) < 0) {
        perror("grantpt/unlockpt");
        close(ptm_fd);
        return 1;
    }

    char *pts_name = ptsname(ptm_fd);
    if (!pts_name) {
        perror("ptsname");
        close(ptm_fd);
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        close(ptm_fd);
        return 1;
    }

    if (pid == 0) {
        // Child process
        close(ptm_fd);

        printf("Opening pts: %s\n", pts_name);
        int pts_fd = open(pts_name, O_RDWR);
        if (pts_fd < 0) {
            perror("open pts");
            exit(1);
        }

        setsid(); // Create a new session
        if (ioctl(pts_fd, TIOCSCTTY, 0) < 0) {
            perror("ioctl TIOCSCTTY");
            exit(1);
        }

        // Redirect stdio to the slave PTY
        dup2(pts_fd, STDIN_FILENO);
        dup2(pts_fd, STDOUT_FILENO);
        dup2(pts_fd, STDERR_FILENO);
        if (pts_fd > STDERR_FILENO) {
            close(pts_fd);
        }

        // Write Hello World to the terminal
        printf("Hello from PTY child!\n");
        fflush(stdout);
    } else {
        // Parent process (master side)
        char    buf[256];
        ssize_t n;
        while ((n = read(ptm_fd, buf, sizeof(buf) - 1)) > 0) {
            buf[n] = '\0';
            printf("Parent received: %s", buf);
        }

        waitpid(pid, NULL, 0);
        close(ptm_fd);
    }

    return 0;
}
