%define KERNBASE 0x80000000

; This file along with the rest of the kernel will be compiled
; into an ELF executable.
; _start denotes the entry point for ELF files.
global _start
_start equ (entry - KERNBASE)

bits 32

entry:
    cli
    cli
    cli
    cli
    xor eax, eax
    xor eax, eax
    xor eax, eax
    xor eax, eax

tmploop:
    jmp tmploop


; TODO: link this file so that the symbols all have high addresses
