[BITS 32]

global idt_load
global isr_stub_0
global isr_stub_32
global isr_stub_33

extern isr_handler

section .text

; Charger l'IDT
idt_load:
    mov eax, [esp + 4]
    lidt [eax]
    ret                     ; Ne pas activer les interruptions ici

; === Exception 0 : Division par zéro ===
isr_stub_0:
    cli
    push dword 0            ; Code erreur factice
    push dword 0            ; Numéro d'interruption
    jmp isr_common_stub

; === IRQ0 : Timer ===
isr_stub_32:
    cli
    push dword 0            ; Code erreur factice
    push dword 32           ; Numéro d'interruption
    jmp isr_common_stub

; === IRQ1 : Clavier ===
isr_stub_33:
    cli
    push dword 0            ; Code erreur factice
    push dword 33           ; Numéro d'interruption
    jmp isr_common_stub

; === Handler commun ===
isr_common_stub:
    ; Sauvegarder tous les registres
    pusha
    
    ; Sauvegarder les segments
    mov ax, ds
    push eax
    
    ; Charger le segment de données du kernel
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Appeler le handler C avec le numéro d'interruption
    ; Le numéro est à [esp + 36] (après pusha=32 + push ds=4)
    mov eax, [esp + 36]
    push eax
    call isr_handler
    add esp, 4
    
    ; Restaurer le segment de données
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Restaurer les registres
    popa
    
    ; Nettoyer la pile (numéro int + code erreur)
    add esp, 8
    
    ; Retour d'interruption
    iret