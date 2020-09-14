#include <assert.h>
#include <kernel/interrupts.h>
#include <stddef.h>
#include <stdint.h>

#include "interrupt_handlers.h"
#include "intrinsics.h"
#include "pic.h"

typedef struct IDTDescr {
    uint16_t offset_1;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_2;
} __attribute__((packed)) IDTDescr;

static_assert(sizeof(IDTDescr) == 8, "IDTDescr size is not 8!");

// offset 1 and 2 combine to the linear address of the function
// selector is a 16 bit value to a valid GDT descriptor
// zero is always zero

IDTDescr interrupt_table[256];

const size_t NUM_INTERRUPT_DESCRIPTORS = sizeof(interrupt_table) / sizeof(IDTDescr);

void load_descriptor(IDTDescr* descriptor, uint32_t offset, uint16_t selector, uint8_t type)
{
    descriptor->offset_1 = offset & 0xFFFF;
    descriptor->offset_2 = (offset & 0xFFFF0000) >> 16;
    descriptor->selector = selector;
    descriptor->zero = 0;
    descriptor->type_attr = type;
}

// type_attr is
// bit    use
// 7      P (Present)
// 5-6    DPL (Descriptor Privilege Level)
// 4      S (Storage Segment)
// 0-3    GateType

void setup_interrupts()
{
    // Fill interrupt table
    // Exceptions first
    load_descriptor(&interrupt_table[0], (uint32_t)divide_by_zero_error, 0x8, 0x8F);
    load_descriptor(&interrupt_table[1], (uint32_t)debug, 0x8, 0x8F);
    load_descriptor(&interrupt_table[2], (uint32_t)nonmaskable_interrupt, 0x8, 0x8F);
    load_descriptor(&interrupt_table[3], (uint32_t)breakpoint, 0x8, 0x8F);
    load_descriptor(&interrupt_table[4], (uint32_t)overflow, 0x8, 0x8F);
    load_descriptor(&interrupt_table[5], (uint32_t)bound_range_exceeded, 0x8, 0x8F);
    load_descriptor(&interrupt_table[6], (uint32_t)invalid_opcode, 0x8, 0x8F);
    load_descriptor(&interrupt_table[7], (uint32_t)device_not_available, 0x8, 0x8F);
    load_descriptor(&interrupt_table[8], (uint32_t)double_fault, 0x8, 0x8F);
    // No 9
    load_descriptor(&interrupt_table[10], (uint32_t)invalid_tss, 0x8, 0x8F);
    load_descriptor(&interrupt_table[11], (uint32_t)segment_not_present, 0x8, 0x8F);
    load_descriptor(&interrupt_table[12], (uint32_t)stack_segment_fault, 0x8, 0x8F);
    load_descriptor(&interrupt_table[13], (uint32_t)general_protection_fault, 0x8, 0x8F);
    load_descriptor(&interrupt_table[14], (uint32_t)page_fault, 0x8, 0x8F);
    // No 15
    load_descriptor(&interrupt_table[16], (uint32_t)floating_point_exception, 0x8, 0x8F);
    load_descriptor(&interrupt_table[17], (uint32_t)alignment_check, 0x8, 0x8F);
    load_descriptor(&interrupt_table[18], (uint32_t)machine_check, 0x8, 0x8F);
    load_descriptor(&interrupt_table[19], (uint32_t)simd_floating_point_exception, 0x8, 0x8F);
    load_descriptor(&interrupt_table[20], (uint32_t)virtualization_exception, 0x8, 0x8F);
    // No 21-29
    load_descriptor(&interrupt_table[30], (uint32_t)divide_by_zero_error, 0x8, 0x8F);
    // No 31
    // now IRQs
    load_descriptor(&interrupt_table[32], (uint32_t)irq0, 0x8, 0x8E);
    load_descriptor(&interrupt_table[33], (uint32_t)irq1, 0x8, 0x8E);
    load_descriptor(&interrupt_table[34], (uint32_t)irq2, 0x8, 0x8E);
    load_descriptor(&interrupt_table[35], (uint32_t)irq3, 0x8, 0x8E);
    load_descriptor(&interrupt_table[36], (uint32_t)irq4, 0x8, 0x8E);
    load_descriptor(&interrupt_table[37], (uint32_t)irq5, 0x8, 0x8E);
    load_descriptor(&interrupt_table[38], (uint32_t)irq6, 0x8, 0x8E);
    load_descriptor(&interrupt_table[39], (uint32_t)irq7, 0x8, 0x8E);
    load_descriptor(&interrupt_table[40], (uint32_t)irq8, 0x8, 0x8E);
    load_descriptor(&interrupt_table[41], (uint32_t)irq9, 0x8, 0x8E);
    load_descriptor(&interrupt_table[42], (uint32_t)irq10, 0x8, 0x8E);
    load_descriptor(&interrupt_table[43], (uint32_t)irq11, 0x8, 0x8E);
    load_descriptor(&interrupt_table[44], (uint32_t)irq12, 0x8, 0x8E);
    load_descriptor(&interrupt_table[45], (uint32_t)irq13, 0x8, 0x8E);
    load_descriptor(&interrupt_table[46], (uint32_t)irq14, 0x8, 0x8E);
    load_descriptor(&interrupt_table[47], (uint32_t)irq15, 0x8, 0x8E);

    // load table
    lidt((void*)interrupt_table, sizeof(interrupt_table) - 1);
    // Remap PIC
    remap_pic(0x20, 0x28);
    clear_all_irq_masks();
    io_wait();
    set_irq_mask(0); // Disable the timer interrupt for now
    // enable interrupts
    enable_interrupts();
}