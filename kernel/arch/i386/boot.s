# 1 "kernel/arch/i386/boot.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "kernel/arch/i386/boot.S"

.set ALIGN, 1<<0
.set MEMINFO, 1<<1
.set FLAGS, ALIGN | MEMINFO
.set MAGIC, 0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)
# 15 "kernel/arch/i386/boot.S"
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM







.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:




.section .text
.global _start
.type _start, @function
_start:






    mov $stack_top, %esp
# 60 "kernel/arch/i386/boot.S"
    call _init





    call kernel_main




    call _fini







    cli
1: hlt
    jmp 1b




.size _start, . - _start
