; PSG ADSR Envelope Manager 
; Author:		Rob Eaglestone
;
; Bigfixed and optimized.
;
; Adapted from:
;    Simplest Sound Effects Library for BASIC Programs
;    Author:		Dusan Strakl
;    More Info:	https://www.8bitcoding.com/p/simplest-sound-effects-library-for.html
;
;
; System:		Commander X16
; Version:		Emulator R.48
; Compiler:		CC65
; Build using:	cl65 -t cx16 adsr5.s -C cx16-asm.cfg -o ADSR5.PRG

.org $0400
.segment "CODE"

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
	sta ZP_REGISTERS+28; data_store			; try: pha
	lda VERA_addr_high
	sta ZP_REGISTERS+29; data_store+1		; try: pha
	lda VERA_addr_bank
	sta ZP_REGISTERS+30; data_store+2		; try: pha
	lda VERA_ctrl
	sta ZP_REGISTERS+31; data_store+3		; try: pha
.endmacro

.macro RESTORE_VERA_REGISTERS
	lda ZP_REGISTERS+31; data_store+3		; try: plp
	sta VERA_ctrl
	lda ZP_REGISTERS+30; data_store+2		; try: plp
	sta VERA_addr_bank
	lda ZP_REGISTERS+29; data_store+1		; try: plp
	sta VERA_addr_high
	lda ZP_REGISTERS+28; data_store			; try: plp
	sta VERA_addr_low
.endmacro

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
;	bra set_envelope		; $046c			; removed for now

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
;	a = volume (peak volume to reach)
;
; ------------------------------------------------
activate_voice:
	lda #0
	sta volume,x
	sta volume_fractional,x
	lda #2					; State = Attack
	sta state,x
	rts

; ------------------------------------------------
;
;  	Set Envelope Values
; 	
;	x         = voice
;   $02 - $0d = envelope values
;
; ------------------------------------------------
;set_envelope:
;	lda ZP_REGISTERS
;	sta attack,X
;	lda ZP_REGISTERS+1
;	sta attack_fractional,X
;	lda ZP_REGISTERS+2
;	sta decay,X
;	lda ZP_REGISTERS+3
;	sta decay_fractional,x
;	lda ZP_REGISTERS+4
;	sta sustain_level,X
;	lda ZP_REGISTERS+5
;	sta sustain_timer,X
;	lda ZP_REGISTERS+6
;	sta sustain_timer_fractional,X
;	lda ZP_REGISTERS+7 
;	sta release,X
;	lda ZP_REGISTERS+8
;	sta release_fractional,X
;	rts

;----------------------------------------
handler:					
	lda handler_is_active	
	beq @inactive

	; 30 Hz: skip every other frame, but double-step each run to keep 60 steps/sec
	lda frame_counter
	eor #1
	sta frame_counter
	beq @inactive
	
	jsr run_states
	jsr run_states
	jsr update_volumes
	jsr update_volumes
@inactive:
    jmp (default_irq_vector) ; done
;----------------------------------------

state_attack:
	lda attack,y 
	ora attack_fractional,y 
	beq @state_attack_done  ; attack = 0000
	lda volume,y
	cmp #62
	bcs @state_attack_done  ; volume >= 62 
	clc
	lda volume_fractional,y ; not done
	adc attack_fractional,y ; vol.lo+=
	sta volume_fractional,y
	lda volume,y
	adc attack,y			; vol.hi+=
	sta volume,y
	rts
@state_attack_done:
	lda #4
	sta state,y 
    lda #63
	sta volume,y
	rts

state_decay:
	lda decay,y 
	ora decay_fractional,y 
	beq @state_decay_done
	lda volume,y
	cmp sustain_level,y
	bcc @state_decay_done   ; volume < sustain_level
	sec
	lda volume_fractional,y ; not done
	sbc decay_fractional,y  ; vol.lo-=
	sta volume_fractional,y
	lda volume,y
	sbc decay,y				; vol.hi-=
	sta volume,y
	rts
@state_decay_done:
	lda #6
	sta state,y
	lda #0
	sta sustain_counter,y ; set up sustain
	sta sustain_counter_fractional,y
    lda sustain_level,y 
	sta volume,y
	rts

state_idle:
	rts

;------------------------------------------
;  Run States
;------------------------------------------
run_states:
    SAVE_VERA_REGISTERS
	ldy #7
run_states_loop:
	lda state,y				; get state for voice Y
	tax						; state index in X
	jsr @dispatch			; call handler (Y preserved)
	dey
	bpl run_states_loop
	RESTORE_VERA_REGISTERS
	rts
@dispatch:
	jmp (envelope_state,x)	; Y=voice, X=state

state_sustain:
    lda sustain_timer,y 
	beq @state_sustain_done ; no timer
	lda sustain_counter,y
	cmp sustain_timer,y
	bcs @state_sustain_done	; sustain_counter >= sustain_timer
	lda sustain_counter_fractional,y ; not done
	adc #01							 ; counter.lo++ (C=0 from cmp/bcs)
	sta sustain_counter_fractional,y
	lda sustain_counter,y
	adc #00							 ; counter.hi += carry
	sta sustain_counter,y
	rts
@state_sustain_done:
	lda #8
	sta state,y 
	rts 

state_release:
	lda volume,y
	cmp #4
	bcc @state_release_done  ; volume < 4
	sec
	lda volume_fractional,y  ; not done
	sbc release_fractional,y ; vol.lo-=
	sta volume_fractional,y
	lda volume,y
	sbc release,y            ; vol.hi-=
	sta volume,y
	rts
@state_release_done:
	lda #0
	sta volume,y
	sta state,y 			 ; idle
	rts 

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
default_irq_vector: .addr 0
envelope_state: 	.addr state_idle
					.addr state_attack
					.addr state_decay
					.addr state_sustain
					.addr state_release

handler_is_active:	.byte 0		; OFF BY DEFAULT
frame_counter:		.byte 0		; for 30 Hz gating

sustain_counter:			.res 8,0	; INTERNAL counters
sustain_counter_fractional:	.res 8,0	;   for sustain timer
; -----------------------------------------------------------------
;
;  Everything after this point can be safely overwritten 
;  after installation.
;
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
