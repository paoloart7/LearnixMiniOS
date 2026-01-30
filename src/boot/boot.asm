[BITS 16]
[ORG 0x7C00]

KERNEL_OFFSET     equ 0x10000
KERNEL_SECTORS    equ 32          ; 32 secteurs = 16KB (assez large)

CODE_SEG          equ 0x08
DATA_SEG          equ 0x10

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti
    
    ; Sauvegarder le numéro de lecteur (passé par le BIOS dans DL)
    mov [boot_drive], dl
    
    mov si, msg_boot
    call print_string_16
    
    call load_kernel
    
    mov si, msg_loaded
    call print_string_16
    
    call switch_to_pm
    
    jmp $

load_kernel:
    mov si, msg_loading
    call print_string_16
    
    ; ES:BX = 0x1000:0x0000 = adresse physique 0x10000
    mov ax, 0x1000
    mov es, ax
    xor bx, bx
    
    ; Paramètres INT 13h
    mov ah, 0x02            ; Fonction: Lire secteurs
    mov al, KERNEL_SECTORS  ; Nombre de secteurs
    mov ch, 0               ; Cylindre 0
    mov cl, 2               ; Secteur 2 (après le bootloader)
    mov dh, 0               ; Tête 0
    mov dl, [boot_drive]    ; Lecteur (sauvegardé au boot)
    
    int 0x13
    jc disk_error
    
    ; Restaurer ES
    xor ax, ax
    mov es, ax
    ret

disk_error:
    mov si, msg_disk_error
    call print_string_16
    jmp $

print_string_16:
    pusha
.loop:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0E
    mov bh, 0
    int 0x10
    jmp .loop
.done:
    popa
    ret

switch_to_pm:
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:init_pm

; Données
boot_drive: db 0

msg_boot:       db "LearnixMiniOS Boot", 0x0D, 0x0A, 0
msg_loading:    db "Loading kernel...", 0x0D, 0x0A, 0
msg_loaded:     db "Kernel loaded!", 0x0D, 0x0A, 0
msg_disk_error: db "Disk Error!", 0x0D, 0x0A, 0

; GDT
gdt_start:
    dq 0x0
gdt_code:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0x00
gdt_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; Mode protégé 32 bits
[BITS 32]
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000
    mov ebp, esp
    
    ; Saut vers le kernel
    jmp CODE_SEG:KERNEL_OFFSET

; Padding et signature
times 510-($-$$) db 0
dw 0xAA55