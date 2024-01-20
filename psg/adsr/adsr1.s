.org $0400
.segment "CODE"

; ----------------------------------------------------------------------------
;
;  Envelope Settings
;
;  A possible way to set these is to use a single byte:
;    ASL the byte four times, shifting in the carry bit
;    into the upper register each time.
;
; ----------------------------------------------------------------------------
;                            Voice  0   1   2   3     4   5   6   7
;                                  ----------------------------------
state:              		.byte   0,  0,  0,  0,    0,  0,  0,  0 ; $0400
volume: 					.byte   0,  0,  0,  0,    0,  0,  0,  0 
volume_fractional:			.byte   0,  0,  0,  0,    0,  0,  0,  0 ; $0410
attack:             		.byte   0,  0,  0,  0,    0,  0,  0,  8 
attack_fractional:  		.byte  90, 80, 70, 60,    0,  0,  0,  0 ; $0420
decay:						.byte   0,  0,  0,  0,    0,  0,  0,  1 
decay_fractional:			.byte  60, 70, 80, 90,    0,  0,  0,  0 ; $0430
sustain_level:				.byte  40, 45, 50, 55,    0,  0,  0, 40
sustain_timer:      		.byte   1,  0,  0,  0,    0,  0,  0,  0 ; $0440
sustain_timer_fractional: 	.byte   0,  7,  6,  5,    0,  0,  0, 20
release:            		.byte   1,  2,  3,  1,    0,  0,  0,  0 ; $0450
release_fractional: 		.byte   0,  0,  0,  0,    0,  0,  0, 20

	; jump table
	jmp installer 			; $0460
	bra activate_voice 		; $0463 
	jmp voice_7_test		; $0465
	bra turn_handler_off	; $0468 
	bra turn_handler_on		; $046a 

ZP_REGISTERS	= $02
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
	sta ZP_REGISTERS; data_store			; try: pha
	lda VERA_addr_high
	sta ZP_REGISTERS+1; data_store+1		; try: pha
	lda VERA_addr_bank
	sta ZP_REGISTERS+2; data_store+2		; try: pha
	lda VERA_ctrl
	sta ZP_REGISTERS+3; data_store+3		; try: pha
.endmacro

.macro RESTORE_VERA_REGISTERS
	lda ZP_REGISTERS+3; data_store+3		; try: plp
	sta VERA_ctrl
	lda ZP_REGISTERS+2; data_store+2		; try: plp
	sta VERA_addr_bank
	lda ZP_REGISTERS+1; data_store+1		; try: plp
	sta VERA_addr_high
	lda ZP_REGISTERS; data_store			; try: plp
	sta VERA_addr_low
.endmacro

turn_handler_on:
	inc handler_is_active
	rts

turn_handler_off:
	stz handler_is_active
	rts

; ------------------------------------------------
;
;  	Set Volume and begin ADSR
;
;	x = voice 	
;	a = volume 
;
; ------------------------------------------------
activate_voice:
	sta volume,x
	lda #$ff
	sta volume_fractional,x
	lda #2					; State = Attack
	sta state,x
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

state_attack:
	plx       				; pop Voice X
	lda attack,x 
	adc attack_fractional,x 
	beq @state_attack_done  ; attack = 0000
	lda volume,x
	cmp #62
	bcs @state_attack_done  ; volume >= 62 
	lda volume_fractional,x ; not done
	adc attack_fractional,x ; vol.lo+=
	sta volume_fractional,x
	lda volume,x
	adc attack,x			; vol.hi+=
@state_attack_update_volume:
	sta volume,x
	bra return_from_jump
@state_attack_done:
	lda #4
	sta state,x 
    lda #63
	bra @state_attack_update_volume

state_decay:
	plx       				; pop Voice X
	lda decay,x 
	adc decay_fractional,x 
	beq @state_decay_done
	lda volume,x
	cmp sustain_level,x
	bcc @state_decay_done   ; volume < sustain_level
	lda volume_fractional,x ; not done
	sbc decay_fractional,x  ; vol.lo-=
	sta volume_fractional,x
	lda volume,x
	sbc decay,x				; vol.hi-=
@state_decay_update_volume:
	sta volume,x
	bra return_from_jump 
@state_decay_done:
	lda #6
	sta state,x 
	stz sustain_counter,x ; set up sustain
	stz sustain_counter_fractional,x
    lda sustain_level,x 
	bra @state_decay_update_volume

state_idle:
	plx       				; pop Voice X
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
    lda sustain_timer,x 
	beq @state_sustain_done ; no timer
	lda sustain_counter,x
	cmp sustain_timer,x
	bcs @state_sustain_done	; sustain_counter >= sustain_timer
	lda sustain_counter_fractional,x ; not done
	adc #01							 ; counter.lo++
	sta sustain_counter_fractional,x
	lda sustain_counter,x
	adc #00							 ; counter.hi += carry
	sta sustain_counter,x
	bra return_from_jump 
@state_sustain_done:
	lda #8
	sta state,x 
	bra return_from_jump 

state_release:
	plx       				; pop Voice X
	lda volume,x
	cmp #4
	bcc @state_release_done  ; volume < 4
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

; -----------------------------------------------------------------
;  Variables
; -----------------------------------------------------------------
;data_store:			.res 4,0  ; replaced by $02 $03 $04 $05
default_irq_vector: .addr 0
envelope_state: 	.addr state_idle
					.addr state_attack
					.addr state_decay
					.addr state_sustain
					.addr state_release

handler_is_active:	.byte 0		; OFF BY DEFAULT

sustain_counter:			.res 8,0	; INTERNAL counters
sustain_counter_fractional:	.res 8,0	;   for sustain timer
; -----------------------------------------------------------------

voice_7_test:				; 
	ldx #7					; voice
	lda #0					; vol
	jmp activate_voice

installer:					; 
   lda default_irq_vector	; installed already?
   bne @installed			; yes, done.
   lda IRQVec				; no, backup default RAM IRQ vector
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
   lda #$60			; rts
   sta installer
   rts
