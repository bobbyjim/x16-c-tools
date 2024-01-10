.org $0400
.segment "CODE"

	; jump table
	jmp installer 		; $0400
	bra set_voice 		; $0403 
	bra set_envelope	; $0405
	jmp voice_7_test	; $0407 (was: 0409)
	bra turn_handler_off; $040a (was: 040c)
	bra turn_handler_on	; $040c (was: 040f)

VERA_addr_low	= $9f20		; VERA
VERA_addr_high	= $9f21
VERA_addr_bank	= $9f22
VERA_data0		= $9f23
VERA_data1		= $9f24
VERA_ctrl		= $9f25
VRAM_psg_voice0	= $1f9c0	; VRAM Addresses
VRAM_psg_voice1	= $1f9c4	
VRAM_psg_voice2	= $1f9c8
VRAM_psg_voice3	= $1f9cc	
VRAM_psg_voice7 = $1f9dc
VRAM_psg_vol	= $1f9c2
MIDDLE_C		= 702		; Frequency
IRQVec          = $0314		; RAM Interrupt Vector

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

turn_handler_on:
	inc handler_is_active
	rts

turn_handler_off:
	stz handler_is_active
	rts

; ------------------------------------------------
;
;  	Set Voice Volume and begin ADSR
;
;	x = voice 	
;	a = volume 
;
; ------------------------------------------------
set_voice:
	sta volume,x
	lda #$ff
	sta volume_fractional,x
	lda #2					; State = Attack
	sta state,x
	rts
	
; ------------------------------------------------
;
;  	Set Envelope for a given Voice.
;
;	x = voice 	
;   a = volume
;	$02 attack
;	$03 attack_fractional
;   $04 decay
;	$05 decay_fractional
;   $06 sustain_level
;	$07 sustain_target
;	$08 sustain_timer
;	$09 sustain_timer_fractional
;	$0a release
;	$0b release_fractional
;
; ------------------------------------------------
set_envelope:
    sta volume,x
	lda $02
	sta attack,x 
	lda $03
	sta attack_fractional,x 
	lda $04
	sta decay,x
	lda $05
	sta decay_fractional,x
	lda $06
	sta sustain_level,x
	lda $07
	sta sustain_target,x
	lda $08
	sta sustain_timer,x
	lda $09
	sta sustain_timer_fractional,x
	lda $0a
	sta release,x
	lda $0b
	sta release_fractional,x	
	rts

;----------------------------------------
handler:
	lda handler_is_active
	beq @inactive
	jsr run_states
	jsr update_volumes
@inactive:
    jmp (default_irq_vector) ; done
;----------------------------------------

state_idle:
	plx       				; pop Voice X
	bra return_from_jump

state_attack:
	plx       				; pop Voice X
	lda volume,x
	cmp #63
	beq @state_attack_done  ; done 
	lda volume_fractional,x ; not done
	adc attack_fractional,x ; vol.lo+=
	sta volume_fractional,x
	lda volume,x
	adc attack,x			; vol.hi+=
	sta volume,x
	bra return_from_jump
@state_attack_done:
	lda #4
	sta state,x 
	bra return_from_jump

state_decay:
	plx       				; pop Voice X
	lda volume,x
	cmp sustain_level,x
	beq @state_decay_done   ; done
	lda volume_fractional,x ; not done
	sbc decay_fractional,x  ; vol.lo-=
	sta volume_fractional,x
	lda volume,x
	sbc decay,x				; vol.hi-=
	sta volume,x
	bra return_from_jump 
@state_decay_done:
	lda #6
	sta state,x 
	stz sustain_counter,x ; set up sustain
	bra return_from_jump 

;------------------------------------------
;  Run States
;------------------------------------------
run_states:
    SAVE_VERA_REGISTERS
	ldx #7
run_states_loop:
	lda state,x
	phx						; push Voice X
	tax						; clobber X
	jmp (envelope_state,x)
return_from_jump:
	; X ought to be restored at this point
	dex
	bpl run_states_loop
	RESTORE_VERA_REGISTERS
	rts

state_sustain:
	plx       				; pop Voice X
	lda sustain_counter,x
	cmp sustain_target,x
	beq @state_sustain_done	; done
	lda sustain_counter_fractional,x ; not done
	adc sustain_timer_fractional,x   ; counter.lo+=
	sta sustain_counter_fractional,x
	lda sustain_counter,x
	adc sustain_timer,x				 ; counter.hi+=
	sta sustain_counter,x
	bra return_from_jump 
@state_sustain_done:
	lda #8
	sta state,x 
	bra return_from_jump 

state_release:
	plx       				; pop Voice X
	lda volume,x
	cmp #0
	beq @state_release_done  ; done
	cmp release,x
	bmi @state_release_done  ; done
	lda volume_fractional,x  ; not done
	sbc release_fractional,x ; vol.lo-=
	sta volume_fractional,x
	lda volume,x
	sbc release,x            ; vol.hi-=
	sta volume,x
	bra return_from_jump 
@state_release_done:
	stz volume,x
	stz state,x 			 ; idle
	bra return_from_jump 

;------------------------------------------
;  Update Volumes
;------------------------------------------
update_volumes:
	stz VERA_ctrl			       ; set to volume register
	lda #($30 | $08 | ^VRAM_psg_voice7)  ; stride = -4
	sta VERA_addr_bank
	lda #>(VRAM_psg_voice7 + 2)
	sta VERA_addr_high
	lda #<(VRAM_psg_voice7 + 2)
	sta VERA_addr_low

	ldx #7
@update_volume_loop:
	lda volume,x 
    ora #%11000000    ; L/R channel	(#192)
	sta VERA_data0
	dex
	bpl @update_volume_loop
	rts

data_store:			.res 4,0
default_irq_vector: .addr 0
envelope_state: 	.addr state_idle
					.addr state_attack
					.addr state_decay
					.addr state_sustain
					.addr state_release

handler_is_active:	.byte 0		; OFF BY DEFAULT

; envelope variables.        Voice  0   1   2   3     4   5   6   7
state:              		.byte   0,  0,  0,  0,    0,  0,  0,  0
volume: 					.byte   0,  0,  0,  0,    0,  0,  0,  0
volume_fractional:			.byte   0,  0,  0,  0,    0,  0,  0,  0
attack:             		.byte   0,  0,  0,  0,    0,  0,  0,  1
attack_fractional:  		.byte  90, 80, 70, 60,    0,  0,  0,  0
decay:						.byte   0,  0,  0,  0,    0,  0,  0,  1
decay_fractional:			.byte  60, 70, 80, 90,    0,  0,  0,  0
sustain_level:				.byte  40, 45, 50, 55,    0,  0,  0, 40
sustain_target:				.byte   5,  6,  7,  8,    0,  0,  0, 10
sustain_counter:			.byte   0,  0,  0,  0,    0,  0,  0,  0
sustain_counter_fractional:	.byte   0,  0,  0,  0,    0,  0,  0,  0
sustain_timer:      		.byte   0,  0,  0,  0,    0,  0,  0,  2
sustain_timer_fractional: 	.byte   8,  7,  6,  5,    0,  0,  0,  0
release:            		.byte   0,  2,  3,  1,    0,  0,  0,  0
release_fractional: 		.byte  16,  0,  0,  0,    0,  0,  0, 10

voice_7_test:				; $05e9
	ldx #7					; voice
	lda #20					; vol
	jmp set_voice

installer:					; $05f0
   lda default_irq_vector	; installed already?
   bne @installed			; yes, done.
   lda IRQVec				; no, backup default RAM IRQ vector
   sta default_irq_vector
   lda IRQVec+1
   sta default_irq_vector+1
   							; $0601
   sei 						
   lda #<handler	
   sta IRQVec				; overwrite RAM IRQ vector 
   lda #>handler
   sta IRQVec+1				; ...with custom handler address
   cli 
@installed:
   lda #$60			; rts
   sta installer
   rts
