#include <kernel/multiboot.h>
#include <stdint.h>
#include <string.h>

// Copies the multiboot_info_t and all structures in it
// to the destination buffer
// Note: This is called in the init stage *before* any
// global constructors are called, and *before* paging
// is enabled
// This only exists because I was too lazy to figure
// out how to write it in assembly
void copy_multiboot_info(multiboot_info_t* mbd, char* dest, uint32_t virtual_offset)
{
    size_t bytes_used = 0;
    // copy the main structure
    memcpy(dest, mbd, sizeof(multiboot_info_t));
    bytes_used += sizeof(multiboot_info_t);

    // Copy additional structures if needed

    if (mbd->flags & MULTIBOOT_INFO_CMDLINE) {
        strcpy(dest + bytes_used, mbd->cmdline);
        // update address in new multiboot structure
        ((multiboot_info_t*)dest)->cmdline = dest + bytes_used + virtual_offset;
        bytes_used += strlen(mbd->cmdline) + 1; // include null terminator
    }

    if (mbd->flags & MULTIBOOT_INFO_MODS) {
        // copy the modules themselves
        // TODO how to handle the addresses of the modules??
        memcpy(dest + bytes_used, mbd->mods_addr, mbd->mods_count * sizeof(multiboot_module_t));
        // update address in new multiboot structure
        ((multiboot_info_t*)dest)->mods_addr = dest + bytes_used + virtual_offset;
        // Copy each modules' command line
        multiboot_module_t* modules = dest + bytes_used;
        bytes_used += mbd->mods_count * sizeof(multiboot_module_t);
        for (size_t mod_num = 0; mod_num < mbd->mods_count; ++mod_num) {
            strcpy(dest + bytes_used, modules[mod_num].cmdline);
            modules[mod_num].cmdline = dest + bytes_used + virtual_offset;
            bytes_used += strlen(modules[mod_num].cmdline) + 1; // include null terminator
        }
    }

    if (mbd->flags & MULTIBOOT_INFO_AOUT_SYMS) {
        // TODO
        // This shouldn't be true since FoxOS uses ELF format
    }

    if (mbd->flags & MULTIBOOT_INFO_ELF_SHDR) {
        // TODO
    }

    if (mbd->flags & MULTIBOOT_INFO_MEM_MAP) {
        // The length is the total length of the buffer,
        // so we don't need to worry about all the individual entries
        memcpy(dest + bytes_used, mbd->mmap_addr, mbd->mmap_length);
        // update address in new multiboot structure
        ((multiboot_info_t*)dest)->mmap_addr = dest + bytes_used + virtual_offset;
        bytes_used += mbd->mmap_length;
    }

    // TODO: AOUT_SYMS, ELF_SHDR, DRIVES, CONFIG, BOOT_LOADER_NAME, APM_TABLE, VBE, FRAMEBUFFER
}