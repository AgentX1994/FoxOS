/* Constants for multiboot header */
.set ALIGN, 1<<0 /* align loaded modules on page boundaries */
.set MEMINFO, 1<<1 /* provide memory map */
.set FLAGS, ALIGN | MEMINFO /* This is the multiboot flag field */
.set MAGIC, 0x1BADB002 /* magic number to let bootloader find the header */
.set CHECKSUM, -(MAGIC + FLAGS) /* Checksum of magic and flags, proof of multiboot */

/*
Declare the multiboot header that marks the program as a kernel. These
are magic values that ar documented in the multiboot standard.
The bootloader will search for this signature in the first 8 KiB of the kernel file,
aligned at a 32-bit boundary. THe signature is in its own section so it can be forced
to be in this range
*/
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

/*
The multiboot standard does not define the value of esp, so we provide a stack.
Create a symbol at the bottom, allocate 16384 bytes, then create a symbol at the top.
We mark this section as nobits so it is not in the executable. It must be 16 byte aligned, 
according to System V ABI standard.
*/
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

/* Linker script specifies _start as the entry point, and the bootloader will jump here
once the kernel has been loaded. No point in returning.
*/
.section .text
.global _start
.type _start, @function
_start:
    /*
    Current state: 32-bit protected mode, disabled interrupts, disabled paging.
    Full control of the cpu. No libraries, all code must be provided by the kernel.
    No security restrictions, no safeguards, no debugging mechanisms.
    */
    /* setup the stack */
    mov $stack_top, %esp

    /*
    Push multiboot address and magic onto stack
    */
    push %eax
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
 Set the size of the _start symbol
*/
.size _start, . - _start
