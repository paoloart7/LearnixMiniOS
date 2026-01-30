#include "pic.h"

void pic_init(void) {
    // ICW1: Initialisation
    outb(PIC1_COMMAND, 0x11);
    io_wait();
    outb(PIC2_COMMAND, 0x11);
    io_wait();
    
    // ICW2: Vecteurs d'interruption
    outb(PIC1_DATA, 0x20);  // IRQ 0-7  -> INT 32-39
    io_wait();
    outb(PIC2_DATA, 0x28);  // IRQ 8-15 -> INT 40-47
    io_wait();
    
    // ICW3: Cascade
    outb(PIC1_DATA, 0x04);
    io_wait();
    outb(PIC2_DATA, 0x02);
    io_wait();
    
    // ICW4: Mode 8086
    outb(PIC1_DATA, 0x01);
    io_wait();
    outb(PIC2_DATA, 0x01);
    io_wait();
    
    // Masquer toutes les IRQ sauf IRQ1 (clavier)
    outb(PIC1_DATA, 0xFD);  // 11111101 = seulement IRQ1 actif
    io_wait();
    outb(PIC2_DATA, 0xFF);
    io_wait();
}

void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);
}