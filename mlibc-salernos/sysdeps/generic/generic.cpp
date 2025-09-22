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
#include <sys/uio.h>
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
    struct iovec iov;
    iov.iov_base = (void *)buf;
    iov.iov_len  = count;

    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_WRITEV,
                                         fd,
                                         &iov,
                                         1);

    if (ret.errno != 0) {
        return ret.errno;
    }

    *bytes_written = (ssize_t)ret.ret;
    return 0;
}

#endif

int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
    struct iovec iov;
    iov.iov_base             = (void *)buf;
    iov.iov_len              = count;
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_READV, fd, &iov, 1);

    if (ret.errno != 0) {
        return ret.errno;
    }

    *bytes_read = (ssize_t)ret.ret;
    return 0;
}

int sys_execve(const char *path, char *const argv[], char *const envp[]) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_EXECVE,
                                         path,
                                         argv,
                                         envp);
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
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_WAITPID,
                                         pid,
                                         status,
                                         flags);

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
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_IOCTL,
                                         fd,
                                         request,
                                         arg);

    if (0 != ret.errno) {
        return ret.errno;
    }

    *result = (int)ret.ret;
    return 0;
}

int sys_stat(fsfd_target  fsfdt,
             int          fd,
             const char  *path,
             int          flags,
             struct stat *statbuf) {
    struct __syscall_ret ret;

    switch (fsfdt) {
        case fsfd_target::fd: {
            ret = __syscall(__SALERNOS_SYSCALL_FSTATAT,
                            fd,
                            path,
                            statbuf,
                            flags | AT_EMPTY_PATH);
            break;
        }
        case fsfd_target::path: {
            ret = __syscall(__SALERNOS_SYSCALL_FSTATAT,
                            AT_FDCWD,
                            path,
                            statbuf,
                            flags);
            break;
        }
        case fsfd_target::fd_path: {
            ret = __syscall(__SALERNOS_SYSCALL_FSTATAT,
                            fd,
                            path,
                            statbuf,
                            flags);
            break;
        }
        default: {
            __ensure(!"stat: Invalid fsfdt");
            __builtin_unreachable();
        }
    }
    if (ret.errno != 0)
        return ret.errno;
    return ret.ret;
}

#endif

int sys_openat(int dirfd, const char *path, int flags, mode_t mode, int *fd) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_OPENAT,
                                         dirfd,
                                         path,
                                         flags,
                                         mode);

    if (0 != ret.errno) {
        return ret.errno;
    }

    *fd = (int)ret.ret;
    return 0;
}

int sys_open(const char *path, int flags, mode_t mode, int *fd) {
    return sys_openat(AT_FDCWD, path, flags, mode, fd);
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
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_SEEK,
                                         fd,
                                         offset,
                                         whence);

    if (0 != ret.errno) {
        return ret.errno;
    }

    *new_offset = ret.ret;
    return 0;
}

#ifndef MLIBC_BUILDING_RTLD

int sys_isatty(int fd) {
    struct winsize ws;
    int            ret;
    int            err = 0;

    if (!(err = sys_ioctl(fd, TIOCGWINSZ, &ws, &ret)))
        return 0;

    mlibc::infoLogger() << "isatty(" << fd << ") failed with errno " << err
                        << "\n"
                        << frg::endlog;
    return ENOTTY;
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
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_SIGPROCMASK,
                                         how,
                                         set,
                                         retrieve);

    if (0 != ret.errno) {
        return ret.errno;
    }

    return 0;
}

int sys_thread_sigmask(int how,
                       const sigset_t *__restrict set,
                       sigset_t *__restrict retrieve) {
    // NOTE: __SALERNOS_SYSCALL_SIGTHREADMASK is now deprecated (kernel 0.2.4
    // indev)
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_SIGPROCMASK,
                                         how,
                                         set,
                                         retrieve);

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

    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_SIGACTION,
                                         signum,
                                         call_action,
                                         oldact);

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

    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_KILL_THREAD,
                                         tid,
                                         sig);

    if (0 != ret.errno) {
        return ret.errno;
    }

    return 0;
}

#ifndef MLIBC_BUILDING_RTLD

int sys_dup2(int fd, int flags, int newfd) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_DUP3,
                                         fd,
                                         newfd,
                                         flags);

    if (0 != ret.errno) {
        return ret.errno;
    }

    return 0;
}

int sys_dup(int fd, int flags, int *newfd) {
    (void)flags;
    __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_FCNTL, fd, F_DUPFD, 0);

    if (0 != ret.errno) {
        return ret.errno;
    }

    *newfd = (ssize_t)ret.ret;
    return 0;
}

int sys_getcwd(char *buffer, size_t size) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_GETCWD,
                                         buffer,
                                         size);

    if (0 != ret.errno) {
        return ret.errno;
    }

    return 0;
}

int sys_fcntl(int fd, int request, va_list args, int *result) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_FCNTL,
                                         fd,
                                         request,
                                         va_arg(args, uint64_t));

    if (0 != ret.errno) {
        return ret.errno;
    }

    *result = (ssize_t)ret.ret;
    return 0;
}

#endif

int sys_close(int fd) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_CLOSE, fd);

    if (0 != ret.errno) {
        return ret.errno;
    }

    return 0;
}

#ifndef MLIBC_BUILDING_RTLD

typedef struct {
    ino_t          d_ino;
    off_t          d_off;
    unsigned short d_reclen;
    unsigned char  d_type;
    char           d_name[1024];
} dent_t;

int sys_open_dir(const char *path, int *handle) {
    return sys_open(path, O_DIRECTORY, 0, handle);
}

int sys_read_entries(int     fd,
                     void   *buffer,
                     size_t  max_size,
                     size_t *bytes_read) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_READDIR,
                                         fd,
                                         buffer,
                                         max_size);

    if (0 != ret.errno) {
        return ret.errno;
    }

    *bytes_read = ret.ret;
    return 0;
}

int sys_chdir(const char *path) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_CHDIR, path);

    if (0 != ret.errno) {
        return ret.errno;
    }

    return 0;
}

int sys_faccessat(int dirfd, const char *pathname, int mode, int flags) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_FACCESSAT,
                                         dirfd,
                                         pathname,
                                         mode,
                                         flags);

    if (0 != ret.errno) {
        return ret.errno;
    }

    return 0;
}

int sys_access(const char *path, int mode) {
    return sys_faccessat(AT_FDCWD, path, mode, 0);
}

#endif

int sys_clock_get(int clock, time_t *secs, long *nanos) {
    struct timespec      ts;
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_CLOCK_GET,
                                         clock,
                                         &ts);

    if (0 != ret.errno) {
        return ret.errno;
    }

    *secs  = ts.tv_sec;
    *nanos = ts.tv_nsec;

    return 0;
}

#ifndef MLIBC_BUILDING_RTLD

int sys_ppoll(struct pollfd         *fds,
              int                    nfds,
              const struct timespec *timeout,
              const sigset_t        *sigmask,
              int                   *num_events) {
    __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_PPOLL, fds, nfds, timeout);

    if (ret.errno != 0) {
        return ret.errno;
    }

    *num_events = (int)ret.ret;
    return 0;
}

int sys_poll(struct pollfd *fds, nfds_t count, int timeout, int *num_events) {
    struct timespec ts;
    ts.tv_sec  = timeout / 1000;
    ts.tv_nsec = (timeout % 1000) * 1000000;
    return sys_ppoll(fds, count, timeout < 0 ? NULL : &ts, NULL, num_events);
}

// TAKEN: vloxei64/ke
int sys_pselect(int                    nfds,
                fd_set                *read_set,
                fd_set                *write_set,
                fd_set                *except_set,
                const struct timespec *timeout,
                const sigset_t        *sigmask,
                int                   *num_events) {
    struct pollfd *fds = (struct pollfd *)calloc(nfds, sizeof(struct pollfd));
    if (fds == NULL) {
        return ENOMEM;
    }

    for (int i = 0; i < nfds; i++) {
        struct pollfd *fd = &fds[i];

        if (read_set && FD_ISSET(i, read_set)) {
            fd->events |= POLLIN;
        }
        if (write_set && FD_ISSET(i, write_set)) {
            fd->events |= POLLOUT;
        }
        if (except_set && FD_ISSET(i, except_set)) {
            fd->events |= POLLPRI;
        }

        if (!fd->events) {
            fd->fd = -1;
            continue;
        }
        fd->fd = i;
    }

    int ret = sys_ppoll(fds, nfds, timeout, sigmask, num_events);
    if (ret != 0) {
        free(fds);
        return ret;
    }

    fd_set res_read_set, res_write_set, res_except_set;
    FD_ZERO(&res_read_set);
    FD_ZERO(&res_write_set);
    FD_ZERO(&res_except_set);

    for (int i = 0; i < nfds; i++) {
        struct pollfd *fd = &fds[i];

        if (read_set && FD_ISSET(i, read_set) &&
            (fd->revents & (POLLIN | POLLERR | POLLHUP)) != 0) {
            FD_SET(i, &res_read_set);
        }
        if (write_set && FD_ISSET(i, write_set) &&
            (fd->revents & (POLLOUT | POLLERR | POLLHUP)) != 0) {
            FD_SET(i, &res_write_set);
        }
        if (except_set && FD_ISSET(i, except_set) &&
            (fd->revents & POLLPRI) != 0) {
            FD_SET(i, &res_except_set);
        }
    }

    free(fds);
    if (read_set) {
        *read_set = res_read_set;
    }
    if (write_set) {
        *write_set = res_write_set;
    }
    if (except_set) {
        *except_set = res_except_set;
    }

    return 0;
}

int sys_ptsname(int fd, char *buffer, size_t length) {
    int index;
    int result;
    if (sys_ioctl(fd, TIOCGPTN, &index, &result); result) {
        return result;
    }
    if ((size_t)snprintf(buffer, length, "/dev/pts/%d", index) >= length) {
        return ERANGE;
    }
    return 0;
}

int sys_posix_openpt(int flags, int *fd) {
    return sys_open("/dev/ptmx", flags, 0, fd);
}

int sys_unlockpt(int fd) {
    int unlock = 0;
    int result;

    if (sys_ioctl(fd, TIOCSPTLCK, &unlock, &result); result) {
        return result;
    }

    return 0;
}

int sys_readlinkat(int         dirfd,
                   const char *path,
                   void       *buffer,
                   size_t      max_size,
                   ssize_t    *length) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_READLINKAT,
                                         dirfd,
                                         path,
                                         buffer,
                                         max_size);

    if (0 != ret.errno) {
        return ret.errno;
    }

    *length = ret.ret;
    return 0;
}

int sys_readlink(const char *path,
                 void       *data,
                 size_t      max_size,
                 ssize_t    *length) {
    return sys_readlinkat(AT_FDCWD, path, data, max_size, length);
}

int sys_symlinkat(const char *target_path, int dirfd, const char *link_path) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_SYMLINKAT,
                                         target_path,
                                         dirfd,
                                         link_path);

    if (0 != ret.errno) {
        return ret.errno;
    }

    return 0;
}

int sys_symlink(const char *target_path, const char *link_path) {
    return sys_symlinkat(target_path, AT_FDCWD, link_path);
}

int sys_unlinkat(int fd, const char *path, int flags) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_UNLINKAT,
                                         fd,
                                         path,
                                         flags);

    if (0 != ret.errno) {
        return ret.errno;
    }

    return 0;
}

int sys_rmdir(const char *path) {
    return sys_unlinkat(AT_FDCWD, path, AT_REMOVEDIR);
}

int sys_socket(int domain, int type_and_flags, int proto, int *fd) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_SOCKET,
                                         domain,
                                         type_and_flags,
                                         proto);

    if (0 != ret.errno) {
        return ret.errno;
    }

    *fd = (int)ret.ret;
    return 0;
}

int sys_bind(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_BIND,
                                         fd,
                                         addr_ptr,
                                         addr_length);

    if (0 != ret.errno) {
        return ret.errno;
    }

    return 0;
}

int sys_listen(int fd, int backlog) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_LISTEN,
                                         fd,
                                         backlog);

    if (0 != ret.errno) {
        return ret.errno;
    }

    return 0;
}

int fcntl_helper(int fd, int request, int *result, ...) {
    va_list args;
    va_start(args, result);
    if (!mlibc::sys_fcntl) {
        return ENOSYS;
    }
    int ret = mlibc::sys_fcntl(fd, request, args, result);
    va_end(args);
    return ret;
}

int sys_accept(int              fd,
               int             *newfd,
               struct sockaddr *addr_ptr,
               socklen_t       *addr_length,
               int              flags) {
    (void)flags;
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_ACCEPT,
                                         fd,
                                         addr_ptr,
                                         addr_length);

    if (0 != ret.errno) {
        return ret.errno;
    }

    *newfd = (int)ret.ret;

    if (flags & SOCK_NONBLOCK) {
        int fcntl_ret = 0;
        fcntl_helper(*newfd, F_GETFL, &fcntl_ret);
        fcntl_helper(*newfd, F_SETFL, &fcntl_ret, fcntl_ret | O_NONBLOCK);
    }

    if (flags & SOCK_CLOEXEC) {
        int fcntl_ret = 0;
        fcntl_helper(*newfd, F_GETFD, &fcntl_ret);
        fcntl_helper(*newfd, F_SETFD, &fcntl_ret, fcntl_ret | FD_CLOEXEC);
    }

    return 0;
}

int sys_connect(int                    fd,
                const struct sockaddr *addr_ptr,
                socklen_t              addr_length) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_CONNECT,
                                         fd,
                                         addr_ptr,
                                         addr_length);

    if (0 != ret.errno) {
        return ret.errno;
    }

    return 0;
}

int sys_setitimer(int                     which,
                  const struct itimerval *new_value,
                  struct itimerval       *old_value) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_SETITIMER,
                                         which,
                                         new_value,
                                         old_value);

    if (0 != ret.errno) {
        return ret.errno;
    }

    return 0;
}

int sys_mkdirat(int dirfd, const char *path, mode_t mode) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_MKDIRAT,
                                         dirfd,
                                         path,
                                         mode);

    if (0 != ret.errno) {
        return ret.errno;
    }

    return 0;
}

int sys_mkdir(const char *path, mode_t mode) {
    return sys_mkdirat(AT_FDCWD, path, mode);
}

int sys_pause() {
    poll(NULL, 0, -1);
    return 0;
}

int sys_sleep(time_t *secs, long *nanos) {
    time_t s   = (NULL != secs) ? *secs : 0;
    time_t n   = (NULL != nanos) ? *nanos : 0;
    time_t tot = s * 1000UL + n / 1000000UL;
    poll(NULL, 0, tot);
    return 0;
}

int sys_peername(int              fd,
                 struct sockaddr *addr_ptr,
                 socklen_t        max_addr_length,
                 socklen_t       *actual_length) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_GETPEERNAME,
                                         fd,
                                         addr_ptr,
                                         max_addr_length);

    if (0 != ret.errno) {
        return ret.errno;
    }

    *actual_length = (socklen_t)ret.ret;
    return 0;
}

#endif

int sys_msg_send(int                  sockfd,
                 const struct msghdr *hdr,
                 int                  flags,
                 ssize_t             *length) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_SENDMSG,
                                         sockfd,
                                         hdr,
                                         flags);

    if (0 != ret.errno) {
        return ret.errno;
    }

    *length = (ssize_t)ret.ret;
    return 0;
}

int sys_msg_recv(int sockfd, struct msghdr *hdr, int flags, ssize_t *length) {
    struct __syscall_ret ret = __syscall(__SALERNOS_SYSCALL_RECVMSG,
                                         sockfd,
                                         hdr,
                                         flags);

    if (0 != ret.errno) {
        return ret.errno;
    }

    *length = (ssize_t)ret.ret;
    return 0;
}

} // namespace mlibc
