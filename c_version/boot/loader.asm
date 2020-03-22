; Protection Enable flag
%define CR0_PE      0x00000001

; Start the first CPU: switch to 32-bit protected mode, jump into C.
; The BIOS loads this code from the first sector of the hard disk into
; memory at physical address 0x7c00 and starts executing in real mode
; with cs=0 and ip=7c00

bits 16                     ; Emit 16-bit code

global start

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
    lgdt gdtdesc
    mov eax, cr0
    or eax, CR0_PE
    mov cr0, eax
