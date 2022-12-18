/*Реализуйте на языке ассемблера x86_64 программу, которая читает со стандартного потока ввода текст, после чего выводит все строки текста в обратном порядке.

Использование стандартной библиотеки Си запрещено.

Точка входа в программу - функция _start.

Файл может не содержать перевода строки в конце.*/

#include <sys/syscall.h>
#define capacity r13
#define size r12
#define to_read r14
#define adress rbx
#define line_len r15

    .intel_syntax noprefix
    .text
    .global _start

_start:
    push r12
    push r13
    push r14
    push r15
    push rbx
    mov rax, SYS_brk
    mov rdi, 0
    syscall
    mov adress, rax
    mov capacity, 1
    mov r12, 0
alloc:
    add adress, capacity
    mov rax, SYS_brk
    mov rdi, rbx
    syscall
    mov rbx, rax
    sub adress, capacity
    mov to_read, capacity
    jmp read
realloc:
    add adress, capacity
    mov rax, SYS_brk
    mov rdi, adress
    syscall
    mov adress, rax
    sub adress, capacity
    mov to_read, capacity
    add capacity, capacity
    jmp read
read:
    mov   rax, SYS_read
    mov   rdi, 0
    mov   rsi, adress
    mov   rdx, to_read
    syscall
    cmp rax, 0
    je check_for_NL_sym
    add adress, rax
    add size, rax
    sub to_read, rax
    cmp size, capacity
    je realloc
    jmp read

realloc_one:
    add adress, capacity
    mov rax, SYS_brk
    mov rdi, adress
    syscall
    mov adress, rax
    sub adress, capacity
    mov to_read, capacity
    add capacity, capacity
    jmp add_NL_sym

add_NL_sym:
    movb [adress], 10
    mov rax, 1
    add adress, rax
    add size, rax
    sub to_read, rax
    jmp start_print

check_for_NL_sym:
    cmp size, 0
    je exit
    sub adress, 1
    mov rax, [adress]
    add adress, 1
    cmp rax, 10
    jne realloc_one
    jmp start_print

start_print:
    mov to_read, size
    sub to_read, 1
    sub adress, 1
next_line:
    mov line_len, 1
back:
    cmp to_read, 0
    jle first_line
    add line_len, 1
    sub to_read, 1
    sub adress, 1
    movb al, [adress]
    cmp al, 10
    je print_line
    jmp back

print_line:
    sub line_len, 1
    add adress, 1
    mov rax, SYS_write
    mov rdi, 1
    mov rsi, adress
    mov rdx, line_len
    syscall
    add line_len, 1
    sub adress, 1
    jmp next_line

first_line:
    mov rax, SYS_write
    mov rdi, 1
    mov rsi, adress
    mov rdx, line_len
    syscall
    jmp exit

exit:
    pop rbx
    pop r15
    pop r14
    pop r13
    pop r12
    mov rax, SYS_exit
    mov rdi, 0
    syscall
