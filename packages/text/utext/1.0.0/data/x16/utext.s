;
;   IN PROGRESS -- when it's actually working
;   then I can write a platform/x16 function
;   to call it.
;

;
; cl65 -o UTEXT.BIN -t cx16 -C cx16-asm.cfg utext.s
;
chrout		= $ffd2

	.org 	$A000
	.export	LOADADDR = *

; $A000
Main:	stz $02		; cur-pos in R0L
		stz $03		; case shift in R0H

loop:	ldy $02		; get cur-pos
		lda $04,Y   ; get current byte
		and #$1f	; lower 5 bits
		cmp	$1f		; is it == 31?
		bne index	; no
		inc	$03		; yes, increment case 
		bra loop	; next byte

index:	ldx $03		; get case shift
		cpx #$01	; is it shifted?
		bne fetch	; no
		adc #$20	; yes, add 32 to the offset

fetch:	tay			; our first index goes in Y
		lda case,Y	; index into our case table
		jsr $ffdr   ; and print it

....got to repeat this for the next two bytes...

;   Z-char 0123456789abcdef0123456789abcdef
;          --------------------------------
;   x-set   ABCDEFGHIJKLMNOPQRSTUVWXYZ?:'!y
;   y-set   0123456789ABCDEFGHILMNORSTUWY-x

case:	.byte 32, 65,66,67, 68,69,70, 71,72,73, 74,75,76, 77,78,79, 80,81,82, 83,84,85, 86,87,88, 89,90,'?', ':','\'','!', 0
        .byte 32, 48,49,50, 51,52,53, 54,55,56, 57,65,66, 67,68,69, 70,71,72, 73,76,77, 78,81,84, 85,86,87,  89,91,'-',    0


