start:
	in 01
	mov a, s
	out 01
	ldax a
	mov b, a
	lda undef
	hlt
