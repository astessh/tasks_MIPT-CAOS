/*
Реализуйте программу на языке ассемблера armv8 (AArch64), которая считывает символы со стандартного потока ввода, а затем выводит их в обратном порядке.

Можно использовать функции стандартной библиотеки языка Си: fgetc, fputc, getchar_unlocked, putchar_unlocked, realloc и free.

В качестве точки входа можно считать функцию main.
*/

.text
	.global main
	.extern free
	.extern realloc
	.extern getchar_unlocked
	.extern putchar_unlocked

main:
	str x30, [sp, -16]!
        str x18, [sp, -16]!
        str x19, [sp, -16]!
        str x20, [sp, -16]!
        str x22, [sp, -16]!
        str x23, [sp, -16]!
	mov x19, 0
	mov x20, 8
	mov x21, 1
	mov x22, 8
	mov x0, 0
	mov x1, x22
	bl realloc
	mov x23, x0
begin:
	bl getchar_unlocked
	cmp x0, -1
		beq out
	str x0, [x23, x19, lsl 3]
	add x19, x19, x21
	add x20, x20, x22
	mov x0, x23
	mov x1, x20
	bl  realloc
	mov x23, x0
	b begin
out:
	cmp x19, 0
		beq end
	sub x19, x19, x21
	ldr x0, [x23, x19, lsl 3]
	bl putchar_unlocked
	b out
end:
	mov x0, x23
	bl free

        ldr x23, [sp], 16
        ldr x22, [sp], 16
        ldr x21, [sp], 16
        ldr x20, [sp], 16
        ldr x19, [sp], 16
 	ldr x30, [sp], 16
	ret

