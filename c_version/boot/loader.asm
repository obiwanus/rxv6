; Protection Enable flag
%define CR0_PE      0x00000001

%define SEG_KERNEL_CODE 1
%define SEG_KERNEL_DATA 2

; Start the first CPU: switch to 32-bit protected mode, jump into C.
; The BIOS loads this code from the first sector of the hard disk into
; memory at physical address 0x7c00 and starts executing in real mode
; with cs=0 and ip=7c00

bits 16                     ; Emit 16-bit code

global start
extern bootmain

start:
    cli                     ; Disable interrupts

    ; Zero data segment registers DS, ES and SS
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax

    ; Allow using the 21st bit of address with segmented addressing
seta20.1:
    in al, 0x64             ; wait for not busy
    test al, 0x2
    jnz seta20.1

    mov al, 0xd1            ; 0xd1 -> port 0x64
    out 0x64, al

seta20.2:
    in al, 0x64             ; wait for not busy
    test al, 0x2
    jnz seta20.2

    mov al, 0xdf            ; 0xdf -> port 0x60
    out 0x60, al

    ; Switch from real to protected mode. Use a temporary GDT that
    ; maps virtual addresses directly to physical addresses
    lgdt [gdtdesc]
    mov eax, cr0
    or eax, CR0_PE
    mov cr0, eax

    ; Far jump to 32-bit code to reload cs and eip
    jmp (SEG_KERNEL_CODE << 3):start32

bits 32                     ; Emit 32-bit code

start32:
    ; Set up the segment registers to use the GDT below
    mov ax, (SEG_KERNEL_DATA << 3)
    mov ds, ax              ; Make the data segments use the data segment selector
    mov es, ax
    mov ss, ax
    xor ax, ax              ; Zero the extra segments
    mov fs, ax
    mov gs, ax

    ; Set up the stack pointer and call into the C part of the bootloader
    mov esp, start
    call bootmain

    ; bootmain should never return, but if it does, trigger a Bochs breakpoint
    ; (only useful when running under Bochs)
    mov ax, 0x8a00          ; 0x8a00 -> port 0x8a00
    mov dx, ax
    out dx, ax
    mov ax, 0x8ae0          ; 0x8ae0 -> port 0x8a00
    out dx, ax
spin:
    jmp spin                ; the death loop

; Temporary GDT
align 4
gdt:
    ; null descriptor
    dq 0

    ; code segment - 0x0 to 0xffffffff
    dw 0xffff           ; limit 0:15
    dw 0x0              ; base 0:15
    db 0x0              ; base 16:23
    db 0b10011010       ; access flags
    db 0b11001111       ; granularity + limit 16:19
    db 0x0              ; base 24:31

    ; data segment - 0x0 to 0xffffffff
    dw 0xffff           ; limit 0:15
    dw 0x0              ; base 0:15
    db 0x0              ; base 16:23
    db 0b10010010       ; access flags
    db 0b11001111       ; granularity + limit 16:19
    db 0x0              ; base 24:31

gdtdesc:
    dw (gdtdesc - gdt - 1)      ; sizeof(gdt) - 1
    dd gdt
