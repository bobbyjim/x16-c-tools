.org $0400
.segment "CODE"

							; jump table
	jmp installer 			;   $0400
	jmp set_up_test_tone	;   $0403
	jmp set_test_volume		;   $0406

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
VRAM_psg_vol	= $1f9c2
MIDDLE_C		= 702		; Frequency
IRQVec          = $0314		; RAM Interrupt Vector

; envelope variables.        Voice  0   1   2   3
state:              		.byte   0,  0,  0,  0
volume: 					.byte   0,  0,  0,  0
volume_fractional:			.byte   0,  0,  0,  0
attack:             		.byte   0,  0,  0,  0
attack_fractional:  		.byte  90, 80, 70, 60
decay:						.byte   0,  0,  0,  0
decay_fractional:			.byte  60, 70, 80, 90
sustain_level:				.byte  40, 45, 50, 55
sustain_target:				.byte   5,  6,  7,  8
sustain_counter:			.byte   0,  0,  0,  0
sustain_counter_fractional:	.byte   0,  0,  0,  0
sustain_timer:      		.byte   0,  0,  0,  0
sustain_timer_fractional: 	.byte   8,  7,  6,  5
release:            		.byte   1,  2,  3,  1
release_fractional: 		.byte   0,  0,  0,  0

default_irq_vector: .addr 0
is_installed: 		.byte 0
is_initialized: 	.byte 0

installer:
   lda is_installed
   bne @installed
   inc is_installed

   lda IRQVec				; backup default RAM IRQ vector
   sta default_irq_vector
   lda IRQVec+1
   sta default_irq_vector+1

   sei 				
   lda #<handler	
   sta IRQVec				; overwrite RAM IRQ vector 
   lda #>handler
   sta IRQVec+1				; ...with custom handler address
   cli 
@installed:
   rts

set_up_test_tone:
    lda is_initialized
    bne @initialized
    inc is_initialized

	stz VERA_ctrl

	lda #($10 | ^VRAM_psg_voice2)
	sta VERA_addr_bank
	lda #>VRAM_psg_voice2
	sta VERA_addr_high
	lda #<VRAM_psg_voice2
	sta VERA_addr_low
	lda #<MIDDLE_C
	sta VERA_data0
	lda #>MIDDLE_C
	sta VERA_data0
	lda #%11011111			; L&R + half volume
	sta VERA_data0
	;lda #$3f 				; pulse 50%
	lda #$C0  				; sound
	sta VERA_data0
@initialized:
	rts

set_test_volume:
	ldx #2					; Voice 
	lda #32					; Vol at about 50%
	sta volume,x
	lda #$ff
	sta volume_fractional,x
	lda #2
	sta state,x
	rts

envelope_state: 	.addr state_idle
					.addr state_attack
					.addr state_decay
					.addr state_sustain
					.addr state_release

;----------------------------------------
handler:
	jsr run_states
	jsr update_volumes
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
	ldx #3
run_states_loop:
	lda state,x
	phx						; push Voice X
	tax						; clobber X
	jmp (envelope_state,x)
return_from_jump:
	; X ought to be restored at this point
	dex
	bpl run_states_loop
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
	lda #($30 | $08 | ^VRAM_psg_voice3)  ; stride = -4
	sta VERA_addr_bank
	lda #>(VRAM_psg_voice3 + 2)
	sta VERA_addr_high
	lda #<(VRAM_psg_voice3 + 2)
	sta VERA_addr_low

	ldx #3
@update_volume_loop:
	lda volume,x 
    ora #%11000000    ; L/R channel	(#192)
	sta VERA_data0
	dex
	bpl @update_volume_loop
	rts
