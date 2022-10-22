/*
Реализуйте на языке ассемблера armv8 (AArch64) функцию с сигнатурой:

int sum(int x0, size_t N, int *X)

Функция должна вычислять значение x0+∑xi, где 0≤i<N

Обратите внимание на то, что тип int имеет размер 32 бит, а тип size_t - 64 бит.
*/
	.text
	.global sum
sum:
	mov    x8, 0
	mov    x10, 0
	mov    x11, 4
LoopBegin:
	cmp    x8, x1
 	bge    LoopEnd
 	ldr    w9, [x2, x10]
	add    w0, w0, w9
	add    x10, x10, x11
  	add    x8, x8, 1
  	b      LoopBegin
LoopEnd:
	ret
