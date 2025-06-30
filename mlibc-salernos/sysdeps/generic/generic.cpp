#include <asm/ioctl.h>
#include <asm/ioctls.h>
#include <bits/ensure.h>
#include <cstdlib>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <poll.h>
#include <salernos/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>

#define STUB_ONLY                                   \
    {                                               \
        __ensure(!"STUB_ONLY function was called"); \
        __builtin_unreachable();                    \
    }

namespace mlibc {

void sys_libc_log(const char *message) {
    __syscall(__SALERNOS_SYSCALL_TEST, message);
}

void sys_libc_panic() {
    sys_libc_log("\nMLIBC PANIC\n");
    sys_exit(1);
    __builtin_unreachable();
}

void sys_exit(int code) {
    __syscall(__SALERNOS_SYSCALL_EXIT, code);
    __builtin_unreachable();
}

// SOME WEIRD GUARD HERE

#ifndef MLIBC_BUILDING_RTLD

int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written) {
    struct __syscall_ret ret =
        __syscall(__SALERNOS_SYSCALL_WRITE, fd, buf, count);

    if (ret.errno != 0) {
        return ret.errno;
    }

    *bytes_written = (ssize_t)ret.ret;
    return 0;
}

#endif

int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
    struct __syscall_ret ret =
        __syscall(__SALERNOS_SYSCALL_READ, fd, buf, count);

    if (ret.errno != 0) {
        return ret.errno;
    }

    *bytes_read = (ssize_t)ret.ret;
    return 0;
}

int sys_execve(const char *path, char *const argv[], char *const envp[]) {
    struct __syscall_ret ret =
        __syscall(__SALERNOS_SYSCALL_EXECVE, path, argv, envp);
    return ret.errno;
}

int sys_fork(pid_t *child) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_FORK);

    if (ret.errno != 0) {
        return ret.errno;
    }

    *child = (pid_t)ret.ret;
    return 0;
}

int sys_waitpid(pid_t          pid,
                int           *status,
                int            flags,
                struct rusage *ru,
                pid_t         *ret_pid) {
    if (ru) {
        mlibc::infoLogger()
            << "mlibc: struct reuse in sys_waitpid is unsupported"
            << frg::endlog;
        return ENOSYS;
    }

again:
    struct __syscall_ret ret =
        __syscall(__SALERNOS_SYSCALL_WAITPID, pid, status, flags);

    if (ret.errno != 0) {
        if (ret.errno == EINTR) {
            goto again;
        }

        return ret.errno;
    }

    *ret_pid = (pid_t)ret.ret;
    return 0;
}

#ifndef MLIBC_BUILDING_RTLD

int sys_ioctl(int fd, unsigned long request, void *arg, int *result) {
    struct __syscall_ret ret =
        __syscall(__SALERNOS_SYSCALL_IOCTL, fd, request, arg);

    if (0 != ret.errno) {
        return ret.errno;
    }

    *result = (int)ret.ret;
    return 0;
}

#endif

int sys_open(const char *path, int flags, mode_t mode, int *fd) {
    // TODO: have modes
    (void)mode;
    struct __syscall_ret ret =
        __syscall(__SALERNOS_SYSCALL_OPEN, path, strlen(path), flags);

    if (0 != ret.errno) {
        return ret.errno;
    }

    *fd = ret.ret;
    return 0;
}

int sys_vm_map(void  *hint,
               size_t size,
               int    prot,
               int    flags,
               int    fd,
               off_t  offset,
               void **window) {
    __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_MMAP,
                                  hint,
                                  size,
                                  (uint64_t)prot << 32 | (uint64_t)flags,
                                  fd,
                                  offset);
    if (0 != ret.errno) {
        return ret.errno;
    }

    *window = (void *)ret.ret;
    return 0;
}

int sys_anon_allocate(size_t size, void **pointer) {
    return sys_vm_map(NULL,
                      size,
                      PROT_EXEC | PROT_READ | PROT_WRITE,
                      MAP_ANONYMOUS,
                      -1,
                      0,
                      pointer);
}

int sys_tcb_set(void *pointer) {
    __syscall(__SALERNOS_SYSCALL_SET_TLS, pointer);
    return 0;
}

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
    struct __syscall_ret ret =
        __syscall(__SALERNOS_SYSCALL_SEEK, fd, offset, whence);

    if (0 != ret.errno) {
        return ret.errno;
    }

    *new_offset = ret.ret;
    return 0;
}

#ifndef MLIBC_BUILDING_RTLD

int sys_isatty(int fd) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_ISATTY, fd);
    return ret.errno;
}

int sys_tcgetattr(int fd, struct termios *attr) {
    int unused;
    return sys_ioctl(fd, TCGETS, attr, &unused);
}

int sys_tcsetattr(int fd, int optional_action, const struct termios *attr) {
    int unused;
    return sys_ioctl(fd, TCSETS, (void *)attr, &unused);
}

#endif

} // namespace mlibc
