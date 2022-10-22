.intel_syntax noprefix
	.text
	.global very_important_function

very_important_function:
	sub rsp, 8
	push r12
	push r13
	push r14
	push r15

	mov r12, rdi
	mov r13, 0
	jmp .LoopBegin

.LoopBegin:
	cmp r13, r12
	jge .LoopEnd

	mov rdi, offset scanf_data_format
	mov rsi, rsp
	call scanf

	mov r14, A
	movsxd r14, [r14 + 4 * r13]
	movsxd r15, [rsp]
	imul r14, r15

	mov rdi, offset printf_data_format
	mov rsi, r14
	call printf

	inc r13
	jmp .LoopBegin

.LoopEnd:
	pop r15
	pop r14
	pop r13
	pop r12

	add rsp, 8
	mov rax, 0
	ret

.section .rodata

scanf_data_format:
	.string "%d"

printf_data_format:
	.string "%lld\n"

