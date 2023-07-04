    .intel_syntax noprefix
    .global avx_sin
    .text
avx_sin:
    # xmm0 - sum
    # xmm1 - new
    # xmm4 - -1
    # xmm5 - 1
    # xmm6 - count
    # xmm7 - acc
    # xmm9 - input
    vmovsd xmm9, xmm0, xmm0
    vmovsd xmm1, xmm0, xmm0
    vxorpd xmm0, xmm0, xmm0
    mov rax, 1 
    vcvtsi2sd xmm5, xmm5, rax
    vmovsd xmm6, xmm5, xmm5
    mov rax, -1
    vcvtsi2sd xmm4, xmm4, rax
begin:
    vmovsd xmm7, xmm0, xmm0
    vaddsd xmm0, xmm0, xmm1
    vcomisd xmm0, xmm7
    je end
    vmulsd xmm1, xmm1, xmm4
    vaddsd xmm6, xmm6, xmm5 
    vdivsd xmm1, xmm1, xmm6 
    vaddsd xmm6, xmm6, xmm5
    vdivsd xmm1, xmm1, xmm6
    vmulsd xmm1, xmm1, xmm9
    vmulsd xmm1, xmm1, xmm9
    jmp begin 
end:
    vmovsd xmm0, xmm7, xmm7
    ret
