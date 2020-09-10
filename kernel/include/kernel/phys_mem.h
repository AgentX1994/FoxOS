#pragma once

#include <kernel/multiboot.h>
#include <stddef.h>
#include <stdint.h>

#define PAGE_SIZE 4096

void setup_memory_map(multiboot_info_t* mmap);

uint64_t allocate_pages(size_t num_pages);

void free_pages(uint64_t pages, size_t num_pages);