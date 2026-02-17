#include <fcntl.h>
#include <salernos/devprofile.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

static const char *g_module_name;
static int (*g_comparator)(const void *, const void *);

static int comparator_real(const void *a_, const void *b_) {
    const struct devprofile_fn_data *a = a_;
    const struct devprofile_fn_data *b = b_;

    if (a->real_time_ns > b->real_time_ns) {
        return -1;
    } else if (a->real_time_ns < b->real_time_ns) {
        return 1;
    } else {
        return 0;
    }
}

static int comparator_cpu(const void *a_, const void *b_) {
    const struct devprofile_fn_data *a = a_;
    const struct devprofile_fn_data *b = b_;

    if (a->cpu_time_ns > b->cpu_time_ns) {
        return -1;
    } else if (a->cpu_time_ns < b->cpu_time_ns) {
        return 1;
    } else {
        return 0;
    }
}

static int comparator_calls(const void *a_, const void *b_) {
    const struct devprofile_fn_data *a = a_;
    const struct devprofile_fn_data *b = b_;

    if (a->num_calls > b->num_calls) {
        return -1;
    } else if (a->num_calls < b->num_calls) {
        return 1;
    } else {
        return 0;
    }
}

static int comparator_ratio(const void *a_, const void *b_) {
    const struct devprofile_fn_data *a = a_;
    const struct devprofile_fn_data *b = b_;

    if (0 == a->num_calls && 0 == b->num_calls) {
        return 0;
    }

    if (0 == a->num_calls) {
        return 1;
    }

    if (0 == b->num_calls) {
        return -1;
    }

    float p1 = (float)a->cpu_time_ns / a->num_calls;
    float p2 = (float)b->cpu_time_ns / b->num_calls;

    if (p1 > p2) {
        return -1;
    } else if (p1 < p2) {
        return 1;
    }

    return 0;
}

static void error_printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "%s: ", g_module_name);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}

static int open_devprofile(void) {
    int fd = open("/dev/profile", O_RDWR);
    if (-1 == fd) {
        error_printf("unable to open profiler device");
        exit(EXIT_FAILURE);
    }
    return fd;
}

static void *checked_malloc(size_t size) {
    void *ret = malloc(size);
    if (NULL == ret) {
        error_printf("out of memory");
        exit(EXIT_FAILURE);
    }
    return ret;
}

static void checked_ioctl(int fd, unsigned long op, void *buf) {
    if (-1 == ioctl(fd, op, buf)) {
        perror("ioctl");
        exit(EXIT_FAILURE);
    }
}

static void print_function_table(struct devprofile_fn_data *data,
                                 size_t                     num_functions) {
    printf("%-32s %15s %15s %10s %15s\n\n",
           "Name",
           "Real Time (ns)",
           "CPU Time (ns)",
           "Calls",
           "CPU Time / Calls");

    for (size_t i = 0; i < num_functions; i++) {
        struct devprofile_fn_data *row = &data[i];
        printf("%-32s %15ju %15ju %10zu %15lf\n",
               row->name,
               row->real_time_ns,
               row->cpu_time_ns,
               row->num_calls,
               (0 != row->num_calls) ? (float)row->cpu_time_ns / row->num_calls
                                     : 0);
    }
}

static void profile_functions(void) {
    int devprofile = open_devprofile();

    size_t num_functions;
    checked_ioctl(devprofile,
                  DEVPROFILE_IOCTL_GET_NUM_FUNCTIONS,
                  &num_functions);

    struct devprofile_fn_res *res = checked_malloc(
        DEVPROFILE_SIZEOF_FN_RES(num_functions));
    checked_ioctl(devprofile, DEVPROFILE_IOCTL_GET_FUNCTIONS, res);

    qsort(res->data,
          num_functions,
          sizeof(struct devprofile_fn_data),
          g_comparator);

    print_function_table(res->data, num_functions);
}

static void profile_syscalls(void) {
    int devprofile = open_devprofile();

    size_t num_syscalls;
    checked_ioctl(devprofile, DEVPROFILE_IOCTL_GET_NUM_SYSCALLS, &num_syscalls);

    struct devprofile_fn_res *res = checked_malloc(
        DEVPROFILE_SIZEOF_FN_RES(num_syscalls));
    checked_ioctl(devprofile, DEVPROFILE_IOCTL_GET_SYSCALLS, res);

    qsort(res->data,
          num_syscalls,
          sizeof(struct devprofile_fn_data),
          g_comparator);

    print_function_table(res->data, num_syscalls);
}

int main(int argc, const char *const argv[]) {
    if (argc < 1) {
        return EXIT_FAILURE;
    }

    g_module_name = argv[0];
    g_comparator  = comparator_ratio;

    if (argc < 2) {
        error_printf("not enough arguments, try --help");
        return EXIT_FAILURE;
    }

    for (int i = 2; i < argc; i++) {
        const char *option = argv[i];

        if (0 == strcmp(option, "--by-real")) {
            g_comparator = comparator_real;
        } else if (0 == strcmp(option, "--by-cpu")) {
            g_comparator = comparator_cpu;
        } else if (0 == strcmp(option, "--by-calls")) {
            g_comparator = comparator_calls;
        } else if (0 == strcmp(option, "--by-ratio")) {
            g_comparator = comparator_ratio;
        } else {
            error_printf("unknown option '%s'", option);
            return EXIT_FAILURE;
        }
    }

    const char *command = argv[1];

    if (0 == strcmp(command, "--help")) {
        printf("\n\tusage: %s <command> [<options>]\n\n", argv[0]);
        printf("\tcommands:\n\n");
        printf("\t\t--help\tshow this menu\n");
        printf(
            "\t\t--functions\tshow kernel profile with function granularity\n");
        printf("\t\t--syscalls\tshow kernel profile with system call "
               "granularity\n");
        printf("\n\n\toptions:\n\n");
        printf("\t\t--by-real\tsort by real time\n");
        printf("\t\t--by-cpu\tsort by cpu time\n");
        printf("\t\t--by-calls\tsort by number of calls\n");
        printf("\t\t--by-ratio\tsort by ratio (default)\n");
        printf("\n");
    } else if (0 == strcmp(command, "--functions")) {
        profile_functions();
    } else if (0 == strcmp(command, "--syscalls")) {
        profile_syscalls();
    } else {
        error_printf("unknown command '%s'", command);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
