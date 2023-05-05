	.arch msp430g2553
	.p2align 1,0
	.text

	.global turnRightS
	.extern direction


turnRightS:
	cmp &direction, #0
	jnc out
	mov #3, &direction
	pop r0
out:
	sub #1, &direction
	pop r0
