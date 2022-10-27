/*
Реализуйте функцию на языке ассемблера x86-64 с сигнатурой:

extern void sum(size_t N, const int *A, const int *B, int *R);
      

Функция вычисляет значения Ri=Ai+Bi для всех i от 0 до N.

Память для хранения массива-результата уже выделена.
*/
    .intel_syntax noprefix
    .text
    .global sum

sum:
    push rbx
begin:
    dec edi
    mov eax, [rsi + 4*rdi]
    add eax, [rdx + 4*rdi]
    mov [rcx + 4*rdi], eax
    mov rbx, 0
    cmp rdi, rbx
    jg begin
end:
    pop rbx
    mov rax, 0
    ret
