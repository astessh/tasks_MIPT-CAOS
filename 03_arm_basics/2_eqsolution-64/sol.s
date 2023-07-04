
	.text
	.global solve
solve:
	mov w4,0
	sxtw x1,w1
	sxtw x2,w2
	sxtw x3,w3
Begin:
	sxtw x7,w4
	mul x5,x7,x7
	mul x5,x5,x7
	mul x5,x5,x0
	mul x6,x7,x7
	mul x6,x6,x1
	add x5,x5,x6
	mul x6,x7,x2
	add x5,x5,x6
	add x5,x5,x3
	cmp x5,0
	beq End
	add w4,w4,1
	cmp w4, 255
	bne Begin
End:
	mov w0,w4
	ret
