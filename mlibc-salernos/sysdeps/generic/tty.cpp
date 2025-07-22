#include <asm/ioctl.h>
#include <asm/ioctls.h>
#include <mlibc/debug.hpp>
#include <stddef.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

int tcsetsid(int fd, pid_t pid) {
    return ioctl(fd, TIOCSCTTY, NULL);
}

int login_tty(int fd) {
    pid_t s;

    s = setsid();
    if (s == -1) {
        s = getsid(0);
    }
    if (tcsetsid(fd, s) == -1) {
        mlibc::infoLogger() << "tcsetsid: login_tty1 " << frg::endlog;
        return (-1);
    }
    mlibc::infoLogger() << "tcsetsid: login_tty2 " << frg::endlog;

    (void)dup2(fd, 0);
    (void)dup2(fd, 1);
    (void)dup2(fd, 2);
    if (fd > 2) {
        (void)close(fd);
    }
    return 0;
}
