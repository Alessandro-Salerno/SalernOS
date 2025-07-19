#pragma once

#include <stdio.h>

#define ILOG(...)                  \
    fprintf(stderr, "[ i/log ] "); \
    fprintf(stderr, __VA_ARGS__);  \
    fprintf(stderr, "\n")

#define IERR(...)                  \
    fprintf(stderr, "[ i/err ] "); \
    fprintf(stderr, __VA_ARGS__);  \
    fprintf(stderr, "\n")
