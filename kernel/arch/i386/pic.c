#include "pic.h"

#include "intrinsics.h"

#define PIC1 0x20 /* IO base address for master PIC */
#define PIC2 0xA0 /* IO base address for slave PIC */
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2 + 1)

#define PIC_EOI 0x20 /* End of interrupt command code */
#define PIC_READ_IRR 0x0a /* Get IRR value */
#define PIC_READ_ISR 0x0b /* Get ISR value */

#define ICW1_ICW4 0x01 /* ICW4 (not) needed */
#define ICW1_SINGLE 0x02 /* Single (cascade) mode */
#define ICW1_INTERVAL4 0x04 /* Call address interval 4 (8) */
#define ICW1_LEVEL 0x08 /* Level triggered (edge) mode */
#define ICW1_INIT 0x10 /* Initialization - required! */

#define ICW4_8086 0x01 /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO 0x02 /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE 0x08 /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C /* Buffered mode/master */
#define ICW4_SFNM 0x10 /* Special fully nested (not) */

void remap_pic(uint8_t master_offset, uint8_t slave_offset)
{
    unsigned char a1, a2;

    // Save masks
    a1 = inb(PIC1_DATA);
    a2 = inb(PIC2_DATA);

    // Start the initialization sequence in cascade mode
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    // ICW2: Master PIC vector offset
    outb(PIC1_DATA, master_offset);
    io_wait();

    // ICW2: Slave PIC vector offset
    outb(PIC2_DATA, slave_offset);
    io_wait();

    // ICW3: Tell master PIC that there is a slave PIC at IRQ2 (0000 0100)
    outb(PIC1_DATA, 4);
    io_wait();

    // ICW3: Tell slave PIC its cascade identity
    outb(PIC2_DATA, 2);
    io_wait();

    // Set mode
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    // restore saved masks
    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
}

void send_eoi_to_pic(uint8_t irq)
{
    if (irq >= 8)
        outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}

static uint16_t get_irq_reg_pic(int8_t reg)
{
    /* OCW3 to PIC CMD to get the register values
     * PIC2 is chained and represents IRQs 8-15
     */
    outb(PIC1_COMMAND, reg);
    outb(PIC2_COMMAND, reg);
    return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}

uint16_t get_irr_pic(void)
{
    return get_irq_reg_pic(PIC_READ_IRR);
}

uint16_t get_isr_pic(void)
{
    return get_irq_reg_pic(PIC_READ_ISR);
}

void set_irq_mask(uint8_t irqline)
{
    uint16_t port;
    if (irqline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irqline -= 8;
    }

    uint8_t value = inb(port) | (1 << irqline);
    outb(port, value);
}

void clear_irq_mask(uint8_t irqline)
{
    uint16_t port;
    if (irqline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irqline -= 8;
    }

    uint8_t value = inb(port) & ~(1 << irqline);
    outb(port, value);
}

void clear_all_irq_masks(void)
{
    // Just set both masks to 0
    outb(PIC1_DATA, 0);
    outb(PIC2_DATA, 0);
}