/*
Реализуйте функцию на языке ассемблера x86-64 с сигнатурой:

extern void mergesort(int from, int to, const int *in, int *out);
      

Функция выполняет сортировку целых чисел слиянием (Merge Sort).

Параметры from и to - это индексы левой и правой границ областей сортировки [from, to); in - исходный массив, out - указатель на выделенную память для хранения результата (начиная с которого нужно класть ответ).
*/
    .intel_syntax noprefix
    .text
    .global mergesort

mergesort:
    push rbp
    push r12
    push r13
    push r14
    push r15
    sub rsi, rdi
    shl rdi,2
    add rdx, rdi
    mov rdi, 0 # tempo
    mov r11, 1 # curr_size
    mov r12, 0 # l
    mov r13, 0 # m
    mov r14, 0 # r
    mov r15, 0 # i
    mov rax, 0 # j
    mov r10, 0 # k
    mov r8, 0 # n1
    mov r9, 0 # n2
fcheck:
    cmp r11, rsi
    jl fbegin
    mov rdi, 0
loop:
    cmp rdi, rsi
    jge end
    mov ebp, [rdx+4*rdi]
    mov [rcx+4*rdi], ebp
    add rdi, 1
    jmp loop
end:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    mov rax, 0
    ret

fbegin:
    mov r12, 0
scheck:
    mov rdi, rsi
    sub rdi, 1
    cmp r12, rdi
    jl sbegin
    add r11, r11
    jmp fcheck


sbegin:
    mov r13, r12
    add r13, r11
    sub r13, 1
    mov rdi ,rsi
    sub rdi, 1
    cmp rdi, r13
    jl ifm
fim:
    mov r14, r11
    add r14, r11
    add r14, r12
    sub r14, 1
    cmp rdi, r14
    jl ifr
fir:
    mov r10, r12
    mov r15, 0
    mov rax, 0
    mov r8, r13
    sub r8, r12
    add r8, 1
    mov r9, r14
    sub r9, r13
    mov rdi, 0
tcheck:
    cmp rdi, rsi
    jl tbegin
fwhile:
    cmp r15, r8 
    jge continue
    cmp rax, r9
    jge continue
    mov rdi, r13
    add rdi, rax
    add rdi, 1
    mov ebp, [rcx+4*rdi]
    mov rdi, r12
    add rdi, r15
    mov ebx, [rcx+4*rdi]
    cmp ebx, ebp
    jle if
    mov [rdx+4*r10], ebp
    add rax, 1
fi:
    add r10, 1
    jmp fwhile

continue:
    cmp r15, r8
    jl swhile
continue2:
    cmp rax, r9
    jl twhile
    add r12, r11
    add r12, r11
    jmp scheck

twhile:
    mov rdi, r13
    add rdi, 1
    add rdi, rax
    mov ebp, [rcx+4*rdi]
    mov [rdx+4*r10], ebp
    add rax, 1
    add r10, 1
    jmp continue2

swhile:
    mov rdi, r12
    add rdi, r15
    mov ebp, [rcx+4*rdi]
    mov [rdx+4*r10], ebp
    add r15, 1
    add r10, 1
    jmp continue

tbegin:
    mov ebp, [rdx+4*rdi]
    mov [rcx+4*rdi], ebp
    add rdi, 1
    jmp tcheck

if:
    mov [rdx+4*r10], ebx
    add r15, 1
    jmp fi

ifm:
    mov r13, rdi
    jmp fim

ifr:
    mov r14, rdi
    jmp fir
