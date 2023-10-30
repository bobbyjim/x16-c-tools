; Build using:	cl65 -t cx16 adsr5.s -C cx16-asm.cfg -o ADSR5.PRG

.org $0400
.segment "CODE"

	ldx #00
loop:
	jmp (handlers,x)
back:
	inx
	inx
	cpx #10
	bcc loop
	rts

state0:
	lda #65
	jsr $ffd2
	bra back

state1:
	lda #66
	jsr $ffd2
	bra back

state2:
	lda #67
	jsr $ffd2
	bra back

state3:
	lda #68
	jsr $ffd2
	bra back

state4:
	lda #69
	jsr $ffd2
	bra back

state5:
	lda #70
	jsr $ffd2
	bra back

handlers: 
	.word state0
	.word state1
	.word state2
	.word state3
	.word state4
