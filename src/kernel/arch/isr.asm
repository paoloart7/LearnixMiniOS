[BITS 32]

global isr_keyboard
extern isr_handler

section .text

isr_keyboard:
    cli
    pusha
    
    ; Appeler le handler C avec le numéro 33
    push dword 33
    call isr_handler
    add esp, 4
    
    popa
    sti
    iret