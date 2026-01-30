#ifndef PIC_H
#define PIC_H

#include "types.h"

// Ports PIC
#define PIC1_COMMAND    0x20
#define PIC1_DATA       0x21
#define PIC2_COMMAND    0xA0
#define PIC2_DATA       0xA1

// Commandes PIC
#define PIC_EOI         0x20    // End Of Interrupt

// Offsets des IRQ (après remapping)
#define IRQ_BASE        32
#define IRQ_TIMER       0
#define IRQ_KEYBOARD    1

void pic_init(void);
void pic_send_eoi(uint8_t irq);
void pic_set_mask(uint8_t irq);
void pic_clear_mask(uint8_t irq);

#endif