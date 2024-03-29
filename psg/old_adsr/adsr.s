; Simplest Sound Effects Library for BASIC Programs
; Author:		Dusan Strakl
; More Info:	8BITCODING.COM
;
; System:		Commander X16
; Version:		Emulator R.41
; Compiler:		CC65
; Build using:	cl65 -t cx16 adsr.s -C cx16-asm.cfg -o ADSR.PRG

R0				= $02


VERA_LOW		= $9F20
VERA_MID		= $9F21
VERA_HIGH		= $9F22
VERA_DATA0		= $9F23
IRQ_VECTOR 		= $0314
OLD_IRQ_HANDLER = $06
PSG_CHANNEL		= $1F9FC
PSG_VOLUME		= PSG_CHANNEL + 2

.macro VERA_SET addr, increment
    lda #((^addr) | (increment << 4))
	sta VERA_HIGH
	lda #(>addr)
	sta VERA_MID
	lda #(<addr)
	sta VERA_LOW
.endmacro

.org $0400
;.org $9000							; Alternative memory location
.segment "CODE"


;*******************************************************************************
; ENTRY SECTION
;******************************************************************************
jmp ping 			; $400
jmp shoot			; $403
jmp zap				; $406
jmp explode			; $409
jmp generic			; $40c

; $40f load envelope

load_envelope:
;*******************************************************************************
;  Loads envelope data from R0 thru R5
;*******************************************************************************
	ldx #00						; index = 0
loop: 
	lda R0,x					; load next byte
	sta generic_envelope, x		; store next byte
	inx
	cpx #09						; all bytes moved?
	bne loop
	rts

ping:
;*******************************************************************************
	ldx #0
	jmp common

shoot:
; ******************************************************************************
	ldx #10
	jmp common

zap:
;*******************************************************************************
	ldx #20
	jmp common

explode:
;*******************************************************************************
	ldx #30
	jmp common

generic:
;*******************************************************************************
	ldx #40
	; fall through

common:
;*******************************************************************************
	ldy #0								; move 10 bytes from definitions
:	lda sounds,x
	sta channel15,y
	inx
	iny
	cpy #10
	bne :-

	lda #255							; Start playing
	sta phase

	lda running							; is IRQ player already running
	bne return

	sei 								; insert new IRQ player
	lda IRQ_VECTOR
	sta OLD_IRQ_HANDLER
    lda #<Play
    sta IRQ_VECTOR
    lda IRQ_VECTOR+1
    sta OLD_IRQ_HANDLER+1
    lda #>Play
    sta IRQ_VECTOR+1
    cli
	lda #1
	sta running

return:
	rts




; ******************************************************************************
; IRQ PLAY SECTION
; ******************************************************************************
Play:

	; sava VERA registers
	lda $9F20
	sta data_store
	lda $9F21
	sta data_store+1
	lda $9F22
	sta data_store+2
	lda $9F25
	sta data_store+3



	lda phase
	cmp #0								; if phase = 0 - Exit
	bne :+
	jmp exit

:	cmp #1								; if phase = 1 - Release
	bne :+
	jmp release

:	lda #1								; else phase = 255 - Start
	sta phase

	VERA_SET PSG_CHANNEL,1

	lda frequency						; read and set frequency
	sta VERA_DATA0
	lda frequency+1
	sta VERA_DATA0
	lda volume+1
	ora #%11000000						; stereo channel
	sta VERA_DATA0						; starting Volume  = volume
	lda waveform
	sta VERA_DATA0						; set waveform
	jmp exit

;*******************************************************************************
release:
;*******************************************************************************
	lda release_count
	bne release_loop					; not finished yet

	VERA_SET PSG_VOLUME,0
	stz VERA_DATA0						; set volume to 0 at the end of Release phase

	stz phase							; release finished, exit
    jmp exit

release_loop:

    sec									; decrease 16 bit volume
	lda volume
	sbc vol_change
	sta volume
	lda volume+1
	sbc vol_change+1
	sta volume+1

	sec									; decrease 16 bit frequency
	lda frequency
	sbc freq_change
	sta frequency
	lda frequency+1
	sbc freq_change+1
	sta frequency+1

	VERA_SET PSG_CHANNEL,1

	lda frequency						; read and set frequency
	sta VERA_DATA0
	lda frequency+1
	sta VERA_DATA0

    lda volume+1
    ora #%11000000
    sta VERA_DATA0						; read and set volume

	dec release_count

exit:
	; restore VERA registers
	lda data_store
	sta $9F20
	lda data_store+1
	sta $9F21
	lda data_store+2
	sta $9F22
	lda data_store+3
	sta $9F25

	jmp (OLD_IRQ_HANDLER)



; ******************************************************************************
;
; Variables
;
; ******************************************************************************
running:			.byte 0				; 0 - not running, 1 - running
phase:				.byte 0				; 0 - not playing, 255 - Start, 1 - Play Release

channel15:
release_count:      .byte 0				; byte 0

frequency:          .word 0				; bytes 1 and 2
waveform:			.byte 0				; byte 3

volume:				.word 0				; bytes 4 and 5
vol_change:			.word 0				; bytes 6 and 7
freq_change:		.word 0				; bytes 8 and 9

					;       C   Frequency	W		Volume		Vol Change		Freq Change
sounds:				;		0	1	  2		3		4	5		6		7		8		9
ping_envelope:		.byte 100, 	199,  9, 	160, 	0, 	63, 	161, 	0, 		0,   	0
shoot_envelope:		.byte  20, 	107,  17, 	224, 	0, 	63,   	0, 		3, 		0,   	0
zap_envelope:		.byte  37, 	232,  10,  	96, 	0, 	63, 	179, 	1, 		100, 	0
explode_envelope:	.byte 200, 	125,  5, 	224, 	0, 	63,  	80, 	0, 		0,   	0
generic_envelope:   .byte   0,  0,    0,   	0, 		0,  0,   	0, 		0, 		0,   	0

data_store:			.res 4,0

