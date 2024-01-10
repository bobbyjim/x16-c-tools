;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; ADSR Envelope Manager by Rob Eaglestone
;
; Adapted from:
;    Simplest Sound Effects Library for BASIC Programs, by Dusan Strakl
;    https://www.8bitcoding.com/p/simplest-sound-effects-library-for.html
;
;
; Commander X16 R.46
; Build using CC65:	cl65 -t cx16 adsr3.s -C cx16-asm.cfg -o ADSR3.PRG
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.org $0400
.segment "STARTUP"
.segment "INIT"
.segment "ONCE"
.segment "CODE"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   PSG Code
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
VERA_addr_low		= $9f20
VERA_addr_high		= $9f21
VERA_addr_bank		= $9f22
VERA_data0			= $9f23
VERA_ctrl			= $9f25

VERA_voice_3_vol	= $1f9ce

.macro SAVE_VERA_REGISTERS
	lda VERA_addr_low
	sta data_store
	lda VERA_addr_high
	sta data_store+1
	lda VERA_addr_bank
	sta data_store+2
	lda VERA_ctrl
	sta data_store+3
.endmacro

.macro RESTORE_VERA_REGISTERS
	lda data_store
	sta VERA_addr_low
	lda data_store+1
	sta VERA_addr_high
	lda data_store+2
	sta VERA_addr_bank
	lda data_store+3
	sta VERA_ctrl
.endmacro

.macro WRITE_VOLUMES
	stz VERA_ctrl
	; stride = -4
    lda #($30 | $08 | ^VERA_voice_3_vol)
	sta VERA_addr_bank
	lda #<VERA_voice_3_vol 
	sta VERA_addr_high 
	lda #>VERA_voice_3_vol
	sta VERA_addr_low
	ldx #3
@loop:
	lda volume,x 
    ora #%11000000    ; L/R channel	
	sta VERA_data0
	dex
	bpl @loop
.endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;  installer
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
IRQ_VECTOR 					= $0314
OLD_IRQ_HANDLER 			= $06

install:
	lda system_is_installed		
	bne installed				
	sei									; 1 byte
	lda IRQ_VECTOR						; 3 bytes
	sta OLD_IRQ_HANDLER					; 3 bytes
    lda #<player						; 3 bytes
    sta IRQ_VECTOR						; 3 bytes
    lda IRQ_VECTOR+1					; 3 bytes
    sta OLD_IRQ_HANDLER+1				; 3 bytes
    lda #>player						; 2 bytes
    sta IRQ_VECTOR+1					; 3 bytes
    cli									; 1 byte
	lda #1
	sta system_is_installed
installed:
	rts

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;  variables
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
data_store:				.res 4,0			; R
system_is_installed:	.byte 0				; R+4
system_has_work:		.byte 1				; R+5

;
;  envelopes
;
envelope_state:			.byte 4, 0, 0, 0	; R+6 .. R+9

volume:					.byte 0, 0, 0, 0	; R+10 .. R+13
volume_lo:				.byte 0, 0, 0, 0    ; R+14 .. R+17

vol_rls_hi:				.byte 1, 0, 0, 0	; R+18 .. R+21
vol_rls_lo:				.byte 0,80,40,20	; R+22 .. R+25

vol_sus_lvl:			.byte 30,30,30,30   ; R+26 .. R+29
vol_sus_timer_hi:		.byte 0,  0, 0, 0    ; R+30 .. R+33
vol_sus_timer_lo:		.byte 50, 0, 0, 0   ; R+34 .. R+37

vol_dec_hi:				.byte 0, 0, 0, 0	; R+38 .. R+41
vol_dec_lo:				.byte 90,90,90,90 	; R+42 .. R+45

vol_att_hi:				.byte  0, 0, 0, 0	; R+46 .. R+49
vol_att_lo:				.byte 190,190,190,190	; R+50 .. R+53

sustain_timer:			.byte 0, 0, 0, 0    ; R+54
sustain_timer_lo:		.byte 0, 0, 0, 0    ; R+58

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;  player BEGIN
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
player:
	lda system_has_work
	beq player_done   		;  0 = NOT running
    SAVE_VERA_REGISTERS
    ldx #00	
	jsr try_envelope
	inx
	jsr try_envelope
	inx
	jsr try_envelope
	inx
	jsr try_envelope
	WRITE_VOLUMES
    RESTORE_VERA_REGISTERS
player_done:
    jmp (OLD_IRQ_HANDLER)

;
;  Try to run an envelope
;
try_envelope:
	lda envelope_state,x 
	bne try_attack
	rts

try_attack:
    cmp #01
	bne try_decay
	; implement attack
	lda volume_lo,x 
	adc vol_att_lo,x 
	sta volume_lo,x 
	lda volume,x 
	adc vol_att_hi,x 
	;
	;  We have to see if A > 64.
	;
	cmp #64              ; max vol
	bcc vol_in_range     ; A < 64
	lda #64
	sta volume,x		 ; set vol to max
	bra advance_state    ; A >= 64
vol_in_range:
    sta volume,x         ; else OK
	rts 

try_decay:
    cmp #02
	bne try_sustain
	; implement decay
	lda volume_lo,x 
	sbc vol_dec_lo,x 
	sta volume_lo,x 
	lda volume,x 
	sbc vol_dec_hi,x 
	cmp vol_sus_lvl,x
	bcc prepare_sustain		; at or a bit below sustain level
	sta volume,x
	rts
prepare_sustain:
	lda vol_sus_timer_lo,x 
	sta sustain_timer_lo,x 
	lda vol_sus_timer_hi,x 
	sta sustain_timer,x
    bra advance_state

try_sustain:
    cmp #03
	bne try_release
	dec sustain_timer_lo,x 
	lda sustain_timer_lo,x 
	beq sustain_rollover
	rts
sustain_rollover:
	lda sustain_timer,x
	beq advance_state
	dec sustain_timer,x
	rts

try_release:		
	cmp #04							; release?
	bne reset_envelope				; no, this state is invalid
	; apply release
	lda volume,x       ; sets Z
	beq reset_envelope ; checks Z
	sec
	lda volume_lo,x
	sbc vol_rls_lo,x
	sta volume_lo,x
	lda volume,x 
	sbc vol_rls_hi,x
    bcc advance_state
	sta volume,x
	rts

advance_state:
	inc envelope_state,x
	rts 

reset_envelope:	
	stz volume,x          ; Silent
	stz envelope_state,x  ; OFF
	rts

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;  player END
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
