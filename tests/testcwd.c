#include <stdio.h>
#include <unistd.h>

int main(void) {
    char buf[256];
    getcwd(buf, 256);
    printf("cwd: %s\n", buf);
    return 0;
}
