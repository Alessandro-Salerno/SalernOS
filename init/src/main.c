#include <init/interpreter.h>
#include <init/log.h>
#include <init/parse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_CONF_PATH "/boot/initcfg"
// #define INIT_CONF_PATH "./testcfg"

static void handle_sigchild(int signo) {
    IERR("received SIGCHILD, a child process has exited!");
}

int main(void) {
    ILOG("opening configuration file at %s", INIT_CONF_PATH);
    FILE *cfg_file = fopen(INIT_CONF_PATH, "r");

    if (NULL == cfg_file) {
        IERR("failed to open configuration file");
        return EXIT_FAILURE;
    }

    fseek(cfg_file, 0, SEEK_END);
    size_t cfg_len = ftell(cfg_file);
    fseek(cfg_file, 0, SEEK_SET);

    ILOG("allocating space for configuration file contents");
    char *config = malloc(cfg_len + 1);

    if (NULL == config) {
        IERR("failed to allocate buffer");
        fclose(cfg_file);
        return EXIT_FAILURE;
    }

    ILOG("loading configuration file contents");
    fread(config, cfg_len, 1, cfg_file);

    struct directive *directives;
    size_t            num_directives;

    ILOG("parsing configuration file contents");
    if (PARSE_ERR == parse_config(&directives, &num_directives, config)) {
        free(config);
        fclose(cfg_file);
        return EXIT_FAILURE;
    }

    ILOG("running init");
    int r = interpret_config(directives, num_directives);

    if (INTERPRET_OK != r) {
        IERR("failed to run init");
    }
}
