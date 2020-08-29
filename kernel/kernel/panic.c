#include <kernel/tty.h>
#include <stdbool.h>
#include <stdio.h>

void panic(const char* message)
{
    printf("KERNEL PANIC: %s\n", message);
    while (true) {
    }
    __builtin_unreachable();
}
