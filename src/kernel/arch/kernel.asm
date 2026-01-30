[BITS 32]

section .text.start
global _start
extern kernel_main

_start:
    mov esp, 0x90000
    mov ebp, esp
    call kernel_main
.hang:
    cli
    hlt
    jmp .hang