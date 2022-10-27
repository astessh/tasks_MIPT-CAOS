/*
Реализуйте на языке ассемблера x86-64 функцию с сигнатурой:

extern double very_important_function(size_t N, const float *A, const float *B, float *R);

Функция вычисляет:

    поэлементную сумму R[i]=A[i]+B[i];
    среднее арифметическое всех значений R[i], которое является возвращаемым результатом функции.

Гарантируется, что число N больше нуля и кратно 8, а адреса массивов выровнены в памяти по границе 32 байта. Используйте векторные инструкции для вычисления. 
*/

    .intel_syntax noprefix
    .global very_important_function
    .text
very_important_function: 
    enter 0, 0
    push rdi
    xor rax, rax
    xorps xmm0, xmm0
    xorps xmm1, xmm1
.Lwhile:
    cmp rdi, rax
    je .Lexit
    movss xmm1, dword ptr [rsi + rax * 4]
    addss xmm1, dword ptr [rdx + rax * 4]
    addss xmm0, xmm1
    movss dword ptr [rcx + rax * 4], xmm1
    inc rax
    jmp .Lwhile
.Lexit:
    cvtss2sd xmm1, xmm0
    movsd xmm0, xmm1
    cvtsi2sd xmm1, rdi
    divsd xmm0, xmm1
    leave
    ret
