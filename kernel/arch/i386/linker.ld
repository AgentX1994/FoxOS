
/* Tell where the various sections of the object files will be put 
 * in the final kernel image
 */
PHYS = 0x00100000;
VIRT_OFFSET = 0xC0000000;

/* Set the start point, where the bootloader will hand over control */
ENTRY(_start)
SECTIONS
{
    . = PHYS + VIRT_OFFSET;
    /* The kernel will live at 3GB + 1MB in the virtual address space.
     * which will be mapped to 1 MB in the physical address space
     * page align the sections
     */

    /* First put the multiboot header, as it is required to be put very
     * early in the image or the bootloader won't recognize the file 
     * format. Next we'll put the .text section.
     */
    /*
    .multiboot : AT (ADDR (.multiboot) ) {
        *(.multiboot.data)
        *(.multiboot.text)
    }
    */

    _kernel_start = .;

    .text ALIGN(4K) : AT (ADDR (.text) - VIRT_OFFSET)
    {
        *(.multiboot.text)
        *(.multiboot.data)
        *(.text)
    }

    /* Read-only data. */
    .rodata ALIGN(4K) : AT (ADDR (.rodata) - VIRT_OFFSET)
    {
        *(.rodata)
    }

    /* Read-write data (initialized) */
    .data ALIGN(4K) : AT (ADDR (.data) - VIRT_OFFSET)
    {
        *(.data)
    }

    /* Read-write data (uninitialized and stack */
    .bss ALIGN(4K) : AT (ADDR (.bss) - VIRT_OFFSET)
    {
        *(COMMON)
        *(.bss)
        *(.bootstrap_stack)
    }
    _kernel_end = .;

    /* The compiler may produce other sections, by default it
     * will put them in a segment with the same name. Simply
     * add stuff here as needed. */
}
