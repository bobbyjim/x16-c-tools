.org $0400
.segment "STARTUP"
.segment "INIT"
.segment "ONCE"
.segment "CODE"

	jmp start 

array: .asciiz "0123456789abcdefghjklmnpqrstuvwxyz"

start:
	ldx #16
	stx $02
	stx $03
@loop:
	lda $02
	sbc #01
	sta $02
	lda $03
	beq @done
	sbc #00
	sta $03
	tax
	lda array,x
	jsr $ffd2
	bra @loop

@done:
	rts
