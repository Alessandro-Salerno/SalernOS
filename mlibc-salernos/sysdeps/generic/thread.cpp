#include <bits/ensure.h>
#include <errno.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/tcb.hpp>
#include <sys/mman.h>

extern "C" void
__mlibc_thread_trampoline(void *(*fn)(void *), Tcb *tcb, void *arg) {
    if (mlibc::sys_tcb_set(tcb)) {
        __ensure(!"failed to set tcb for new thread");
    }

    mlibc::infoLogger() << "mlibc: tidAddr=" << &tcb->tid
                        << ", didExitAddr=" << &tcb->didExit << "\n"
                        << frg::endlog;

    mlibc::infoLogger() << "mlibc: waiting for thread to be initialized, tid="
                        << tcb->tid << "\n"
                        << frg::endlog;
    while (__atomic_load_n(&tcb->tid, __ATOMIC_RELAXED) == 0) {
        mlibc::sys_futex_wait(&tcb->tid, 0, nullptr);
    }
    mlibc::infoLogger() << "mlibc: thread initialized with tid=" << tcb->tid
                        << "\n"
                        << frg::endlog;

    tcb->invokeThreadFunc(reinterpret_cast<void *>(fn), arg);

    mlibc::infoLogger() << "mlibc: got out of thread tid=" << tcb->tid
                        << ", here didExit=" << tcb->didExit << "\n"
                        << frg::endlog;
    __atomic_store_n(&tcb->didExit, 1, __ATOMIC_RELEASE);
    mlibc::infoLogger() << "mlibc: set didExit=" << tcb->didExit
                        << "for thread tid=" << tcb->tid << "\n"
                        << frg::endlog;
    mlibc::sys_futex_wake(&tcb->didExit);

    mlibc::sys_thread_exit();
}

#define DEFAULT_STACK 0x400000

namespace mlibc {
int sys_prepare_stack(void  **stack,
                      void   *entry,
                      void   *arg,
                      void   *tcb,
                      size_t *stack_size,
                      size_t *guard_size,
                      void  **stack_base) {
    // TODO guard

    mlibc::infoLogger() << "mlibc: sys_prepare_stack() does not setup a guard!"
                        << frg::endlog;

    *guard_size = 0;

    *stack_size = *stack_size ? *stack_size : DEFAULT_STACK;

    if (!*stack) {
        *stack_base = mmap(NULL,
                           *stack_size,
                           PROT_READ | PROT_WRITE,
                           MAP_ANONYMOUS | MAP_PRIVATE,
                           -1,
                           0);
        if (*stack_base == MAP_FAILED) {
            return errno;
        }
    } else {
        *stack_base = *stack;
    }

    *stack = (void *)((char *)*stack_base + *stack_size);

    void **stack_it = (void **)*stack;

    *--stack_it = arg;
    *--stack_it = tcb;
    *--stack_it = entry;

    *stack = (void *)stack_it;

    return 0;
}
} // namespace mlibc
