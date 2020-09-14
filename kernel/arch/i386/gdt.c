#include "gdt.h"
#include <stdint.h>

#include "intrinsics.h"

// Each define here is for a specific flag in the descriptor.
// Refer to the intel documentation for a description of what each one does.
#define SEG_DESCTYPE(x) ((x) << 0x04) // Descriptor type (0 for system, 1 for code/data)
#define SEG_PRES(x) ((x) << 0x07) // Present
#define SEG_SAVL(x) ((x) << 0x0C) // Available for system use
#define SEG_LONG(x) ((x) << 0x0D) // Long mode
#define SEG_SIZE(x) ((x) << 0x0E) // Size (0 for 16-bit, 1 for 32)
#define SEG_GRAN(x) ((x) << 0x0F) // Granularity (0 for 1B - 1MB, 1 for 4KB - 4GB)
#define SEG_PRIV(x) (((x)&0x03) << 0x05) // Set privilege level (0 - 3)

#define SEG_DATA_RD 0x00 // Read-Only
#define SEG_DATA_RDA 0x01 // Read-Only, accessed
#define SEG_DATA_RDWR 0x02 // Read/Write
#define SEG_DATA_RDWRA 0x03 // Read/Write, accessed
#define SEG_DATA_RDEXPD 0x04 // Read-Only, expand-down
#define SEG_DATA_RDEXPDA 0x05 // Read-Only, expand-down, accessed
#define SEG_DATA_RDWREXPD 0x06 // Read/Write, expand-down
#define SEG_DATA_RDWREXPDA 0x07 // Read/Write, expand-down, accessed
#define SEG_CODE_EX 0x08 // Execute-Only
#define SEG_CODE_EXA 0x09 // Execute-Only, accessed
#define SEG_CODE_EXRD 0x0A // Execute/Read
#define SEG_CODE_EXRDA 0x0B // Execute/Read, accessed
#define SEG_CODE_EXC 0x0C // Execute-Only, conforming
#define SEG_CODE_EXCA 0x0D // Execute-Only, conforming, accessed
#define SEG_CODE_EXRDC 0x0E // Execute/Read, conforming
#define SEG_CODE_EXRDCA 0x0F // Execute/Read, conforming, accessed

#define GDT_CODE_PL0  \
    SEG_DESCTYPE(1)   \
    | SEG_PRES(1)     \
        | SEG_SAVL(0) \
        | SEG_LONG(0) \
        | SEG_SIZE(1) \
        | SEG_GRAN(1) \
        | SEG_PRIV(0) \
        | SEG_CODE_EXRD

#define GDT_DATA_PL0  \
    SEG_DESCTYPE(1)   \
    | SEG_PRES(1)     \
        | SEG_SAVL(0) \
        | SEG_LONG(0) \
        | SEG_SIZE(1) \
        | SEG_GRAN(1) \
        | SEG_PRIV(0) \
        | SEG_DATA_RDWR

#define GDT_CODE_PL3  \
    SEG_DESCTYPE(1)   \
    | SEG_PRES(1)     \
        | SEG_SAVL(0) \
        | SEG_LONG(0) \
        | SEG_SIZE(1) \
        | SEG_GRAN(1) \
        | SEG_PRIV(3) \
        | SEG_CODE_EXRD

#define GDT_DATA_PL3  \
    SEG_DESCTYPE(1)   \
    | SEG_PRES(1)     \
        | SEG_SAVL(0) \
        | SEG_LONG(0) \
        | SEG_SIZE(1) \
        | SEG_GRAN(1) \
        | SEG_PRIV(3) \
        | SEG_DATA_RDWR

uint64_t
create_descriptor(uint32_t base, uint32_t limit, uint16_t flag)
{
    uint64_t descriptor;

    // Create the high 32 bit segment
    descriptor = limit & 0x000F0000; // set limit bits 19:16
    descriptor |= (flag << 8) & 0x00F0FF00; // set type, p, dpl, s, g, d/b, l and avl fields
    descriptor |= (base >> 16) & 0x000000FF; // set base bits 23:16
    descriptor |= base & 0xFF000000; // set base bits 31:24

    // Shift by 32 to allow for low part of segment
    descriptor <<= 32;

    // Create the low 32 bit segment
    descriptor |= base << 16; // set base bits 15:0
    descriptor |= limit & 0x0000FFFF; // set limit bits 15:0

    return descriptor;
}

// The actual table
uint64_t GDT[4];

// TODO find a place for the TSS stuff
typedef volatile struct __tss_struct {
    uint16_t link;
    uint16_t link_h;

    uint32_t esp0;
    uint16_t ss0;
    uint16_t ss0_h;

    uint32_t esp1;
    uint16_t ss1;
    uint16_t ss1_h;

    uint32_t esp2;
    uint16_t ss2;
    uint16_t ss2_h;

    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;

    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;

    uint32_t esp;
    uint32_t ebp;

    uint32_t esi;
    uint32_t edi;

    uint16_t es;
    uint16_t es_h;

    uint16_t cs;
    uint16_t cs_h;

    uint16_t ss;
    uint16_t ss_h;

    uint16_t ds;
    uint16_t ds_h;

    uint16_t fs;
    uint16_t fs_h;

    uint16_t gs;
    uint16_t gs_h;

    uint16_t ldt;
    uint16_t ldt_h;

    uint16_t trap;
    uint16_t iomap;

} tss_struct;

tss_struct myTss;

uint8_t system_call_stack[4096];

void setup_gdt(void)
{
    // setup kernel TSS
    // SS0 = 0x10 (see below)
    // ESP0 = ?
    // IOPB = sizeof(TSS)
    myTss.ss0 = 0x10;
    myTss.esp0 = (uint32_t)system_call_stack;
    myTss.iomap = (uint16_t)sizeof(myTss);

    // TODO setup a real gdt
    // For now, just have no limits
    GDT[0] = create_descriptor(0, 0, 0); // 0x00 selector cannot be used
    GDT[1] = create_descriptor(0, 0xFFFFFFFF, GDT_CODE_PL0); // 0x08 kernel code segment
    GDT[2] = create_descriptor(0, 0xFFFFFFFF, GDT_DATA_PL0); // 0x10 kernel data
    GDT[3] = create_descriptor((uint32_t)&myTss, sizeof(myTss), 0x89); // 0x18 kernel TSS

    lgdt((void*)GDT, sizeof(GDT) - 1);

    // Set segment registers
    uint16_t segment = 0x10;
    set_cs(0x8);
    set_ds(segment);
    set_es(segment);
    set_fs(segment);
    set_gs(segment);
    set_ss(segment);
}