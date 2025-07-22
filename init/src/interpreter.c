#include <asm/ioctls.h>
#include <fcntl.h>
#include <init/interpreter.h>
#include <init/log.h>
#include <init/parse.h>
#include <init/table.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>

#define EVAL_OK             LOOKUP_FOUND
#define EVAL_NOTABLE        -1
#define EVAL_NOT_RECOGNIZED -2
#define EVAL_WRONG_TYPE     -3

#define OPT_CLEAR 1

static int   g_active_options = 0;
static int   g_tty_lock       = 1;
static pid_t g_children[20] = {0}; // TODO: arbitrary number higher than TTY_MAX
static int   g_num_children = 0;

static struct table_entry g_values_entries[] = {
    {"TTY_TEXT_MODE", TYPE_INT, .value.local = (int)1},
    {"OPT_CLEAR", TYPE_INT, .value.local = (int)OPT_CLEAR},
};
static struct table g_values = CONST_TABLE(g_values_entries);

static void handle_sigusr1(int signo) {
    __atomic_store_n(&g_tty_lock, 0, __ATOMIC_SEQ_CST);
}

static void wait_start_signal(void) {
    while (__atomic_load_n(&g_tty_lock, __ATOMIC_SEQ_CST)) {
        // waiting until init has finished
    }
}

static int eval(void         *out,
                enum type    *out_type,
                struct table *table,
                const char   *value,
                size_t        value_len,
                enum type     exp_type);

static int eval_entry(void *out, enum type *out_type, struct table_entry *e) {
    switch (e->type) {
    case TYPE_STRING:
        return eval(
            out, out_type, NULL, e->value.ptr, strlen(e->value.ptr), e->type);
    case TYPE_INT:
        *(int *)out = (int)e->value.local;
        *out_type   = e->type;
        return EVAL_OK;
    default:
        return EVAL_WRONG_TYPE;
    }
}

static int eval(void         *out,
                enum type    *out_type,
                struct table *table,
                const char   *value,
                size_t        value_len,
                enum type     exp_type) {
    enum type _;
    if (NULL == out_type) {
        out_type = &_;
    }

    if ('$' == value[0]) {
        if (NULL == table) {
            return EVAL_NOTABLE;
        }

        struct table_entry e;
        int ret = table_lookup(&e, table, &value[1], value_len - 1, exp_type);

        if (LOOKUP_FOUND == ret) {
            return eval_entry(out, out_type, &e);
        }

        return EVAL_NOT_RECOGNIZED;
    }

    if (TYPE_STRING == exp_type) {
        char *buf = malloc(value_len + 1);
        memcpy(buf, value, value_len);
        buf[value_len] = 0;
        *(char **)out  = buf;
        *out_type      = TYPE_STRING;
        return EVAL_OK;
    }

    // TODO: add int type
    return EVAL_NOT_RECOGNIZED;
}

static int cb_env(struct directive *d) {
    if (3 != d->num_args) {
        return INTERPRET_ERR;
    }

    const char *env_name, *env_value;

    if (EVAL_OK != eval(&env_name,
                        NULL,
                        &g_values,
                        d->args[1].str,
                        d->args[1].len,
                        TYPE_STRING)) {
        IERR("unable to evaluate env name %.*s",
             (int)d->args[1].len,
             d->args[1].str);
        return INTERPRET_ERR;
    }

    if (EVAL_OK != eval(&env_value,
                        NULL,
                        &g_values,
                        d->args[2].str,
                        d->args[2].len,
                        TYPE_STRING)) {
        IERR("unable to evaluate env value %.*s for env %.*s",
             (int)d->args[2].len,
             d->args[2].str,
             (int)d->args[1].len,
             d->args[1].str);
        return INTERPRET_ERR;
    }

    setenv(env_name, env_value, true);
    ILOG("setting env %s=%s", env_name, env_value);
    return INTERPRET_OK;
}

static int cb_tty(struct directive *d) {
    if (d->num_args < 4) {
        return INTERPRET_ERR;
    }

    const char *tty_path;
    int         tty_mode;
    size_t      argc = d->num_args - 3;
    char      **argv = malloc(sizeof(char *) * (argc + 1));
    argv[argc]       = NULL;

    if (EVAL_OK != eval(&tty_path,
                        NULL,
                        &g_values,
                        d->args[1].str,
                        d->args[1].len,
                        TYPE_STRING)) {
        IERR("failed to evaluate tty path %.*s",
             (int)d->args[1].len,
             d->args[1].str);
        return INTERPRET_ERR;
    }

    if (EVAL_OK != eval(&tty_mode,
                        NULL,
                        &g_values,
                        d->args[2].str,
                        d->args[2].len,
                        TYPE_INT)) {
        IERR("failed to evaluate tty mode %.*s",
             (int)d->args[1].len,
             d->args[1].str);
        return INTERPRET_ERR;
    }

    for (size_t i = 0; i < argc; i++) {
        if (EVAL_OK != eval(&argv[i],
                            NULL,
                            &g_values,
                            d->args[3 + i].str,
                            d->args[3 + i].len,
                            TYPE_STRING)) {
            IERR("failed to evaluate command line argument %.*s",
                 (int)d->args[3 + i].len,
                 d->args[3 + i].str);
            return INTERPRET_ERR;
        }
    }

    int pid = fork();

    if (pid < 0) {
        IERR("failed to perform fork()");
        return INTERPRET_ERR;
    }

    if (0 == pid) {
        int tty_fd = open(tty_path, O_RDWR);
        if (-1 == tty_fd) {
            exit(-1);
        }

        dup2(tty_fd, STDOUT_FILENO);
        dup2(tty_fd, STDIN_FILENO);
        dup2(tty_fd, STDERR_FILENO);

        int sid = setsid();
        ioctl(tty_fd, TIOCSCTTY, NULL);
        ILOG("moved %s on %s to session sid=%d", argv[0], tty_path, sid);

        ILOG("running %s on %s", argv[0], tty_path);

        wait_start_signal();

        if (OPT_CLEAR & g_active_options) {
            fprintf(stderr, "\033[2J\033[H"); // ANSI escape to clear the screen
        }

        if (0 != execv(argv[0], argv)) {
            IERR("failed to perform execl() on %s", tty_path);
            exit(-1);
        }
    }

    g_children[g_num_children] = pid;
    g_num_children++;

    // TODO: cleanup all allocated memory
    return INTERPRET_OK;
}

static int cb_enable(struct directive *d) {
    if (2 != d->num_args) {
        return INTERPRET_ERR;
    }

    int option;

    if (EVAL_OK != eval(&option,
                        NULL,
                        &g_values,
                        d->args[1].str,
                        d->args[1].len,
                        TYPE_INT)) {
        IERR("failed to evaluate option %.*s",
             (int)d->args[1].len,
             d->args[1].str);
        return INTERPRET_ERR;
    }

    g_active_options |= option;
    ILOG("set option with id=%04x, active_options=%04x",
         option,
         g_active_options);
    return INTERPRET_OK;
}

static struct table_entry g_directive_entries[] = {
    {"env", TYPE_FUNCTION, .value.ptr = cb_env},
    {"tty", TYPE_FUNCTION, .value.ptr = cb_tty},
    {"enable", TYPE_FUNCTION, .value.ptr = cb_enable},
};
static struct table g_directives = CONST_TABLE(g_directive_entries);

int interpret_directive(struct directive *d, size_t i, size_t n) {
    if (0 == d->num_args) {
        return INTERPRET_ERR;
    }

    ILOG("processing directive %zu/%zu: %.*s",
         i + 1,
         n,
         (int)d->args[0].len,
         d->args[0].str);

    struct table_entry drt_e;

    if (LOOKUP_NOT_FOUND == table_lookup(&drt_e,
                                         &g_directives,
                                         d->args[0].str,
                                         d->args[0].len,
                                         TYPE_FUNCTION)) {
        IERR("%.*s is not a known directive",
             (int)d->args[0].len,
             d->args[0].str);
        return INTERPRET_ERR;
    }

    int (*cb)(struct directive *) = drt_e.value.ptr;
    return cb(d);
}

int interpret_config(struct directive *directives, size_t num_directives) {
    ILOG("setting signal handler for SIGUSR1");
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = handle_sigusr1;
    if (0 != sigaction(SIGUSR1, &act, NULL)) {
        IERR("failed to perform sigaction() for SIGUSR1");
        exit(-1);
    }

    for (size_t i = 0; i < num_directives; i++) {
        if (INTERPRET_OK !=
            interpret_directive(&directives[i], i, num_directives)) {
            return INTERPRET_ERR;
        }
    }

    // Notify all children that they can start
    ILOG("sending SIGUSR1 to all children");
    for (int i = 0; i < g_num_children; i++) {
        kill(g_children[i], SIGUSR1);
    }

    return INTERPRET_OK;
}
