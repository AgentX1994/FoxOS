#pragma once
#include <stdbool.h>
#include <stdint.h>

static inline void outb(uint16_t port, uint8_t val)
{
    asm volatile("outb %0, %1"
                 :
                 : "a"(val), "Nd"(port));
}

static inline void outw(uint16_t port, uint16_t val)
{
    asm volatile("outw %0, %1"
                 :
                 : "a"(val), "Nd"(port));
}

static inline void outl(uint16_t port, uint32_t val)
{
    asm volatile("outl %0, %1"
                 :
                 : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile("inb %1, %0"
                 : "=a"(ret)
                 : "Nd"(port));
    return ret;
}

static inline uint16_t inw(uint16_t port)
{
    uint16_t ret;
    asm volatile("inw %1, %0"
                 : "=a"(ret)
                 : "Nd"(port));
    return ret;
}

static inline uint32_t inl(uint16_t port)
{
    uint32_t ret;
    asm volatile("inl %1, %0"
                 : "=a"(ret)
                 : "Nd"(port));
    return ret;
}

static inline void io_wait(void)
{
    /* Port 0x80 is used for 'checkpoints' during POST. */
    /* The Linux kernel seems to think it is free for use :-/ */
    asm volatile("outb %%al, $0x80"
                 :
                 : "a"(0));
    /* %%al instead of %0 makes no difference. TODO: does the register need to be zeroed? */
}

static inline void enable_interrupts(void)
{
    asm volatile("sti");
}

static inline bool are_interrupts_enabled(void)
{
    unsigned long flags;
    asm volatile("pushf\n\t"
                 "pop %0"
                 : "=g"(flags));
    return flags & (1 << 9);
}

static inline unsigned long save_irqdisable(void)
{
    unsigned long flags;
    asm volatile("pushf\n\t"
                 "cli\n\t"
                 "pop %0"
                 : "=r"(flags)
                 :
                 : "memory");
    return flags;
}

static inline void irqrestore(unsigned long flags)
{
    asm volatile("push %0\n\t"
                 "popf"
                 :
                 : "rm"(flags)
                 : "memory", "cc");
}

static inline void lgdt(void* base, uint16_t size)
{
    struct {
        uint16_t length;
        void* base;
    } __attribute__((packed)) GDTR = { size, base };

    asm volatile("lgdt %0"
                 :
                 : "m"(GDTR));
}

static inline void lidt(void* base, uint16_t size)
{
    struct {
        uint16_t length;
        void* base;
    } __attribute__((packed)) IDTR = { size, base };

    asm volatile("lidt %0"
                 :
                 : "m"(IDTR));
}

static inline uint64_t rdtsc(void)
{
    uint64_t ret;
    asm volatile("rdtsc"
                 : "=A"(ret));
    return ret;
}

// defined in boot.S
extern void set_cs(uint32_t v);

static inline void set_ds(uint32_t v)
{
    asm volatile("mov %0, %%ds"
                 :
                 : "r"(v));
}

static inline void set_es(uint32_t v)
{
    asm volatile("mov %0, %%es"
                 :
                 : "r"(v));
}

static inline void set_fs(uint32_t v)
{
    asm volatile("mov %0, %%fs"
                 :
                 : "r"(v));
}

static inline void set_gs(uint32_t v)
{
    asm volatile("mov %0, %%gs"
                 :
                 : "r"(v));
}

static inline void set_ss(uint32_t v)
{
    asm volatile("mov %0, %%ss"
                 :
                 : "r"(v));
}