	.data
output_string: 	.asciz "%d\n"
input_string: 	.asciz "%d %d"
a: 		.word 0
b: 		.word 0

	.text
	.global main
	.extern printf
	.extern scanf
main:
	str x30, [sp, -16]!
	ldr x0,  =input_string
	ldr x1, =a
	ldr x2, =b
	bl scanf
	ldr x1, =a
	ldr x2, =b
	ldr x1, [x1]
	ldr x2, [x2]

	add x1, x2, x1
	ldr x0, =output_string
	bl printf

	ldr x30, [sp], 16
	ret
