#include <stdio.h>

int main(int argc, char *argv[]) {
    printf("trying to write *(volatile int *)NULL = 10\n");
    *(volatile int *)NULL = 10;

    printf("ok then, about to crash by assigning argv[%d][0] = 'a' -> %s[0] = "
           "'a'\n",
           argc,
           argv[argc]);

    argv[argc][0] = 'a';

    printf("somehow survived\n");
    printf("trying to read %c\n", argv[argc][0]);
    printf("since we're still here, trying to write to random pointer in the "
           "address space\n");
    *(char *)(0x3553ab) = 'a';
    printf("giving up, this is all mapped for some reason\n");
    return 0;
}
