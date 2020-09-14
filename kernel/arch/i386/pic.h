#pragma once
#include <stdint.h>

/*
 * Remaps the PIC interrupts vectors
 * arguments:
 *     master_offset - the offset to use for the master PIC
 *     slave_offset  - the offset to use for the slave PIC
 */
void remap_pic(uint8_t master_offset, uint8_t slave_offset);

/*
 * Send the EOI for IRQ number
 */
void send_eoi_to_pic(uint8_t irq);

/*
 * Get the combined value of the cascaded PICs irq request register
 */
uint16_t get_irr_pic(void);
/*
 * Get the combined value of the cascaded PICs irq in service register
 */
uint16_t get_isr_pic(void);

/*
 * Mask the given irqline to stop receiving that irq
 */
void set_irq_mask(uint8_t irqline);

/*
 * Clear a mask on the given irqline to start receiving that irq
 */
void clear_irq_mask(uint8_t irqline);

/*
 * Clear the mask on all irqlines
 */
void clear_all_irq_masks(void);