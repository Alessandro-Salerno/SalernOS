#include <ctype.h>
#include <init/log.h>
#include <init/parse.h>
#include <stdlib.h>

// NOTE: this code is temporary, it was AI-generated to speed-up development, as
// currently the init system is only needed for use with multiple TTYs and does
// nothing else

static void skip_whitespace(const char **p) {
    while (**p && isspace((unsigned char)**p) && **p != '\n') {
        (*p)++;
    }
}

static int parse_line(const char *line_start, struct directive *out_dir) {
    const char           *p        = line_start;
    struct directive_arg *args     = NULL;
    size_t                num_args = 0;

    while (*p && *p != '\n') {
        skip_whitespace(&p);
        if (*p == '\0' || *p == '\n') {
            break;
        }

        const char *start;
        size_t      len;

        if (*p == '"') {
            p++; // skip opening quote
            start = p;
            while (*p && *p != '"') {
                p++;
            }
            if (*p != '"') {
                IERR("unexpected eof");
                free(args);
                return PARSE_ERR; // unterminated quote
            }
            len = p - start;
            p++; // skip closing quote
        } else {
            start = p;
            while (*p && !isspace((unsigned char)*p) && *p != '\n') {
                p++;
            }
            len = p - start;
        }

        struct directive_arg *new_args =
            realloc(args, (num_args + 1) * sizeof(struct directive_arg));
        if (!new_args) {
            IERR("failed to allocate memory for parser output");
            free(args);
            return PARSE_ERR;
        }

        args               = new_args;
        args[num_args].str = start;
        args[num_args].len = len;
        num_args++;
    }

    out_dir->args     = args;
    out_dir->num_args = num_args;
    return PARSE_OK;
}

int parse_config(struct directive **out, size_t *out_num, const char *config) {
    const char       *p              = config;
    struct directive *directives     = NULL;
    size_t            num_directives = 0;

    while (*p) {
        const char *line_start = p;

        while (*p && *p != '\n') {
            p++;
        }

        const char *line_end = p;
        while (line_start < line_end && isspace((unsigned char)*line_start)) {
            line_start++;
        }

        if (line_start < line_end) {
            struct directive *new_directives = realloc(
                directives, (num_directives + 1) * sizeof(struct directive));
            if (!new_directives) {
                // Free all previously allocated memory
                IERR("failed to allocate memory for parser output");
                for (size_t i = 0; i < num_directives; i++) {
                    free(directives[i].args);
                }
                free(directives);
                return PARSE_ERR;
            }

            directives = new_directives;
            if (parse_line(line_start, &directives[num_directives]) !=
                PARSE_OK) {
                for (size_t i = 0; i < num_directives; i++) {
                    free(directives[i].args);
                }
                free(directives);
                return PARSE_ERR;
            }

            num_directives++;
        }

        if (*p == '\n') {
            p++;
        }
    }

    *out     = directives;
    *out_num = num_directives;
    return PARSE_OK;
}
