#pragma once

#include <stddef.h>
#include <stdint.h>

#define LOOKUP_FOUND     0
#define LOOKUP_NOT_FOUND -1

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
    TYPE_ENTRY,
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

int table_lookup(struct table_entry *out,
                 struct table       *table,
                 const char         *name,
                 size_t              namelen,
                 enum type           type);
