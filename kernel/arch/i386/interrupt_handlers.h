#pragma once

#include <stddef.h>
#include <stdint.h>

#define INTERRUPT __attribute__((interrupt))

struct interrupt_frame {
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t sp;
    uint32_t ss;
};

/*
 * Exceptions
 */
INTERRUPT void divide_by_zero_error(struct interrupt_frame* frame);
INTERRUPT void debug(struct interrupt_frame* frame);
INTERRUPT void breakpoint(struct interrupt_frame* frame);
INTERRUPT void nonmaskable_interrupt(struct interrupt_frame* frame);
INTERRUPT void overflow(struct interrupt_frame* frame);
INTERRUPT void bound_range_exceeded(struct interrupt_frame* frame);
INTERRUPT void invalid_opcode(struct interrupt_frame* frame);
INTERRUPT void device_not_available(struct interrupt_frame* frame);
INTERRUPT void double_fault(struct interrupt_frame* frame, uint32_t error_code);
// Vector #9 is no longer used
//INTERRUPT void coprocessor_segment_overrun(struct interrupt_frame* frame);
INTERRUPT void invalid_tss(struct interrupt_frame* frame, uint32_t error_code);
INTERRUPT void segment_not_present(struct interrupt_frame* frame, uint32_t error_code);
INTERRUPT void stack_segment_fault(struct interrupt_frame* frame, uint32_t error_code);
INTERRUPT void general_protection_fault(struct interrupt_frame* frame, uint32_t error_code);
INTERRUPT void page_fault(struct interrupt_frame* frame, uint32_t error_code);
// Vector #15 is reserved
INTERRUPT void floating_point_exception(struct interrupt_frame* frame);
INTERRUPT void alignment_check(struct interrupt_frame* frame, uint32_t error_code);
INTERRUPT void machine_check(struct interrupt_frame* frame);
INTERRUPT void simd_floating_point_exception(struct interrupt_frame* frame);
INTERRUPT void virtualization_exception(struct interrupt_frame* frame);
// Vector #21-29 are reserved
INTERRUPT void security_exception(struct interrupt_frame* frame, uint32_t error_code);
// Vector #31 is reserved
// IRQ #13 is no longer used
// INTERRUPT void fpu_error_interrupt(struct interrupt_frame* frame);

/*
 * IRQs
 */
INTERRUPT void irq0(struct interrupt_frame* frame);
INTERRUPT void irq1(struct interrupt_frame* frame);
INTERRUPT void irq2(struct interrupt_frame* frame);
INTERRUPT void irq3(struct interrupt_frame* frame);
INTERRUPT void irq4(struct interrupt_frame* frame);
INTERRUPT void irq5(struct interrupt_frame* frame);
INTERRUPT void irq6(struct interrupt_frame* frame);
INTERRUPT void irq7(struct interrupt_frame* frame);
INTERRUPT void irq8(struct interrupt_frame* frame);
INTERRUPT void irq9(struct interrupt_frame* frame);
INTERRUPT void irq10(struct interrupt_frame* frame);
INTERRUPT void irq11(struct interrupt_frame* frame);
INTERRUPT void irq12(struct interrupt_frame* frame);
INTERRUPT void irq13(struct interrupt_frame* frame);
INTERRUPT void irq14(struct interrupt_frame* frame);
INTERRUPT void irq15(struct interrupt_frame* frame);