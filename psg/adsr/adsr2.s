; Simplest Sound Effects Library for BASIC Programs
; Author:		Dusan Strakl
; More Info:	8BITCODING.COM
;
; Modified by:  Rob Eaglestone
;
; System:		Commander X16
; Version:		Emulator R.41
; Compiler:		CC65
; Build using:	cl65 -t cx16 adsr2.s -C cx16-asm.cfg -o ADSR2.PRG

.org $0400
.segment "STARTUP"
.segment "INIT"
.segment "ONCE"
.segment "CODE"

R0				= $02        ; the 16 bit ABI registers

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

.macro INSERT_SOUND_DRIVER
	sei 								; insert new IRQ player
	lda IRQ_VECTOR
	sta OLD_IRQ_HANDLER
    lda IRQ_VECTOR+1
    sta OLD_IRQ_HANDLER+1
    lda #<Play
    sta IRQ_VECTOR
    lda #>Play
    sta IRQ_VECTOR+1
    cli
	lda #1
	sta running
.endmacro

.macro SAVE_VERA_REGISTERS
	lda $9F20
	sta data_store
	lda $9F21
	sta data_store+1
	lda $9F22
	sta data_store+2
	lda $9F25
	sta data_store+3
.endmacro

.macro RESTORE_VERA_REGISTERS
	lda data_store
	sta $9F20
	lda data_store+1
	sta $9F21
	lda data_store+2
	sta $9F22
	lda data_store+3
	sta $9F25
.endmacro

;*******************************************************************************
; ENTRY SECTION
;******************************************************************************
jmp ping 			; $400
jmp shoot			; $403
jmp zap				; $406
jmp explode			; $409

ping:
;*******************************************************************************
	ldx #0
	jmp common

shoot:
; ******************************************************************************
	ldx #1
	jmp common

zap:
;*******************************************************************************
	ldx #2
	jmp common

explode:
;*******************************************************************************
	ldx #3
	jmp common


common:
;*******************************************************************************
	; move bytes from envelope buffers
	lda psg_release_count, x
	sta release_count
	lda psg_frequency_lo, x 
	sta frequency
	lda psg_frequency_hi, x 
	sta frequency+1
	lda psg_waveform, x
	sta waveform
	lda psg_volume_lo, x 
	sta volume
	lda psg_volume_hi, x 
	sta volume+1
	lda psg_vol_change_lo, x 
	sta vol_change
	lda psg_vol_change_hi, x 
	sta vol_change+1
	lda psg_freq_change_lo, x 
	sta freq_change
	lda psg_freq_change_hi, x 
	sta freq_change+1

	lda #255							; Start playing
	sta phase

	lda running							; is IRQ player already running
	bne return

    INSERT_SOUND_DRIVER

return:
	rts

; ******************************************************************************
; IRQ PLAY SECTION
; ******************************************************************************
Play:

    SAVE_VERA_REGISTERS

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
	ora #%11000000
	sta VERA_DATA0						; starting Volume  = volume
	lda waveform
	sta VERA_DATA0						; set waveform
	jmp exit

;*******************************************************************************
release:
;*******************************************************************************
	lda release_count
	bne release_loop					; not finished yet

	VERA_SET PSG_VOLUME, 0
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

	VERA_SET PSG_CHANNEL, 1

	lda frequency						; read and set frequency
	sta VERA_DATA0
	lda frequency+1
	sta VERA_DATA0

    lda volume+1
    ora #%11000000
    sta VERA_DATA0						; read and set volume

	dec release_count

exit:
    RESTORE_VERA_REGISTERS
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

;
;   0 = ping 
;   1 = shoot
;   2 = zap
;   3 = explode
;
;                       Voice    0    1    2    3   
;                       ----------------------------
psg_waveform:           .byte  160, 224,  96, 224
psg_envelope_state:     .byte    0,   0,   0,   0

psg_release_lo:    		.byte    0,   0,   0,   0
psg_release_hi:    		.byte    0,   0,   0,   0

psg_release_delta_lo:   .byte    0,   0,   0,   0
psg_release_delta_hi:   .byte    0,   0,   0,   0

psg_release_count:		.byte  100,  20,  37, 200
psg_frequency_lo:       .byte  199, 107, 232, 125
psg_frequency_hi:       .byte    9,  17,  10,   5
psg_volume_lo:          .byte    0,   0,   0,   0
psg_volume_hi:          .byte   63,  63,  63,  63
psg_vol_change_lo:      .byte  161,   0, 179,  80
psg_vol_change_hi:      .byte    0,   3,   1,   0
psg_freq_change_lo:     .byte    0,   0, 100,   0
psg_freq_change_hi:     .byte    0,   0,   0,   0

data_store:			.res 4,0
