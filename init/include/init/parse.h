#pragma once

#include <stddef.h>

#define PARSE_OK  0
#define PARSE_ERR -1

struct directive_arg {
    const char *str;
    size_t      len;
};

struct directive {
    struct directive_arg *args;
    size_t                num_args;
};

int parse_config(struct directive **out, size_t *out_num, const char *config);
