#include <bits/ensure.h>
#include <cstdlib>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>

#define STUB_ENOSYS                                              \
    {                                                            \
        mlibc::infoLogger()                                      \
            << "mlibc: " << __func__ << " is a stub! (ENOSYS)\n" \
            << frg::endlog;                                      \
        return ENOSYS;                                           \
    }

#define STUB_OK                                                              \
    {                                                                        \
        mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub! (OK)\n" \
                            << frg::endlog;                                  \
        return 0;                                                            \
    }

#define STUB_NULL                    \
    {                                \
        mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub! \
           (NULL)\n" << frg::endlog; \
        return NULL;                 \
    }

#define STUB_VOID                    \
    {                                \
        mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub! \
           (void)\n" << frg::endlog; \
        return;                      \
    }

#define STUB_NORETURN                               \
    {                                               \
        __ensure(!"STUB_ONLY function was called"); \
        __builtin_unreachable();                    \
    }

namespace mlibc {

#ifndef MLIBC_BUILDING_RTLD

int sys_link(const char *old_path, const char *new_path) {
    STUB_OK
}

int sys_linkat(int         olddirfd,
               const char *old_path,
               int         newdirfd,
               const char *new_path,
               int         flags) {
    STUB_OK
}

int sys_fchmodat(int fd, const char *pathname, mode_t mode, int flags) {
    STUB_OK
}

int sys_fchmod(int fd, mode_t mode) {
    STUB_OK
}

int sys_chmod(const char *pathname, mode_t mode) {
    return sys_fchmodat(AT_FDCWD, pathname, mode, 0);
}

int sys_fsync(int) {
    STUB_OK
}

#endif

int sys_vm_unmap(void *pointer, size_t size) {
    STUB_OK
}

int sys_vm_protect(void *pointer, size_t size, int prot) {
    STUB_OK
}

int sys_anon_free(void *pointer, size_t size) {
    return sys_vm_unmap(pointer, size);
}

#ifndef MLIBC_BUILDING_RTLD

uid_t sys_getuid(){STUB_OK}

uid_t sys_geteuid(){STUB_OK}

gid_t sys_getgid() {
    STUB_OK
}

int sys_setgid(gid_t gid){STUB_OK}

gid_t sys_getegid() {
    STUB_OK
}

int sys_ttyname(int fd, char *buf, size_t size) {
    STUB_ENOSYS
}

int sys_socketpair(int domain, int type_and_flags, int proto, int *fds) {
    STUB_ENOSYS
}

int sys_fchownat(int         dirfd,
                 const char *pathname,
                 uid_t       owner,
                 gid_t       group,
                 int         flags) {
    STUB_OK
}

int sys_getsockopt(int fd,
                   int layer,
                   int number,
                   void *__restrict buffer,
                   socklen_t *__restrict size) {
    (void)fd;
    (void)size;
    if (layer == SOL_SOCKET && number == SO_PEERCRED) {
        mlibc::infoLogger() << "\e[31mmlibc: getsockopt() call with SOL_SOCKET "
                               "and SO_PEERCRED is unimplemented\e[39m"
                            << frg::endlog;
        *(int *)buffer = 0;
        return 0;
    } else if (layer == SOL_SOCKET && number == SO_SNDBUF) {
        mlibc::infoLogger() << "\e[31mmlibc: getsockopt() call with SOL_SOCKET "
                               "and SO_SNDBUF is unimplemented\e[39m"
                            << frg::endlog;
        *(int *)buffer = 4096;
        return 0;
    } else if (layer == SOL_SOCKET && number == SO_TYPE) {
        mlibc::infoLogger()
            << "\e[31mmlibc: getsockopt() call with SOL_SOCKET and SO_TYPE is "
               "unimplemented, hardcoding SOCK_STREAM\e[39m"
            << frg::endlog;
        *(int *)buffer = SOCK_STREAM;
        return 0;
    } else if (layer == SOL_SOCKET && number == SO_ERROR) {
        mlibc::infoLogger()
            << "\e[31mmlibc: getsockopt() call with SOL_SOCKET and SO_ERROR is "
               "unimplemented, hardcoding 0\e[39m"
            << frg::endlog;
        *(int *)buffer = 0;
        return 0;
    } else if (layer == SOL_SOCKET && number == SO_KEEPALIVE) {
        mlibc::infoLogger()
            << "\e[31mmlibc: getsockopt() call with SOL_SOCKET and "
               "SO_KEEPALIVE is unimplemented, hardcoding 0\e[39m"
            << frg::endlog;
        *(int *)buffer = 0;
        return 0;
    } else {
        mlibc::panicLogger()
            << "\e[31mmlibc: Unexpected getsockopt() call, layer: " << layer
            << " number: " << number << "\e[39m" << frg::endlog;
        __builtin_unreachable();
    }

    return 0;
}

int sys_setsockopt(int         fd,
                   int         layer,
                   int         number,
                   const void *buffer,
                   socklen_t   size) {
    (void)fd;
    (void)buffer;
    (void)size;
    if (layer == SOL_SOCKET && number == SO_PASSCRED) {
        mlibc::infoLogger()
            << "\e[31mmlibc: setsockopt(SO_PASSCRED) is not implemented"
               " correctly\e[39m"
            << frg::endlog;
        return 0;
    } else if (layer == SOL_SOCKET && number == SO_ATTACH_FILTER) {
        mlibc::infoLogger()
            << "\e[31mmlibc: setsockopt(SO_ATTACH_FILTER) is not implemented"
               " correctly\e[39m"
            << frg::endlog;
        return 0;
    } else if (layer == SOL_SOCKET && number == SO_RCVBUFFORCE) {
        mlibc::infoLogger()
            << "\e[31mmlibc: setsockopt(SO_RCVBUFFORCE) is not implemented"
               " correctly\e[39m"
            << frg::endlog;
        return 0;
    } else if (layer == SOL_SOCKET && number == SO_SNDBUF) {
        mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with SOL_SOCKET "
                               "and SO_SNDBUF is unimplemented\e[39m"
                            << frg::endlog;
        return 0;
    } else if (layer == SOL_SOCKET && number == SO_KEEPALIVE) {
        mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with SOL_SOCKET "
                               "and SO_KEEPALIVE is unimplemented\e[39m"
                            << frg::endlog;
        return 0;
    } else if (layer == SOL_SOCKET && number == SO_REUSEADDR) {
        mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with SOL_SOCKET "
                               "and SO_REUSEADDR is unimplemented\e[39m"
                            << frg::endlog;
        return 0;
    } else if (layer == AF_NETLINK && number == SO_ACCEPTCONN) {
        mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with AF_NETLINK "
                               "and SO_ACCEPTCONN is unimplemented\e[39m"
                            << frg::endlog;
        return 0;
    } else {
        mlibc::panicLogger()
            << "\e[31mmlibc: Unexpected setsockopt() call, layer: " << layer
            << " number: " << number << "\e[39m" << frg::endlog;
        __builtin_unreachable();
    }
}

int sys_inotify_create(int flags, int *fd) {
    STUB_ENOSYS
}

extern "C" void __mlibc_restorer();

int sys_signalfd_create(sigset_t mask, int flags, int *fd) {
    STUB_ENOSYS
}

int sys_getgroups(size_t size, gid_t *list, int *_ret) {
    STUB_ENOSYS
}

int sys_mount(const char   *source,
              const char   *target,
              const char   *fstype,
              unsigned long flags,
              const void   *data) {
    STUB_ENOSYS
}

int sys_umount2(const char *target, int flags) {
    STUB_ENOSYS
}

int sys_gethostname(char *buffer, size_t bufsize) {
    strncpy(buffer, "salernos", bufsize);
    STUB_OK
}

int sys_sethostname(const char *buffer, size_t bufsize) {
    STUB_ENOSYS
}

int sys_getitimer(int, struct itimerval *) {
    STUB_ENOSYS
}

int sys_uname(struct utsname *buf) {
    if (!buf)
        return EFAULT;
    strcpy(buf->sysname, "salernos");
    strcpy(buf->nodename, "salernos");
    strcpy(buf->release, "0.2.4");
    strcpy(buf->version, "amd64");
    return 0;
}

int sys_umask(mode_t mode, mode_t *old) {
    STUB_OK
}

int sys_clock_getres(int clock, time_t *secs, long *nanos) {
    *secs  = 0;
    *nanos = 1;
    STUB_OK
}

#endif

} // namespace mlibc
