
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
