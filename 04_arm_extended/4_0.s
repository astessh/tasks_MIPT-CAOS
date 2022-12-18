/*
Реализуйте на языке ассемблера armv8 (AArch64) функцию calculate, которая вычисляет значение выражения:
R = ( A * B ) + ( C * D ), где A, B, C, и D - это глобальные переменные типа uint64_t,
объявленные во внешнем модуле компиляции, а R, - глобальная переменная типа uint64_t в текущем модуле компиляции.
*/
    .text
    .global R
    .global calculate

calculate:
    adr x5, A
    ldr x0, [x5]

    adr x5, B
    ldr x1, [x5]

    adr x5, C
    ldr x2, [x5]

    adr x5, D
    ldr x3, [x5]

    mul x0, x0, x1
    mul x2, x2, x3
    add x0, x0, x2
    adr x1, R
    str x0, [x1]
    ret

.data
R:
    .quad 0
