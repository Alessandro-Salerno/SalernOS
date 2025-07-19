#include <init/interpreter.h>
#include <init/log.h>
#include <init/parse.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define LOOKUP_FOUND     0
#define LOOKUP_NOT_FOUND -1

#define EVAL_OK             LOOKUP_FOUND
#define EVAL_NOTABLE        -1
#define EVAL_NOT_RECOGNIZED -2
#define EVAL_WRONG_TYPE     -3

#define PROP_CLEAR 1

#define CONST_TABLE(array)                                        \
    (struct table) {                                              \
        .entries     = array,                                     \
        .num_entries = sizeof(array) / sizeof(struct table_entry) \
    }

enum type {
    TYPE_INT,
    TYPE_STRING,
    TYPE_FUNCTION,
    TYPE_ANY,
    TYPE_UNDEF,
    TYPE_ENTRY, // generic entry
};

struct table_entry {
    const char *name;
    enum type   type;
    union {
        uintmax_t local;
        void     *ptr;
    } value;
};

struct table {
    struct table_entry *entries;
    size_t              num_entries;
};

static int g_active_properties = 0;

static int table_lookup(struct table_entry *out,
                        struct table       *table,
                        const char         *name,
                        size_t              namelen,
                        enum type           type) {
    for (size_t i = 0; i < table->num_entries; i++) {
        struct table_entry entry = table->entries[i];

        if (0 == strncmp(name, entry.name, namelen)) {
            if (type == entry.type || TYPE_ANY == type) {
                *out = entry;
                return LOOKUP_FOUND;
            }
        }
    }

    return LOOKUP_NOT_FOUND;
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
    if ('$' == value[0]) {
        if (NULL == table) {
            return EVAL_NOTABLE;
        }
        struct table_entry e;
        int ret = table_lookup(&e, table, &value[1], value_len - 1, exp_type);
        if (LOOKUP_FOUND == ret) {
            return eval_entry(out, out_type, &e);
        } else {
            return EVAL_NOT_RECOGNIZED;
        }
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

static struct table_entry g_values_entries[] = {
    {"TTY_TEXT_MODE", TYPE_INT, .value.local = (int)1},
    {"PROP_CLEAR", TYPE_INT, .value.local = (int)PROP_CLEAR},
};
static struct table g_values = CONST_TABLE(g_values_entries);

static int cb_env(struct directive *d) {
    if (3 != d->num_args) {
        return INTERPRET_ERR;
    }

    const char *env_name, *env_value;
    enum type   _;

    if (EVAL_OK != eval(&env_name,
                        &_,
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
                        &_,
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
    IERR("tty thingy not implemented yet\n");
    return INTERPRET_ERR;
}

static int cb_execl(struct directive *d) {
    if (2 != d->num_args) {
        return INTERPRET_ERR;
    }

    const char *path;
    enum type   _;

    if (EVAL_OK != eval(&path,
                        &_,
                        &g_values,
                        d->args[1].str,
                        d->args[1].len,
                        TYPE_STRING)) {
        IERR("unable to evaluate path %.*s",
             (int)d->args[1].len,
             d->args[1].str);
        return INTERPRET_ERR;
    }

    int pid = fork();

    if (pid < 0) {
        IERR("failed to perform fork()");
        return INTERPRET_ERR;
    }

    if (0 == pid) {
        ILOG("creating new process group for %s", path);
        if (0 != setpgid(0, 0)) {
            IERR("failed to set process group");
        }
        ILOG("switching to %s", path);
        if (PROP_CLEAR & g_active_properties) {
            fprintf(stderr, "\033[2J\033[H");
        }
        if (0 != execl(path, path, NULL)) {
            IERR("failed to perform execl() on %s", path);
            exit(-1);
        }
    }

    int status;
    waitpid(pid, &status, 0);
    return INTERPRET_OK;
}

static int cb_addprop(struct directive *d) {
    if (2 != d->num_args) {
        return INTERPRET_ERR;
    }

    int       property;
    enum type _;

    if (EVAL_OK != eval(&property,
                        &_,
                        &g_values,
                        d->args[1].str,
                        d->args[1].len,
                        TYPE_INT)) {
        IERR("unable to evaluate property %.*s",
             (int)d->args[1].len,
             d->args[1].str);
        return INTERPRET_ERR;
    }

    g_active_properties |= property;
    ILOG("set property with id=%04x, active_properties=%04x",
         property,
         g_active_properties);
    return INTERPRET_OK;
}

static struct table_entry g_directive_entries[] = {
    {"ENV", TYPE_FUNCTION, .value.ptr = cb_env},
    {"TTY", TYPE_FUNCTION, .value.ptr = cb_tty},
    {"EXECL", TYPE_FUNCTION, .value.ptr = cb_execl},
    {"ADDPROP", TYPE_FUNCTION, .value.ptr = cb_addprop},
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
    for (size_t i = 0; i < num_directives; i++) {
        if (INTERPRET_OK !=
            interpret_directive(&directives[i], i, num_directives)) {
            return INTERPRET_ERR;
        }
    }

    return INTERPRET_OK;
}
