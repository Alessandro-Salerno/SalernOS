#include <stdarg.h>
#include <stdint.h>

#define __SALERNOS_SYSCALL_TEST     0
#define __SALERNOS_SYSCALL_WRITE    1
#define __SALERNOS_SYSCALL_READ     2
#define __SALERNOS_SYSCALL_EXECVE   3
#define __SALERNOS_SYSCALL_FORK     4
#define __SALERNOS_SYSSCALL_SYSINFO 5
#define __SALERNOS_SYSCALL_WAITPID  6
#define __SALERNOS_SYSCALL_EXIT     7
#define __SALERNOS_SYSCALL_IOCTL    8
#define __SALERNOS_SYSCALL_OPEN     9
#define __SALERNOS_SYSCALL_MMAP     10
#define __SALERNOS_SYSCALL_SET_TLS  11
#define __SALERNOS_SYSCALL_SEEK     12
#define __SALERNOS_SYSCALL_ISATTY   13
#define __SALERNOS_SYSCALL_STAT     14
#define __SALERNOS_SYSCALL_TRUNCATE 15
#define __SALERNOS_SYSCALL_PIPE     16

struct __syscall_ret {
    uint64_t ret;
    uint64_t errno;
};

static struct __syscall_ret __syscall(int number, ...) {
    va_list args;
    va_start(args, number);
    register uint64_t    a1 asm("rdi") = va_arg(args, uint64_t);
    register uint64_t    a2 asm("rsi") = va_arg(args, uint64_t);
    register uint64_t    a3 asm("rdx") = va_arg(args, uint64_t);
    register uint64_t    a4 asm("rcx") = va_arg(args, uint64_t);
    struct __syscall_ret s;
    asm volatile("int $0x80"
                 : "=a"(s.ret), "=d"(s.errno)
                 : "a"(number), "r"(a1), "r"(a2), "r"(a3), "r"(a4)
                 : "r11", "memory");
    va_end(args);
    return s;
}
