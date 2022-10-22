/*
Напишите программу на языке ассемблера armv8 (AArch64), которая читает со стандартного потока ввода последовательность символов, и выводит только те из них, которые являются десятичными цифрами от 0 до 9.

Можно использовать функции стандартной библиотеки языка Си: getchar_unlocked и putchar_unclocked.

В качестве точки входа можно считать функцию main.
*/

.text
	.global main
	.extern getchar_unlocked
	.extern putchar_unlocked
main:
  	str x30, [sp, -16]!
begin:
	bl getchar_unlocked

	cmp x0, -1
		beq end
	cmp x0, 47
		ble begin
	cmp x0, 58
		bge begin

	bl putchar_unlocked
 	b begin
end:
 	ldr x30, [sp], 16
	ret
