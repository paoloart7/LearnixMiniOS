#ifndef IDT_H
#define IDT_H

#include "types.h"

void idt_init(void);
void register_interrupt_handler(uint8_t num, void (*handler)(void));

#endif