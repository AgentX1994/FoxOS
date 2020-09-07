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
.section .multiboot.data
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

