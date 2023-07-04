
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
