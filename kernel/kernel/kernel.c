#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <kernel/multiboot.h>
#include <kernel/phys_mem.h>
#include <kernel/tty.h>

__attribute__((constructor)) void initialize(void)
{
    /* initialize terminal interface */
    terminal_initialize();
}

void kernel_main(multiboot_info_t* mbd, unsigned int magic)
{
    // Check magic is correct
    if (magic != 0x2BADB002) {
        // magic isn't correct
        terminal_writestring("WARNING: Not loaded by multiboot compatible boot loader!\n");
    }
    printf("Kernel Loaded! mbd = 0x%x\n", mbd);
    if (mbd->flags & MULTIBOOT_INFO_MEMORY) {
        printf("mem_lower = %uB\n", mbd->mem_lower * 1024);
        printf("mem_upper = %uB\n", mbd->mem_upper * 1024);
    }

    if (mbd->flags & MULTIBOOT_INFO_MEM_MAP) {
        setup_memory_map(mbd);
    }

    int64_t page = allocate_pages(1);
    int64_t pages = allocate_pages(2);
    int64_t pages2 = allocate_pages(3);
    free_pages(pages2, 3);
    free_pages(pages, 2);
    free_pages(page, 1);
}
