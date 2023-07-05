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
