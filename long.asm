; temporary storage is allocated in the
; "args" area of the run-time environment

	org  0h

start:	lxi  h, u	; save 1
	mvi  a, 2
	mov  a, m
        inx  h
        xra  a
        inx  h
        mov  a, m
        inx  h
        mov  a, m
        inx  h
        mov  a, m

        lxi  b, 0000h
        lxi  d, 0002h
        
        call qmul
        hlt

; quad subtraction  u - (BCDE) -> (BCDE)

qsub:	call qneg  	;complement (BCDE) and fall thru to add

; quad addition     u + (BCDE) -> (BCDE)

qadd:   push h
	lxi  h,u_3	;tenSHUN
	mov  a,m	;hup
	add  e		;two
	mov  e,a	;three
	dcx  h		;four
	mov  a,m	;hup
	adc  d		;two
	mov  d,a	;three
	dcx  h		;four
	mov  a,m	;hup
	adc  c		;two
	mov  c,a	;three
	dcx  h		;four
	mov  a,m	;hup
	adc  b		;two
	mov  b,a	;three
	pop  h		;four
	ret		;at ease	
	

; two's complement (BCDE)

qneg:	push h
	xra  a
	mov  l,a
	sbb  e
	mov  e,a
	mov  a,l
	sbb  d
	mov  d,a
	mov  a,l
	sbb  c
	mov  c,a
	mov  a,l
	sbb  b
	mov  b,a
	pop  h
	ret


qneghl: push b
	push d
	call qld
	call qneg
	call qst
	pop  d
	pop  b
	ret

; signed quad multiplication
; u * (BCDE) --> (BCDE)

qmul:	call csign			;take abs values and compute signs
	push psw			;save result sign
	call uqmul			;compute product
qmul1:	pop  psw
	jm   qneg			;complement product if needed
	ret

; csign takes abs vals of u, (BCDE), and computes product of their signs

csign:	mov  a,b			;look at (BCDE) first
	ora  a
	push psw			;save flags
	cm   qneg			;complement if needed
	lxi  h,u			;now look at u
	mov  a,m
	ora  a
	jp   csign1
	call qneghl
	pop  psw
	xri  80h			;flip sign
	ret
csign1:	pop psw
	ret

; unsigned quad multiplication 
; u * (BCDE) --> (BCDE)	    (expects ptr. to u in (HL)

uqmul:	lxi  h,u
	push h				;put pointer to u on stack
	lxi  h,mq
	call qst			;(BCDE) -> mq
	lxi  b,0			;init product to 0
	lxi  d,0
uqmul1:	call qtsthl			;test if mq is 0
	jz   uqmul2			;if so, done
	xra  a				;clear carry
	call qrarhl			;shift mq over
	cc   qadd			;add u to (BCDE) if lsb=1
	xthl				;get pointer to u
	xra  a				;clear carry
	call qralhl			;double u
	xthl				;get back pointer to mq
	jmp  uqmul1
uqmul2:	pop  h				;restore stack
	ret

; signed division  u / (BCDE) --> (BCDE)

qdiv:	call qtst			;first test for zero divisor
	rz
	call csign			;take care of signs
	push psw			;save quotient sign
	call uqdiv
	call qld			;get quotient in (BCDE)
	jmp  qmul1			;adjust sign of result

;  signed remainder  u mod (BCDE) --> (BCDE)

qmod:	call qtst			;test for zero modulus
	rz
	lda  u				;sign of u is that of result
	ora  a
	push psw			;save flags
	call csign			;get abs val of args
	call uqdiv			;remainder in (BCDE)
	jmp  qmul1


;  unsigned division  u / (BCDE) --> mq, remainder in (BCDE)



uqdiv:	lxi  h,mq			;mq will contain quotient
	call qclrhl			;clear it
	push h				;save it on the stack

	mvi  l,1			;now normalize divisor
uqdiv1:	mov  a,b			;look at most signif non-sign bit
	ani  40h
	jnz   uqdiv2
	call qral			;if not 1, shift left
	inr  l
	jmp  uqdiv1
uqdiv2:	mov  a,l
	sta  temp			;save normalization count
	lxi  h,u			
	call qxchg			;want divid in (BCDE), divisor in u
	xthl				;pointer to mq in (HL), u on stack

;main loop

uqdiv3: call trial			;trial subtraction of divisor
	call qralhl			;shift in the carry
	lda  temp			;get the count
	dcr  a
	jz   uqdiv4			;done
	sta  temp			;save count again
	xthl				;divisor in (HL)
	xra  a
	call qrarhl			;shift it right one
	xthl				;quotient in (HL)
	jmp  uqdiv3

uqdiv4: inx  sp
	inx  sp				;clean off top of stack
	ret


trial:	call qsub			;subtract divid from divisor
	call qneg			;actually want divisor from divid
	stc				;assume was positive
	rp
	call qadd			;else must restore dividend
	xra  a				;clear carry
	ret


;
; routines to manipulate quads
;
; qld loads the quad pointed to by (HL) into (BCDE)

qld:	push h
	mov  b,m
	inx  h
	mov  c,m
	inx  h
	mov  d,m
	inx  h
	mov  e,m
	pop  h
	ret

; qst is inverse of qld

qst:	push h
	mov  m,b
	inx  h
	mov  m,c
	inx  h
	mov  m,d
	inx  h
	mov  m,e
	pop  h
	ret



; rotate  (BCDE) right thru carry

qrar:	mov a,b
	rar
	mov b,a
	mov a,c
	rar
	mov c,a
	mov a,d
	rar
	mov d,a
	mov a,e
	rar
	mov e,a
	ret

; same for quad pointed to by (HL)

qrarhl:	push h
	mov  a,m
	rar
	mov  m,a
	inx  h
	mov  a,m
	rar
	mov  m,a
	inx  h
	mov  a,m
	rar
	mov  m,a
	inx  h
	mov  a,m
	rar
	mov  m,a
	pop  h
	ret


; rotate (BCDE) left thru carry

qral:	mov a,e
	ral
	mov e,a
	mov a,d
	ral
	mov d,a
	mov a,c
	ral
	mov c,a
	mov a,b
	ral
	mov b,a
	ret

; qralhl does it for quad pointed to by (HL)

qralhl:	inx  h
	inx  h
	inx  h				;get to rightmost byte
	mov  a,m
	ral
	mov  m,a
	dcx  h
	mov  a,m
	ral
	mov  m,a
	dcx  h
	mov  a,m
	ral
	mov  m,a
	dcx  h
	mov  a,m
	ral
	mov  m,a
	ret


;qclrhl clears quad pointed to by (HL)

qclrhl:	push h
	xra  a
	mov  m,a
	inx  h
	mov  m,a
	inx  h
	mov  m,a
	inx  h
	mov  m,a
	pop  h
	ret


; qtst tests sign of (BCDE), setting the usual flags

qtst:	mov  a,b			;look at most signif byte
	ora  a
	rnz
	ora  c				;test for zero
	ora  d
	ora  e
qtst1:	rp
	mvi  a,1
	ora  a
	ret

qtsthl:	mov  a,m
	ora  a
	rnz
	push h
	inx  h
	ora  m
	inx  h
	ora  m
	inx  h
	ora  m
	pop  h
	jmp  qtst1

; swap (BCDE) with thing pointed to by HL

qxchg:	push h
	mov  a,m
	mov  m,b
	mov  b,a
	inx  h
	mov  a,m
	mov  m,c
	mov  c,a
	inx  h
	mov  a,m
	mov  m,d
	mov  d,a
	inx  h
	mov  a,m
	mov  m,e
	mov  e,a
	pop  h
	ret


; Data
u:			;temporary quad storage (4 bytes)
uh:	dw   0		;high word of u
ul:	db   0		;low word of u
u_3:	db   0
mq:	dw   0		;temporary quad storage used by
	dw   0		;multiplication and division routines
temp:	dw   0		;temporary storage byte used by div'n routine
	dw   0


