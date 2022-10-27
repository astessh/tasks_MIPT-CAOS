/*
Реализуйте на языке ассемблера x86-64 функцию с сигнатурой:

extern float dot_product(size_t N, const float *A, const float *B);

которая вычисляет скалярное произведение векторов A и B.

Гарантируется, что N больше нуля, но не гарантируется, что N кратно восьми.

Значения исходных векторов хранятся в произвольных адресах памяти, поэтому выравнивание для них не гарантируется.

Необходимо минимизировать количество выполняемых вычислительных инструкций, используя векторные вычисления. Доступен набор команд AVX.
*/

    .intel_syntax noprefix
    .global dot_product
    .text
dot_product:
    vxorpd ymm0, ymm0, ymm0
    mov rcx, rdi
gbegin:
    cmp rcx, 8
    jb gend
    sub rcx, 8
    vmovups ymm1, [rsi+4*rcx]
    vmovups ymm2, [rdx+4*rcx]
    vdpps ymm3, ymm1, ymm2, 0xFF
    vaddps ymm0, ymm0, ymm3
    jmp gbegin
gend:
    vextractf128 xmm2, ymm0, 1
    addss xmm0, xmm2
lbegin:
    cmp rcx, 0
    je lend
    sub rcx, 1
    movss xmm1, [rsi + rcx * 4]
    movss xmm2, [rdx + rcx * 4]
    mulss xmm1, xmm2
    addss xmm0, xmm1
    jmp lbegin
lend:
    ret
