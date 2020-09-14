#include "interrupt_handlers.h"

#include <kernel/tty.h>
#include <stdio.h>

#include "intrinsics.h"
#include "pic.h"

// Exceptions
INTERRUPT void divide_by_zero_error(struct interrupt_frame* frame)
{
    terminal_writestring("Exception: ");
    terminal_writestring(__PRETTY_FUNCTION__);
    terminal_writestring("\n");
}

INTERRUPT void debug(struct interrupt_frame* frame)
{
    terminal_writestring("Exception: ");
    terminal_writestring(__PRETTY_FUNCTION__);
    terminal_writestring("\n");
}

INTERRUPT void breakpoint(struct interrupt_frame* frame)
{
    terminal_writestring("Exception: ");
    terminal_writestring(__PRETTY_FUNCTION__);
    terminal_writestring("\n");
}

INTERRUPT void nonmaskable_interrupt(struct interrupt_frame* frame)
{
    terminal_writestring("Exception: ");
    terminal_writestring(__PRETTY_FUNCTION__);
    terminal_writestring("\n");
}

INTERRUPT void overflow(struct interrupt_frame* frame)
{
    terminal_writestring("Exception: ");
    terminal_writestring(__PRETTY_FUNCTION__);
    terminal_writestring("\n");
}

INTERRUPT void bound_range_exceeded(struct interrupt_frame* frame)
{
    terminal_writestring("Exception: ");
    terminal_writestring(__PRETTY_FUNCTION__);
    terminal_writestring("\n");
}

INTERRUPT void invalid_opcode(struct interrupt_frame* frame)
{
    terminal_writestring("Exception: ");
    terminal_writestring(__PRETTY_FUNCTION__);
    terminal_writestring("\n");
}

INTERRUPT void device_not_available(struct interrupt_frame* frame)
{
    terminal_writestring("Exception: ");
    terminal_writestring(__PRETTY_FUNCTION__);
    terminal_writestring("\n");
}

INTERRUPT void double_fault(struct interrupt_frame* frame, uint32_t error_code)
{
    terminal_writestring("Exception: ");
    terminal_writestring(__PRETTY_FUNCTION__);
    terminal_writestring("\n");
}

INTERRUPT void invalid_tss(struct interrupt_frame* frame, uint32_t error_code)
{
    terminal_writestring("Exception: ");
    terminal_writestring(__PRETTY_FUNCTION__);
    terminal_writestring("\n");
}

INTERRUPT void segment_not_present(struct interrupt_frame* frame, uint32_t error_code)
{
    terminal_writestring("Exception: ");
    terminal_writestring(__PRETTY_FUNCTION__);
    terminal_writestring("\n");
}

INTERRUPT void stack_segment_fault(struct interrupt_frame* frame, uint32_t error_code)
{
    terminal_writestring("Exception: ");
    terminal_writestring(__PRETTY_FUNCTION__);
    terminal_writestring("\n");
}

INTERRUPT void general_protection_fault(struct interrupt_frame* frame, uint32_t error_code)
{
    terminal_writestring("Exception: ");
    terminal_writestring(__PRETTY_FUNCTION__);
    terminal_writestring("\n");
}

INTERRUPT void page_fault(struct interrupt_frame* frame, uint32_t error_code)
{
    terminal_writestring("Exception: ");
    terminal_writestring(__PRETTY_FUNCTION__);
    terminal_writestring("\n");
}

INTERRUPT void floating_point_exception(struct interrupt_frame* frame)
{
    terminal_writestring("Exception: ");
    terminal_writestring(__PRETTY_FUNCTION__);
    terminal_writestring("\n");
}

INTERRUPT void alignment_check(struct interrupt_frame* frame, uint32_t error_code)
{
    terminal_writestring("Exception: ");
    terminal_writestring(__PRETTY_FUNCTION__);
    terminal_writestring("\n");
}

INTERRUPT void machine_check(struct interrupt_frame* frame)
{
    terminal_writestring("Exception: ");
    terminal_writestring(__PRETTY_FUNCTION__);
    terminal_writestring("\n");
}

INTERRUPT void simd_floating_point_exception(struct interrupt_frame* frame)
{
    terminal_writestring("Exception: ");
    terminal_writestring(__PRETTY_FUNCTION__);
    terminal_writestring("\n");
}

INTERRUPT void virtualization_exception(struct interrupt_frame* frame)
{
    terminal_writestring("Exception: ");
    terminal_writestring(__PRETTY_FUNCTION__);
    terminal_writestring("\n");
}

INTERRUPT void security_exception(struct interrupt_frame* frame, uint32_t error_code)
{
    terminal_writestring("Exception: ");
    terminal_writestring(__PRETTY_FUNCTION__);
    terminal_writestring("\n");
}

// IRQs
INTERRUPT void irq0(struct interrupt_frame* frame)
{
    terminal_writestring("IRQ fired: ");
    terminal_writestring(__func__);
    terminal_writestring("\n");

    send_eoi_to_pic(0);
}

INTERRUPT void irq1(struct interrupt_frame* frame)
{
    terminal_writestring("IRQ fired: ");
    terminal_writestring(__func__);
    terminal_writestring("\n");

    // This is the keyboard interrupt, have to make sure
    // to grab the character data from the keyboard
    uint8_t scancode = inb(0x60);
    printf("Key typed: %2X\n", scancode);

    send_eoi_to_pic(1);
}

INTERRUPT void irq2(struct interrupt_frame* frame)
{
    terminal_writestring("IRQ fired: ");
    terminal_writestring(__func__);
    terminal_writestring("\n");

    send_eoi_to_pic(2);
}

INTERRUPT void irq3(struct interrupt_frame* frame)
{
    terminal_writestring("IRQ fired: ");
    terminal_writestring(__func__);
    terminal_writestring("\n");

    send_eoi_to_pic(3);
}

INTERRUPT void irq4(struct interrupt_frame* frame)
{
    terminal_writestring("IRQ fired: ");
    terminal_writestring(__func__);
    terminal_writestring("\n");

    send_eoi_to_pic(4);
}

INTERRUPT void irq5(struct interrupt_frame* frame)
{
    terminal_writestring("IRQ fired: ");
    terminal_writestring(__func__);
    terminal_writestring("\n");

    send_eoi_to_pic(5);
}

INTERRUPT void irq6(struct interrupt_frame* frame)
{
    terminal_writestring("IRQ fired: ");
    terminal_writestring(__func__);
    terminal_writestring("\n");

    send_eoi_to_pic(6);
}

INTERRUPT void irq7(struct interrupt_frame* frame)
{
    terminal_writestring("IRQ fired: ");
    terminal_writestring(__func__);
    terminal_writestring("\n");

    // check if spurious
    uint16_t isr = get_isr_pic();
    if (isr & (1 << 7)) {
        // wasn't spurious
        terminal_writestring("    not spurious!\n");
        send_eoi_to_pic(7);
    } else {
        terminal_writestring("    spurious!\n");
    }
}

INTERRUPT void irq8(struct interrupt_frame* frame)
{
    terminal_writestring("IRQ fired: ");
    terminal_writestring(__func__);
    terminal_writestring("\n");

    send_eoi_to_pic(8);
}

INTERRUPT void irq9(struct interrupt_frame* frame)
{
    terminal_writestring("IRQ fired: ");
    terminal_writestring(__func__);
    terminal_writestring("\n");

    send_eoi_to_pic(9);
}

INTERRUPT void irq10(struct interrupt_frame* frame)
{
    terminal_writestring("IRQ fired: ");
    terminal_writestring(__func__);
    terminal_writestring("\n");

    send_eoi_to_pic(10);
}

INTERRUPT void irq11(struct interrupt_frame* frame)
{
    terminal_writestring("IRQ fired: ");
    terminal_writestring(__func__);
    terminal_writestring("\n");

    send_eoi_to_pic(11);
}

INTERRUPT void irq12(struct interrupt_frame* frame)
{
    terminal_writestring("IRQ fired: ");
    terminal_writestring(__func__);
    terminal_writestring("\n");

    send_eoi_to_pic(12);
}

INTERRUPT void irq13(struct interrupt_frame* frame)
{
    terminal_writestring("IRQ fired: ");
    terminal_writestring(__func__);
    terminal_writestring("\n");

    send_eoi_to_pic(13);
}

INTERRUPT void irq14(struct interrupt_frame* frame)
{
    terminal_writestring("IRQ fired: ");
    terminal_writestring(__func__);
    terminal_writestring("\n");

    send_eoi_to_pic(14);
}

INTERRUPT void irq15(struct interrupt_frame* frame)
{
    terminal_writestring("IRQ fired: ");
    terminal_writestring(__func__);
    terminal_writestring("\n");

    // check if spurious
    uint16_t isr = get_isr_pic();
    if (isr & (1 << 15)) {
        // wasn't spurious
        terminal_writestring("    not spurious!\n");
        send_eoi_to_pic(15);
    } else {
        terminal_writestring("    spurious!\n");
        // only send eoi to the master, since it doesn't
        // know it was spurious
        // The number doesn't matter too much, as long
        // as it's an IRQ that the master PIC services
        send_eoi_to_pic(7);
    }
}
