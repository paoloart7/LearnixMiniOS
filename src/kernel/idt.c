#include "idt.h"
#include "pic.h"

#define IDT_ENTRIES 256

// Structure IDT Entry
typedef struct {
    uint16_t base_low;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  flags;
    uint16_t base_high;
} __attribute__((packed)) idt_entry_t;

// Structure IDT Pointer
typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

static idt_entry_t idt[IDT_ENTRIES];
static idt_ptr_t idt_ptr;

// Handlers
static void (*handlers[IDT_ENTRIES])(void);

// Handler ASM externe
extern void isr_keyboard(void);

// Fonction pour définir une entrée IDT
static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low  = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector  = sel;
    idt[num].zero      = 0;
    idt[num].flags     = flags;
}

void register_interrupt_handler(uint8_t num, void (*handler)(void)) {
    handlers[num] = handler;
}

// Appelé depuis ASM
void isr_handler(uint32_t int_num) {
    if (handlers[int_num]) {
        handlers[int_num]();
    }
    
    if (int_num >= 32 && int_num < 48) {
        pic_send_eoi(int_num - 32);
    }
}

void idt_init(void) {
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base  = (uint32_t)&idt;
    
    // Mettre à zéro
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate(i, 0, 0x08, 0x00);
        handlers[i] = NULL;
    }
    
    // Initialiser PIC
    pic_init();
    
    // IRQ1 = Clavier = INT 33
    idt_set_gate(33, (uint32_t)isr_keyboard, 0x08, 0x8E);
    
    // Charger IDT
    asm volatile("lidt %0" : : "m"(idt_ptr));
    
    // Activer les interruptions
    asm volatile("sti");
}