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

#define STUB_ENOSYS                                                     \
    {                                                                   \
        mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n" \
                            << frg::endlog;                             \
        return ENOSYS;                                                  \
    }

#define STUB_NULL                                                       \
    {                                                                   \
        mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n" \
                            << frg::endlog;                             \
        return NULL;                                                    \
    }

#define STUB_VOID                                                       \
    {                                                                   \
        mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n" \
                            << frg::endlog;                             \
        return;                                                         \
    }

#define STUB_NORETURN                               \
    {                                               \
        __ensure(!"STUB_ONLY function was called"); \
        __builtin_unreachable();                    \
    }

namespace mlibc {

#ifndef MLIBC_BUILDING_RTLD

[[noreturn]] void sys_thread_exit() {
    STUB_NORETURN
}

extern "C" void __mlibc_thread_entry();

int sys_clone(void *tcb, pid_t *pid_out, void *stack) {
    STUB_ENOSYS
}

int sys_kill(pid_t pid, int signal) {
    STUB_ENOSYS
}

int sys_tcgetattr(int fd, struct termios *attr) {
    STUB_ENOSYS
}

int sys_tcsetattr(int fd, int optional_action, const struct termios *attr) {
    STUB_ENOSYS
}

#endif

int sys_tcb_set(void *pointer) {
    STUB_ENOSYS
}

#ifndef MLIBC_BUILDING_RTLD

int sys_ppoll(struct pollfd         *fds,
              int                    nfds,
              const struct timespec *timeout,
              const sigset_t        *sigmask,
              int                   *num_events) {
    STUB_ENOSYS
}

int sys_pause() {
    STUB_ENOSYS
}

int sys_poll(struct pollfd *fds, nfds_t count, int timeout, int *num_events) {
    STUB_ENOSYS
}

int sys_pselect(int                    nfds,
                fd_set                *read_set,
                fd_set                *write_set,
                fd_set                *except_set,
                const struct timespec *timeout,
                const sigset_t        *sigmask,
                int                   *num_events) {
    STUB_ENOSYS
}

#endif

int sys_futex_wait(int *pointer, int expected, const struct timespec *time) {
    STUB_ENOSYS
}

int sys_futex_wake(int *pointer) {
    STUB_ENOSYS
}

#ifndef MLIBC_BUILDING_RTLD

int sys_isatty(int fd) {
    STUB_ENOSYS
}

typedef struct {
    ino_t          d_ino;
    off_t          d_off;
    unsigned short d_reclen;
    unsigned char  d_type;
    char           d_name[1024];
} dent_t;

int sys_getcwd(char *buffer, size_t size) {
    STUB_ENOSYS
}

#endif

int sys_openat(int dirfd, const char *path, int flags, mode_t mode, int *fd) {
    STUB_ENOSYS
}

#ifndef MLIBC_BUILDING_RTLD

int sys_open_dir(const char *path, int *handle) {
    STUB_ENOSYS
}

int sys_read_entries(int     fd,
                     void   *buffer,
                     size_t  max_size,
                     size_t *bytes_read) {
    STUB_ENOSYS
}

#endif

int sys_close(int fd) {
    STUB_ENOSYS
}

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
    STUB_ENOSYS
}

#ifndef MLIBC_BUILDING_RTLD

int sys_readlink(const char *path,
                 void       *data,
                 size_t      max_size,
                 ssize_t    *length) {
    STUB_ENOSYS
}

int sys_link(const char *old_path, const char *new_path) {
    STUB_ENOSYS
}

int sys_linkat(int         olddirfd,
               const char *old_path,
               int         newdirfd,
               const char *new_path,
               int         flags) {
    STUB_ENOSYS
}

int sys_unlinkat(int fd, const char *path, int flags) {
    STUB_ENOSYS
}

int sys_fchmod(int fd, mode_t mode) {
    STUB_ENOSYS
}

int sys_fsync(int) {
    STUB_ENOSYS
}

#endif

int sys_vm_unmap(void *pointer, size_t size) {
    STUB_ENOSYS
}

#ifndef MLIBC_BUILDING_RTLD

int sys_vm_protect(void *pointer, size_t size, int prot) {
    STUB_ENOSYS
}

#endif

// TODO: kinda a stub
int sys_anon_free(void *pointer, size_t size) {
    return sys_vm_unmap(pointer, size);
}

#ifndef MLIBC_BUILDING_RTLD

pid_t sys_getpid(){STUB_ENOSYS}

pid_t sys_getppid(){STUB_ENOSYS}

uid_t sys_getuid(){STUB_ENOSYS}

uid_t sys_geteuid(){STUB_ENOSYS}

gid_t sys_getgid() {
    STUB_ENOSYS
}

int sys_setgid(gid_t gid) {
    STUB_ENOSYS
}

int sys_getpgid(pid_t pid, pid_t *pgid){STUB_ENOSYS}

gid_t sys_getegid() {
    STUB_ENOSYS
}

int sys_setpgid(pid_t pid, pid_t pgid) {
    STUB_ENOSYS
}

int sys_setsid(pid_t *sid) {
    STUB_ENOSYS
}

int sys_ttyname(int fd, char *buf, size_t size) {
    STUB_ENOSYS
}

int sys_clock_get(int clock, time_t *secs, long *nanos) {
    STUB_ENOSYS
}

int sys_stat(fsfd_target  fsfdt,
             int          fd,
             const char  *path,
             int          flags,
             struct stat *statbuf) {
    STUB_ENOSYS
}

int sys_faccessat(int dirfd, const char *pathname, int mode, int flags) {
    STUB_ENOSYS
}

int sys_access(const char *path, int mode) {
    return sys_faccessat(AT_FDCWD, path, mode, 0);
}

int sys_pipe(int *fds, int flags) {
    STUB_ENOSYS
}

int sys_chdir(const char *path) {
    STUB_ENOSYS
}

int sys_mkdir(const char *path, mode_t mode) {
    STUB_ENOSYS
}

int sys_mkdirat(int dirfd, const char *path, mode_t mode) {
    STUB_ENOSYS
}

int sys_socket(int domain, int type_and_flags, int proto, int *fd) {
    STUB_ENOSYS
}

int sys_socketpair(int domain, int type_and_flags, int proto, int *fds) {
    STUB_ENOSYS
}

int sys_bind(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
    STUB_ENOSYS
}

int sys_connect(int                    fd,
                const struct sockaddr *addr_ptr,
                socklen_t              addr_length) {
    STUB_ENOSYS
}

int sys_fchownat(int         dirfd,
                 const char *pathname,
                 uid_t       owner,
                 gid_t       group,
                 int         flags) {
    STUB_ENOSYS
}

int sys_accept(int              fd,
               int             *newfd,
               struct sockaddr *addr_ptr,
               socklen_t       *addr_length,
               int              flags) {
    STUB_ENOSYS
}

int sys_getsockopt(int fd,
                   int layer,
                   int number,
                   void *__restrict buffer,
                   socklen_t *__restrict size) {
    STUB_ENOSYS
}

int sys_setsockopt(int         fd,
                   int         layer,
                   int         number,
                   const void *buffer,
                   socklen_t   size) {
    STUB_ENOSYS
}

int sys_msg_recv(int sockfd, struct msghdr *hdr, int flags, ssize_t *length) {
    STUB_ENOSYS
}

int sys_peername(int              fd,
                 struct sockaddr *addr_ptr,
                 socklen_t        max_addr_length,
                 socklen_t       *actual_length) {
    STUB_ENOSYS
}

int sys_listen(int fd, int backlog) {
    STUB_ENOSYS
}

int sys_inotify_create(int flags, int *fd) {
    STUB_ENOSYS
}

int sys_fcntl(int fd, int request, va_list args, int *result) {
    STUB_ENOSYS
}

int sys_dup(int fd, int flags, int *newfd) {
    STUB_ENOSYS
}

int sys_dup2(int fd, int flags, int newfd) {
    STUB_ENOSYS
}

int sys_sigprocmask(int how,
                    const sigset_t *__restrict set,
                    sigset_t *__restrict retrieve) {
    STUB_ENOSYS
}

extern "C" void __mlibc_restorer();

int sys_sigaction(int                     signum,
                  const struct sigaction *act,
                  struct sigaction       *oldact) {
    STUB_ENOSYS
}

int sys_signalfd_create(sigset_t mask, int flags, int *fd) {
    STUB_ENOSYS
}

int sys_ptsname(int fd, char *buffer, size_t length) {
    STUB_ENOSYS
}

int sys_posix_openpt(int flags, int *fd) {
    STUB_ENOSYS
}

int sys_unlockpt(int fd) {
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
    STUB_ENOSYS
}

int sys_sethostname(const char *buffer, size_t bufsize) {
    STUB_ENOSYS
}

int sys_sleep(time_t *secs, long *nanos) {
    STUB_ENOSYS
}

int sys_getitimer(int, struct itimerval *) {
    STUB_ENOSYS
}

int sys_setitimer(int                     which,
                  const struct itimerval *new_value,
                  struct itimerval       *old_value) {
    STUB_ENOSYS
}

#endif

} // namespace mlibc
