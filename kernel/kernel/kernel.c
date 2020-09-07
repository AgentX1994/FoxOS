#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <kernel/multiboot.h>
#include <kernel/tty.h>

__attribute__((constructor)) void initialize(void)
{
    /* initialize terminal interface */
    terminal_initialize();
    terminal_writestring("Terminal initialized...\n\n\n\n");
}

typedef struct {
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
} __attribute__((packed)) mem_map_entry;

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
        printf("mmap_length = %uB\n", mbd->mmap_length);
        printf("mmap_addr   = 0x%x\n", mbd->mmap_addr);

        // traverse and print the mmap
        mem_map_entry* mmap;
        for (mmap = (mem_map_entry*)mbd->mmap_addr;
             mmap < (mem_map_entry*)(mbd->mmap_addr + mbd->mmap_length);
             mmap = ((uint32_t)mmap + mmap->size + sizeof(mmap->size))) {
            printf("base_addr = 0x%llx, length = 0x%llx, type=%u\n",
                mmap->addr,
                mmap->len,
                mmap->type);
        }
    }
}
