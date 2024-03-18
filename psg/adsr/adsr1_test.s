.org $0600
.segment "CODE"

bra set_up_test_tone	; $0600

VERA_addr_low	= $9f20		; VERA
VERA_addr_high	= $9f21
VERA_addr_bank	= $9f22
VERA_data0		= $9f23
VERA_ctrl		= $9f25
VRAM_psg_voice7 = $1f9dc
MIDDLE_C		= 702		; Frequency
HIGH_FREQUENCY	= 7000

is_test_initialized: 	.byte 0

set_up_test_tone:
    lda is_test_initialized
    bne @initialized
    inc is_test_initialized

	stz VERA_ctrl
	lda #($10 | ^VRAM_psg_voice7)
	sta VERA_addr_bank
	lda #>VRAM_psg_voice7
	sta VERA_addr_high
	lda #<VRAM_psg_voice7
	sta VERA_addr_low
	lda #<MIDDLE_C
	sta VERA_data0
	lda #>MIDDLE_C
	sta VERA_data0
	lda #%11011111			; L&R + half volume
	sta VERA_data0
	;lda #$3f 				; pulse 50%
	lda #$C0  				; noise
	sta VERA_data0
@initialized:
	rts
