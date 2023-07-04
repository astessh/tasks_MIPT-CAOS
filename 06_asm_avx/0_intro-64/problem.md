# asm-x86/avx/intro-64

Реализуйте на языке ассемблера x86-64 функцию с сигнатурой:
```
extern double very_important_function(size_t N, const float *A, const float *B, float *R);
```
Функция вычисляет:

    поэлементную сумму R[i]=A[i]+B[i];
    среднее арифметическое всех значений R[i], которое является возвращаемым результатом функции.

Гарантируется, что число N больше нуля и кратно 8, а адреса массивов выровнены в памяти по границе 32 байта. Используйте векторные инструкции для вычисления. 