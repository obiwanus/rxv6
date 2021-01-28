; The kernel will be linked so that the symbols will have
; virtual addresses starting from KERNBASE
%define KERNBASE    0x80000000

; Size of per-process kernel stack
%define KERN_STACK_SIZE 4096

; Page size extension (enables 4Mb pages)
%define CR4_PSE     0x00000010
; Write protect
%define CR0_WP      0x00010000
; Paging
%define CR0_PG      0x80000000

; This file along with the rest of the kernel will be compiled
; and linked into an ELF executable.
; _start denotes the entry point for ELF files.
global _start
_start equ (entry - KERNBASE)
extern kernel_start
extern entry_page_dir

bits 32

section .text
; The boot processor jumps here after executing the boot loader
entry:
    ; Enable page size extension
    mov eax, cr4
    or eax, CR4_PSE
    mov cr4, eax

    ; Specify page directory
    mov eax, (entry_page_dir - KERNBASE)
    mov cr3, eax

    ; Turn on paging
    mov eax, cr0
    or eax, (CR0_PG | CR0_WP)
    mov cr0, eax

    ; Set up the stack pointer
    mov esp, stack + KERN_STACK_SIZE

    ; Jump to kernel_start and switch to high addresses.
    ; Doing it as an indirect jump because otherwise the assembler
    ; could generate a PC-relative jump
    mov eax, kernel_start
    jmp eax

section .bss
stack:      resb KERN_STACK_SIZE
