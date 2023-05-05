	.arch msp430g2553
	.p2align 1,0
	.text

	.global turnLeftS
	.extern direction


turnLeftS:
	cmp #3, &direction
	jnc out
	mov #0, &direction
	pop r0
out:
	add #1, &direction
	pop r0
