bits 64
default rel

segment .data
    msg db "Hello World! Super Long Text to debug", 0xd, 0xa, 0
    primeNumber dd 255
    primeNumber2 dd 512

segment .text
global main
extern printf, ExitProcess

main:
    push rbp
    mov  rbp, rsp
    mov rbx, 3
    mov rcx, 2
    int3
    add rbx, rcx
    int3
    sub rsp, 32
    lea rcx, [primeNumber]
    int3
    call printf

    mov rax, 0
    call ExitProcess

; section '.idata' import readable writeable
; idt: ; import directory table starts here
;      ; entry for KERNEL32.DLL
;      dd rva kernel32_iat ;rva = relative virtual address
;      dd 0 ;dd = Define double word. Generally 4 bytes on a typical x86 32-bit system
;      dd 0
;      dd rva kernel32_name
;      dd rva kernel32_iat ;iat = import address table
;      ; NULL entry - end of IDT
;      dd 5 dup(0)
; name_table: ; hint/name table
;         _ExitProcess_Name dw 0 ;dw = Define Word. Generally 2 bytes on a typical x86 32-bit system
;                           db "ExitProcess", 0, 0 ;db = Define Byte. 8 bits

; kernel32_name: db "KERNEL32.DLL", 0
; kernel32_iat: ; import address table for KERNEL32.DLL
;         ExitProcess dq rva _ExitProcess_Name
;         dq 0 ; end of KERNEL32's IAT ;define quadword. 64 bit/8 byte