#include <sys/syscall.h>
.intel_syntax noprefix
    .text
    .global _start
_start:
begin:
    mov   rax, SYS_read
    mov   rdi, 0
    mov   rsi, ptr_to_sym
    mov   rdx, 1
    syscall

    cmp rax, 0
    je end

    mov rax, SYS_write
    mov rdi, 1
    mov rsi, ptr_to_sym
    mov rdx, 1
    syscall
    jmp begin

end:
    mov rax, SYS_exit
    mov rdi, 0
    syscall

    .data
sym:
    .byte 0
ptr_to_sym:
    .quad sym
