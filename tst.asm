	org	0h

abc	equ	aaa
	
start:	in	00
	mov	b, a
	in	01
	mov	c, a
	in	02
	mov	d, a
loop:	call	show
	call	rot
	jmp	loop
 
show:	mov	a, d
	out	02
	mov	a, c
	out	01
	mov	a, b
	out	00
	ret

rot:	mov	a, b
	rar
	mov	a, c
	rar
	mov	c, a
	mov	a, d
	rar
	mov	d, a
	mov	a, b
	rar
	mov	b, a
	ret

aaa:	db	10h
bbb:	dw	5555h
ccc:	dw	7777h
eee:	db	00
fff:	db	01h
