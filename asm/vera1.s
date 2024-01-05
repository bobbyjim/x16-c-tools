.org $0400
.segment "STARTUP"
.segment "INIT"
.segment "ONCE"
.segment "CODE"

	jmp start 

; VERA
VERA_addr_low	= $9f20
VERA_addr_high	= $9f21
VERA_addr_bank	= $9f22
VERA_data0		= $9f23
VERA_data1		= $9f24
VERA_ctrl		= $9f25
VERA_ien		= $9f26
VSYNC_BIT		= $01

; VRAM Addresses
VRAM_psg		= $1f9c0
VRAM_psg_vol	= $1f9c2

; Frequency
MIDDLE_C		= 702
FREQ_STEP		= 20

; RAM Interrupt Vectors
IRQVec            = $0314

; globals
volume: 			.res 16,0
volume_fractional:	.res 16,0
default_irq_vector: .addr 0

start:
	stz VERA_ctrl
	lda #($10 | ^VRAM_psg)
	sta VERA_addr_bank
	lda #>VRAM_psg
	sta VERA_addr_high
	lda #<VRAM_psg
	sta VERA_addr_low
	lda #<MIDDLE_C
	sta VERA_data0
	lda #>MIDDLE_C
	sta VERA_data0
	lda #$ff ; L&R full volume
	sta VERA_data0
	lda #$3f ; pulse 50%
	sta VERA_data0

	; enable only VSYNC IRQs
	lda #VSYNC_BIT
	sta VERA_ien

	; set to volume register
	stz VERA_ctrl
	lda #^VRAM_psg_vol; stride = 0
	sta VERA_addr_bank
	lda #>VRAM_psg_vol
	sta VERA_addr_high
	lda #<VRAM_psg_vol
	sta VERA_addr_low

@vol_down_loop:
	; reset volume registers 
	lda #255
	ldx #0
	sta volume,x
	sta volume_fractional,x
	; now call timer
	jsr @timer
	lda VERA_data0 
	cmp #(128+64) ; vol = 0
	beq @done
	dec VERA_data0
	bra @vol_down_loop

@done:
	rts

@timer:
    ldx #0
	lda volume_fractional,x
	sbc #01
	sta volume_fractional,x
	lda volume,x
	beq @done
	sbc #00
	sta volume,x
	bra @timer
