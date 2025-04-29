; section .text
;     global _start ;entry point for linker
;     extern ExitProcess

;     section .data
;     ;constant
;     hello: db "Hi Mom!", 10 ; string to print. db = Define Byte. 8 bits
;     hellolen: equ $-hello ;length of string. equ = equate
;     exit_code dq 0

;     section .bss
;     ;variable

;     section .text
;     _start:   ;start here
;     ;instruction        operands
;         mov rax,1 ;sys_write
;         mov rdi,1 ;stdout
;         mov rsi,hello ;message to write
;         mov rdx,hellolen ;message length
;         syscall

;     ;end program
;         mov rax,60         ;sys_exit
;         mov rdi,0 ;error code 0 (success)
;         syscall           ;call kernel

section .text
    global main
    extern GetStdHandle, WriteConsoleA, ExitProcess  ; Import ExitProcess from kernel32.dll

section .data
    message db "Hello World!", 0
    messagelength equ $-message
    STD_OUTPUT_HANDLE equ -11
    exit_code dq 0      ; Exit status

section .bss
    bytes_written resq 1

section .text
    main:
        ;Get handle for standard output
        mov ecx, STD_OUTPUT_HANDLE
        call GetStdHandle
        mov r12, rax ;save handle

        ;Call write console a (handle, msg, msg_len, &bytes_written, NULL)
        mov rcx, r12 ;hConsoleOutput
        lea rdx, [rel message] ;lpBuffer
        mov r8, messagelength ;nNumberOfCharsToWrite
        lea r9, [rel bytes_written] ;lpNumberOfCharsWritten
        push 0 ;lpReserved (NULL)
        call WriteConsoleA

        mov ecx, 0          ; Exit code 0
        call ExitProcess    ; Call Windows API ExitProcess


