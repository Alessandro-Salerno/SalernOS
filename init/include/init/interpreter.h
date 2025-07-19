#pragma once

#include <init/parse.h>
#include <stddef.h>

#define INTERPRET_OK  0
#define INTERPRET_ERR -1

int interpret_config(struct directive *directives, size_t num_directives);
