

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
