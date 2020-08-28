#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <kernel/tty.h>

__attribute__((constructor)) void initialize(void)
{
    /* initialize terminal interface */
    terminal_initialize();
    terminal_writestring("Terminal initialized...\n\n\n\n");
}

void kernel_main(void)
{
    /* initialize terminal interface */
    // terminal_initialize();
    // Moved terminal init to a global constructor

    terminal_writestring("Hello world!");
    // while (true) {
    //  terminal_writestring("abcdefghijklmnopqrstuvwxyz");
    //}
}
