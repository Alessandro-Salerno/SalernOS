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
#include <stddef.h>
#include <stdint.h>
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
    (void)message;
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

int sys_ftruncate(int fd, size_t size) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_TRUNCATE, fd, size);

    if (0 != ret.errno) {
        return ret.errno;
    }

    return 0;
}

#ifndef MLIBC_BUILDING_RTLD

int sys_pipe(int *fds, int flags) {
    (void)flags;
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_PIPE, fds);

    if (0 != ret.errno) {
        return ret.errno;
    }

    return 0;
}

pid_t sys_getpid() {
    return __syscall(__SALERNOS_SYSCALL_GETPID).ret;
}

extern "C" void __mlibc_thread_entry();

int sys_clone(void *tcb, pid_t *pid_out, void *stack) {
    (void)tcb;

    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_CLONE,
                                         (uintptr_t)__mlibc_thread_entry,
                                         (uintptr_t)stack);

    if (0 != ret.errno) {
        return ret.errno;
    }

    *pid_out = (pid_t)ret.ret;
    return 0;
}

[[noreturn]] void sys_thread_exit() {
    __syscall(__SALERNOS_SYSCALL_EXIT_THREAD);
    __builtin_unreachable();
}

#endif

int sys_futex_wait(int *pointer, int expected, const struct timespec *time) {
    (void)time;

    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_FUTEX,
                                         pointer,
                                         0, /* FUTEX_WAIT */
                                         expected);
    if (0 != ret.errno) {
        return ret.errno;
    }

    return 0;
}

int sys_futex_wake(int *pointer) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_FUTEX,
                                         pointer,
                                         1, /* FUTEX_WAKE */
                                         INT_MAX);
    if (0 != ret.errno) {
        return ret.errno;
    }

    return 0;
}

#ifndef MLIBC_BUILDING_RTLD

pid_t sys_getppid() {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_GETPPID);
    return ret.ret;
}

int sys_getpgid(pid_t pid, pid_t *pgid) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_GETPGID, pid);
    if (0 != ret.errno) {
        return ret.errno;
    }

    *pgid = ret.ret;
    return 0;
}

int sys_getsid(pid_t pid, pid_t *sid) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_GETSID, pid);
    if (0 != ret.errno) {
        return ret.errno;
    }

    *sid = ret.ret;
    return 0;
}

int sys_setpgid(pid_t pid, pid_t pgid) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_SETPGID, pid, pgid);
    if (0 != ret.errno) {
        return ret.errno;
    }

    return 0;
}

int sys_setsid(pid_t *sid) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_SETSID);
    if (0 != ret.errno) {
        return ret.errno;
    }

    *sid = ret.ret;
    return 0;
}

int sys_sigprocmask(int how,
                    const sigset_t *__restrict set,
                    sigset_t *__restrict retrieve) {
    struct __syscall_ret ret =
        __syscall(__SALERNOS_SYSCALL_SIGPROCMASK, how, set, retrieve);

    if (0 != ret.errno) {
        return ret.errno;
    }

    return 0;
}

int sys_thread_sigmask(int how,
                       const sigset_t *__restrict set,
                       sigset_t *__restrict retrieve) {
    struct __syscall_ret ret =
        __syscall(__SALERNOS_SYSCALL_SIGTHREADMASK, how, set, retrieve);

    if (0 != ret.errno) {
        return ret.errno;
    }

    return 0;
}

int sys_sigpending(sigset_t *set) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_SIGPENDING, set);

    if (0 != ret.errno) {
        return ret.errno;
    }

    return 0;
}

extern "C" void __mlibc_restorer();

int sys_sigaction(int                     signum,
                  const struct sigaction *act,
                  struct sigaction       *oldact) {
    struct sigaction        new_action;
    const struct sigaction *call_action = act;

    if (NULL != act) {
        memcpy(&new_action, act, sizeof(struct sigaction));
        call_action = &new_action;
    }

    if (NULL != act && 0 == (new_action.sa_flags & SA_RESTORER)) {
        new_action.sa_restorer = __mlibc_restorer;
        new_action.sa_flags |= SA_RESTORER;
    }

    struct __syscall_ret ret =
        __syscall(__SALERNOS_SYSCALL_SIGACTION, signum, call_action, oldact);

    if (0 != ret.errno) {
        return ret.errno;
    }

    return 0;
}

#endif

int sys_kill(pid_t pid, int signal) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_KILL, pid, signal);

    if (0 != ret.errno) {
        return ret.errno;
    }

    return 0;
}

int sys_tgkill(int tgid, int tid, int sig) {
    if (tgid != sys_getpid()) {
        mlibc::infoLogger() << "tgkill(): kill_thread() currently does not "
                               "support different pid\n"
                            << frg::endlog;
        return ENOSYS;
    }

    struct __syscall_ret ret =
        __syscall(__SALERNOS_SYSCALL_KILL_THREAD, tid, sig);

    if (0 != ret.errno) {
        return ret.errno;
    }

    return 0;
}

} // namespace mlibc
