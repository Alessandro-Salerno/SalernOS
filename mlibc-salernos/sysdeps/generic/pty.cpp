
#include <asm/ioctls.h>
#include <errno.h>
#include <fcntl.h>
#include <mlibc/debug.hpp>
#include <pty.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

int openpty(int            *amaster,
            int            *aslave,
            char           *name,
            struct termios *termp,
            struct winsize *winp) {
    const char *slavename;
    int         master, slave;

    master = posix_openpt(O_RDWR | O_NOCTTY);
    if (master == -1)
        return (-1);

    if (grantpt(master) == -1)
        goto bad;

    if (unlockpt(master) == -1)
        goto bad;

    slavename = ptsname(master);
    if (slavename == NULL)
        goto bad;

    slave = open(slavename, O_RDWR);
    if (slave == -1)
        goto bad;

    *amaster = master;
    *aslave  = slave;

    if (name)
        strcpy(name, slavename);
    if (termp)
        tcsetattr(slave, TCSAFLUSH, termp);
    if (winp)
        ioctl(slave, TIOCSWINSZ, (char *)winp);

    return (0);

bad:
    close(master);
    return (-1);
}

// TODO: shouldnt be here
int getdtablesize(void) {
    return sysconf(_SC_OPEN_MAX);
}

int tcsetsid(int fd, pid_t pid) {
    // TODO:

    // 	if (pid != getsid(0)) {
    // 		errno = EINVAL;
    // 		return (-1);
    // 	}
    mlibc::infoLogger() << "tcsetsid: todo " << frg::endlog;

    return (ioctl(fd, TIOCSCTTY, NULL));
}

int login_tty(int fd) {
    pid_t s;

    s = setsid();
    if (s == -1)
        s = getsid(0);
    if (tcsetsid(fd, s) == -1) {
        mlibc::infoLogger() << "tcsetsid: login_tty1 " << frg::endlog;
        return (-1);
    }
    mlibc::infoLogger() << "tcsetsid: login_tty2 " << frg::endlog;

    (void)dup2(fd, 0);
    (void)dup2(fd, 1);
    (void)dup2(fd, 2);
    if (fd > 2)
        (void)close(fd);
    return (0);
}

int forkpty(int            *amaster,
            char           *name,
            struct termios *termp,
            struct winsize *winp) {
    int master, slave, pid;

    if (openpty(&master, &slave, name, termp, winp) == -1)
        return (-1);
    switch (pid = fork()) {
    case -1:
        (void)close(master);
        (void)close(slave);
        return (-1);
    case 0:
        /*
         * child
         */
        (void)close(master);
        login_tty(slave);
        return (0);
    }
    /*
     * parent
     */
    *amaster = master;
    (void)close(slave);
    return (pid);
}
