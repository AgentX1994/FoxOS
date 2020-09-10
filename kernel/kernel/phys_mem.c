#include <kernel/panic.h>
#include <kernel/phys_mem.h>

#include <stdio.h>

typedef struct {
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
} __attribute__((packed)) mem_map_entry;

/*
 * 4GB / 4KB = 1048576 potential Pages
 * That's too much, so limit it to 1024 for now...
 */

typedef struct {
    uint64_t start;
    uint64_t end;
} free_memory_t;

#define FREE_MEMORY_ARRAY_COUNT 1024

static free_memory_t free_memory[FREE_MEMORY_ARRAY_COUNT] = { 0 };
static size_t free_memory_entries = 0;
static size_t system_bytes = 0;
static size_t free_bytes = 0;
static size_t used_bytes = 0;
static size_t unavailable_bytes = 0;

void setup_memory_map(multiboot_info_t* mbd)
{
    printf("mmap_length = %uB\n", mbd->mmap_length);
    printf("mmap_addr   = 0x%x\n", mbd->mmap_addr);

    // traverse and print the mmap
    mem_map_entry* mmap;
    for (mmap = (mem_map_entry*)mbd->mmap_addr;
         mmap < (mem_map_entry*)(mbd->mmap_addr + mbd->mmap_length);
         mmap = (mem_map_entry*)((uint32_t)mmap + mmap->size + sizeof(mmap->size))) {
        printf("base_addr = 0x%llx, length = 0x%llx, type=%u\n",
            mmap->addr,
            mmap->len,
            mmap->type);

        // TODO setup pages
        uint64_t start = mmap->addr;
        uint64_t end = start + mmap->len;

        system_bytes += mmap->len;

        switch (mmap->type) {
        case MULTIBOOT_MEMORY_AVAILABLE:
        case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE:
            break;
        case MULTIBOOT_MEMORY_BADRAM:
        case MULTIBOOT_MEMORY_RESERVED:
        case MULTIBOOT_MEMORY_NVS:
        default:
            unavailable_bytes += mmap->len;
            continue;
        }

        if (mmap->type != MULTIBOOT_MEMORY_AVAILABLE) {
            unavailable_bytes += mmap->len;
            continue;
        }

        if (start >= end)
            continue;

        free_memory[free_memory_entries].start = start;
        free_memory[free_memory_entries].end = end;
        ++free_memory_entries;

        free_bytes += mmap->len;
        if (free_memory_entries == FREE_MEMORY_ARRAY_COUNT)
            break;
    }

    used_bytes = system_bytes - free_bytes - unavailable_bytes;

    printf("Initialized Physical Memory System:\n");
    printf("    System Memory: 0x%X (%d MB)\n", system_bytes, system_bytes >> 20);
    printf("    Used Memory  : 0x%X (%d MB)\n", used_bytes, used_bytes >> 20);
    printf("    Free Memory  : 0x%X (%d MB)\n", free_bytes, free_bytes >> 20);
    printf("    Unavailable  : 0x%X (%d MB)\n", unavailable_bytes, unavailable_bytes >> 20);

    printf("Free Memory Entries:\n");
    for (size_t i = 0; i < free_memory_entries; ++i) {
        free_memory_t* entry = &free_memory[i];
        printf("    0x%llx - 0x%llx, %lld B\n", entry->start, entry->end, entry->end - entry->start);
    }
}

uint64_t allocate_pages(size_t num_pages)
{
    // TODO better phys mem allocation
    printf("Allocating %ld pages...\n", num_pages);
    const size_t bytes_size = num_pages * PAGE_SIZE;
    for (size_t i = 0; i < free_memory_entries; ++i) {
        free_memory_t* entry = &free_memory[i];
        if (entry->end - entry->start >= bytes_size) {
            uint64_t pages = entry->start;
            entry->start += bytes_size;
            free_bytes -= bytes_size;
            printf("Allocated %ld pages at 0x%llx!\n", num_pages, pages);
            return pages;
        }
    }

    panic("Could not allocate memory!");
}

void free_pages(uint64_t pages, size_t num_pages)
{
    // TODO
    printf("Freeing %ld pages at 0x%llx...\n", num_pages, pages);
    (void)pages;
    (void)num_pages;
}