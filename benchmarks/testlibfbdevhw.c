#define _GNU_SOURCE
#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main(void) {
    const char *lib_path = "/usr/lib/xorg/modules/libfbdevhw.so";
    struct stat sb;

    // Step 1: Check if the file exists
    if (stat(lib_path, &sb) != 0) {
        fprintf(stderr,
                "ERROR: Could not stat '%s': %s (errno: %d)\n",
                lib_path,
                strerror(errno),
                errno);
        return 1;
    }

    // Step 2: Check if it's a regular file
    if (!S_ISREG(sb.st_mode)) {
        fprintf(stderr, "ERROR: '%s' is not a regular file.\n", lib_path);
        return 2;
    }

    // Step 3: Check for read access
    if (access(lib_path, R_OK) != 0) {
        fprintf(stderr,
                "ERROR: No read access to '%s': %s (errno: %d)\n",
                lib_path,
                strerror(errno),
                errno);
        return 3;
    }

    // Step 4: Attempt to load the shared library with dlopen
    errno = 0;
    dlerror(); // Clear any existing error
    void       *handle     = dlopen(lib_path, RTLD_NOW);
    const char *dlopen_err = dlerror();
    if (!handle) {
        fprintf(stderr,
                "ERROR: dlopen() failed for '%s': %s\n",
                lib_path,
                dlopen_err ? dlopen_err : "Unknown error");
        return 4;
    } else if (dlopen_err) {
        // dlopen succeeded, but dlerror set an error? Unexpected.
        fprintf(stderr,
                "WARNING: dlopen() succeeded, but dlerror reported: %s\n",
                dlopen_err);
    }

    // Step 5: Attempt to locate the symbol fbdevHWEnterVT
    errno = 0;
    dlerror(); // Clear existing error
    void       *sym       = dlsym(handle, "fbdevHWEnterVT");
    const char *dlsym_err = dlerror();

    if (!sym) {
        if (dlsym_err) {
            fprintf(stderr,
                    "ERROR: dlsym() failed to find 'fbdevHWEnterVT': %s\n",
                    dlsym_err);
        } else {
            fprintf(stderr,
                    "ERROR: dlsym() returned NULL for 'fbdevHWEnterVT', but "
                    "dlerror was also NULL (unexpected)\n");
        }
        dlclose(handle);
        return 5;
    } else if (dlsym_err) {
        fprintf(stderr,
                "WARNING: dlsym() succeeded but dlerror reported: %s\n",
                dlsym_err);
    } else {
        printf("SUCCESS: Symbol 'fbdevHWEnterVT' found at address %p\n", sym);
    }

    // Step 6: Close the handle
    if (dlclose(handle) != 0) {
        fprintf(stderr,
                "ERROR: dlclose() failed: %s (errno: %d)\n",
                dlerror(),
                errno);
        return 6;
    }

    return 0;
}
