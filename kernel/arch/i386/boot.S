/*
The multiboot standard does not define the value of esp, so we provide a stack.
Create a symbol at the bottom, allocate 16384 bytes, then create a symbol at the top.
We mark this section as nobits so it is not in the executable. It must be 16 byte aligned, 
according to System V ABI standard.
*/
.section .bootstrap_stack, "aw", @nobits
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

#define VIRT_OFFSET 0xC0000000

/*
 * Preallocate pages used for paging. Don't hard-code addresses and assume they
 * are available, as the bootloader might have loaded its multiboot structures or
 * modules there. This lets the bootloader know it must avoid the addresses.
 */
.section .bss, "aw", @nobits
	.align 4096
# Create a starting heap, only really used for relocating the
# Multiboot info structure from GRUB
init_heap:
    .skip 4096
boot_page_directory:
	.skip 4096
boot_page_table1:
	.skip 4096
# Further page tables may be required if the kernel grows beyond 3 MiB.

/* Linker script specifies _start as the entry point, and the bootloader will jump here
once the kernel has been loaded. No point in returning.
*/
.section .multiboot.text
.global _start
.type _start, @function
_start:
    /* setup the initial stack */
    mov $(stack_top - VIRT_OFFSET), %esp
    # Copy the multiboot info (pointed to by ebx) into the init_heap so that
    # it can still be read after paging is enabled
    # Save MAGIC
    push %eax
    # Setup parameters
    push $VIRT_OFFSET
    mov $(init_heap - VIRT_OFFSET), %ecx
    push %ecx
    push %ebx
    # This copies the multiboot structure into our init heap for easy access later
    call copy_multiboot_info
    # Set %eax and %ebx to the correct values for kernel main later
    # magic
    # Repop function arguments
    pop %ebx
    pop %ebx
    pop %eax
    mov $init_heap, %ebx

    /*
    Current state: 32-bit protected mode, disabled interrupts, disabled paging.
    Full control of the cpu. No libraries, all code must be provided by the kernel.
    No security restrictions, no safeguards, no debugging mechanisms.
    */
    /*
     * Physical address of boot_page_table1.
	 * TODO: I recall seeing some assembly that used a macro to do the
	 *       conversions to and from physical. Maybe this should be done in this
	 *       code as well?
     */
	movl $(boot_page_table1 - VIRT_OFFSET), %edi
	/*
     * First address to map is address 0.
	 * TODO: Start at the first kernel page instead. Alternatively map the first
	 *       1 MiB as it can be generally useful, and there's no need to
	 *       specially map the VGA buffer.
     */
	movl $0, %esi
	# Map 1023 pages. The 1024th will be the VGA text buffer.
	movl $1023, %ecx

1:
	# Only map the kernel.
	cmpl $(_kernel_start - VIRT_OFFSET), %esi
	jl 2f
	cmpl $(_kernel_end - VIRT_OFFSET), %esi
	jge 3f

	# Map physical address as "present, writable". Note that this maps
	# .text and .rodata as writable. Mind security and map them as non-writable.
	movl %esi, %edx
	orl $0x003, %edx
	movl %edx, (%edi)

2:
	# Size of page is 4096 bytes.
	addl $4096, %esi
	# Size of entries in boot_page_table1 is 4 bytes.
	addl $4, %edi
	# Loop to the next entry if we haven't finished.
	loop 1b

3:
	# Map VGA video memory to 0xC03FF000 as "present, writable".
	movl $(0x000B8000 | 0x003), boot_page_table1 - VIRT_OFFSET + 1023 * 4

	# The page table is used at both page directory entry 0 (virtually from 0x0
	# to 0x3FFFFF) (thus identity mapping the kernel) and page directory entry
	# 768 (virtually from 0xC0000000 to 0xC03FFFFF) (thus mapping it in the
	# higher half). The kernel is identity mapped because enabling paging does
	# not change the next instruction, which continues to be physical. The CPU
	# would instead page fault if there was no identity mapping.

	# Map the page table to both virtual addresses 0x00000000 and 0xC0000000.
	movl $(boot_page_table1 - VIRT_OFFSET + 0x003), boot_page_directory - VIRT_OFFSET + 0
	movl $(boot_page_table1 - VIRT_OFFSET + 0x003), boot_page_directory - VIRT_OFFSET + 768 * 4

	# Set cr3 to the address of the boot_page_directory.
	movl $(boot_page_directory - VIRT_OFFSET), %ecx
	movl %ecx, %cr3

	# Enable paging and the write-protect bit.
	movl %cr0, %ecx
	orl $0x80010000, %ecx
	movl %ecx, %cr0

	# Jump to higher half with an absolute jump. 
	lea JumpToHigherHalf, %ecx
	jmp *%ecx

/*
 Set the size of the _start symbol
*/
.size _start, . - _start

.section .text
.global JumpToHigherHalf
.type JumpToHigherHalf, @function
JumpToHigherHalf:
    # Paging is fully set up and enabled
    # Unmap the identity mapping
    movl $0, boot_page_directory + 0
    
    # Reload cr3 to force a TLB flush
    movl %cr3, %ecx
    movl %ecx, %cr3

    /* setup the stack */
    mov $stack_top, %esp

    /*
    Push multiboot address and magic onto stack
    */
    push %eax
    # Add virtual offset to multiboot info
    # add $VIRT_OFFSET, %ebx
    push %ebx

    /*
    Initialize critical processor state before high-level kernel is entered.
    Best practice is to minimize the early environment. The processor is not
    yet fully initialized: Features such as floating point instructions and instruction
    set extensions are not initialized yet. The GDT should be loaded here. Paging should
    be enabled here. C++ features such as global constructors and exceptions will require
    runtime support to work as well.
    */
    
    /*
    Call the _init function to run global constructors
    */
    call _init
    /*
    Enter the high-level kernel. The ABI requires the stack is 16-byte aligned at the time
    of the call instruction (which pushes the returnpointer). It started 16-byte aligned,
    and we've pushed 0 * 16 bytes, so it is still aligned.
    */
    call kernel_main

    /*
    Call the _fini function to run global destructors
    */
    call _fini

    /*
    If the system has nothing more to do, put the computer into an infinite loop.
    1) disable interrupts with cli
    2) wait for interrupt with hlt
    3) jump to hlt if it ever wakes up
    */
    cli
1:  hlt
    jmp 1b

/*
 Set the size of the JumpToHigherHalf symbol
*/
.size JumpToHigherHalf, . - JumpToHigherHalf

# Helper functions for the rest of the kernel
.global set_cs
.type set_cs, @function
set_cs:
    mov 4(%esp), %eax
    pushl %eax
    push $reload_CS
    ljmp *(%esp)
reload_CS:
    add $8, %esp
    ret

.size set_cs, . - set_cs