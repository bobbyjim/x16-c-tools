;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;     sound envelope   
;     Commander X16 R.43
;     Compiler:		CC65
;     cl65 	-t cx16 ssnd.s 
;			-C cx16-asm.cfg 
;			-o SSND.PRG
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.org $0400
.segment "CODE"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;	Constants
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; these get updated
;
ENV_STATE			= $02
ENV_VOLUME_LO		= $03
ENV_VOLUME_HI		= $04
ENV_SUS_TIMER_LO	= $05
ENV_SUS_TIMER_HI	= $06
;
; these do not get updated
;
ENV_ATTACK_LO		= $07
ENV_ATTACK_HI		= $08
ENV_DECAY_LO		= $09
ENV_DECAY_HI		= $0a
ENV_SUSTAIN_LEVEL	= $0b
ENV_SUSTAIN_LO		= $0c
ENV_SUSTAIN_HI		= $0d
ENV_RELEASE_LO		= $0e
ENV_RELEASE_HI		= $0f
ENV_VOICE 			= $10

VERA_LOW			= $9F20
VERA_MID			= $9F21
VERA_HIGH			= $9F22
VERA_DATA0			= $9F23
VERA_CTRL			= $9f25
PSG_CHANNEL_LO		= $C0

IRQ_VECTOR 			= $0314
OLD_IRQ_HANDLER 	= $06

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;	Macros
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro SAVE_VERA_REGISTERS
	lda VERA_LOW
	sta psg_store
	lda VERA_MID
	sta psg_store+1
	lda VERA_HIGH
	sta psg_store+2
	lda VERA_CTRL
	sta psg_store+3
.endmacro

.macro RESTORE_VERA_REGISTERS
	lda psg_store
	sta VERA_LOW
	lda psg_store+1
	sta VERA_MID
	lda psg_store+2
	sta VERA_HIGH
	lda psg_store+3
	sta VERA_CTRL
.endmacro

.macro SET_UP_VERA
	stz VERA_CTRL ; channel 0
	;
	; stride = 4
	; direction = negative
	; VRAM bank = 1
	;
    lda #($30 | $08 | $01)
	sta VERA_HIGH
	lda #$f9 
	sta VERA_MID 
	;
	; start with voice 15
	; 
	lda #($C0 + (4 * 15) + 2) 
	sta VERA_LOW
.endmacro

.macro WRITE_VOLUME
	lda ENV_VOLUME_HI
    ora #%11000000  ; stereo
	sta VERA_DATA0
.endmacro

.macro MACRO_ADVANCE_STATE
	inx             ; advance
	inx             ;   the
	stx ENV_STATE   ;     state
.endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;  Jump Table
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
jmp env_reset	; $0400
jmp env_run 	; $0403
jmp player 		; $0406
jmp ssnd		; $0409
jmp lsnd		; $040c
jmp usnd		; $040f

; $0412
;
;   Move this when the code is done-done.
;
psg_data:							; 224 bytes
state:				.res 16,0
volume_lo:			.res 16,0
volume_hi:			.res 16,0
sustain_timer_lo:	.res 16,0 
sustain_timer_hi:	.res 16,0
attack_lo:			.res 16,0
attack_hi:			.res 16,0
decay_lo:			.res 16,0
decay_hi:			.res 16,0
sustain_level:		.res 16,0
sustain_lo:			.res 16,0
sustain_hi:			.res 16,0
release_lo:			.res 16,0
release_hi:			.res 16,0

; installer     
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;  Installer
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
installer:
	sei						; 1b
	lda IRQ_VECTOR			; 3b
	sta OLD_IRQ_HANDLER		; 3b
    lda #<player			; 3b
    sta IRQ_VECTOR			; 3b
    lda IRQ_VECTOR+1		; 3b
    sta OLD_IRQ_HANDLER+1	; 3b
    lda #>player			; 2b
    sta IRQ_VECTOR+1		; 3b
    cli						; 1b

	; write "RTS" to installer	
	; to re-enable install,
	; just write "SEI" (#$78) 
	; back to installer 
	lda #$60
	sta installer	
	rts

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   Player
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
player:
    SAVE_VERA_REGISTERS 
	SET_UP_VERA 
	ldx #15
	stx ENV_VOICE
do_one_voice:
	jsr lsnd    ; load sound
	jsr env_run
	jsr usnd    ; update sound
	WRITE_VOLUME  
	dec ENV_VOICE
	bne do_one_voice
	RESTORE_VERA_REGISTERS 
	rts

;
;
;   load ABI from ADSR
;   ** voice number is in X **
;
;
lsnd:
	stx ENV_VOICE
	lda state,X
	sta ENV_STATE
	lda volume_lo,X
	sta ENV_VOLUME_LO
	lda volume_hi,X
	sta ENV_VOLUME_HI
	lda sustain_timer_lo,X
	sta ENV_SUS_TIMER_LO
	lda sustain_timer_hi,X
	sta ENV_SUS_TIMER_HI
	lda attack_lo,X
	sta ENV_ATTACK_LO
	lda attack_hi,X
	sta ENV_ATTACK_HI
	lda decay_lo,X
	sta ENV_DECAY_LO
	lda decay_hi,X
	sta ENV_DECAY_HI
	lda sustain_level,X
	sta ENV_SUSTAIN_LEVEL
	lda sustain_lo,X
	sta ENV_SUSTAIN_LO
	lda sustain_hi,X
	sta ENV_SUSTAIN_HI
	lda release_lo,X
	sta ENV_RELEASE_LO
	lda release_hi,X
	sta ENV_RELEASE_HI
	rts

;
; 	save ABI to ADSR
;	voice # is ENV_VOICE
;
ssnd:
    ldx ENV_VOICE
	lda ENV_ATTACK_LO
	sta attack_lo,x			
	lda ENV_ATTACK_HI
	sta attack_hi,x			
	lda ENV_DECAY_LO
	sta decay_lo,x			
	lda ENV_DECAY_HI
	sta decay_hi,x			
	lda ENV_SUSTAIN_LEVEL
	sta sustain_level,x		
	lda ENV_SUSTAIN_LO
	sta sustain_lo,x		
	lda ENV_SUSTAIN_HI
	sta sustain_hi,x		
	lda ENV_RELEASE_LO
	sta release_lo,x		
	lda ENV_RELEASE_HI
	sta release_hi,x		
;
;   update sound state only
;
usnd:
    ldx ENV_VOICE
	lda ENV_STATE
	sta state,x				
	lda ENV_VOLUME_LO
	sta volume_lo,x			
	lda ENV_VOLUME_HI
	sta volume_hi,x			
	lda ENV_SUS_TIMER_LO	
	sta sustain_timer_lo,x	
	lda ENV_SUS_TIMER_HI	
	sta sustain_timer_hi,x	
	rts

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;  Envelope
;  operates on ABI registers.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
env_reset:
	stz ENV_STATE
	rts

env_run:
	ldx ENV_STATE
	beq skip
	jmp (env_handlers,x)

advance_state:
    MACRO_ADVANCE_STATE
skip:
	rts

env_handlers: 
	.word skip			
	.word do_attack		
	.word do_decay		
	.word do_sustain 	
	.word do_release 	
	.word env_reset

do_attack:	; vol += attack;
	lda ENV_VOLUME_LO 	
	adc ENV_ATTACK_LO 
	sta ENV_VOLUME_LO 

	lda ENV_VOLUME_HI 
	adc ENV_ATTACK_HI
	;
	; Check if vol is in range
	;
	cmp #64	; vol > 64?
	bcc vol_in_range ; no
	MACRO_ADVANCE_STATE
	lda #63 ; vol = max
vol_in_range: ; shared with decay at this point
	sta ENV_VOLUME_HI
	rts 

do_decay:  ; vol -= decay
    sec    ; $062f
	lda ENV_VOLUME_LO     
	sbc ENV_DECAY_LO      
	sta ENV_VOLUME_LO

	lda ENV_VOLUME_HI 
	sbc ENV_DECAY_HI
	;;;;;;;;;;;;;;;;;;;;;;;;;;
	;  shall we reset if 
	;  the volume reaches 0 ?
	;;;;;;;;;;;;;;;;;;;;;;;;;;
	beq env_reset
	cmp ENV_SUSTAIN_LEVEL
	bcs vol_in_range ; no, not yet
	;
	; yes = prepare sustain
	;
	MACRO_ADVANCE_STATE
	lda ENV_SUSTAIN_LO
	sta ENV_SUS_TIMER_LO
	lda ENV_SUSTAIN_HI
	sta ENV_SUS_TIMER_HI 
	;
	; fall through
	;
do_sustain: ; sustain is a timer
    lda ENV_SUS_TIMER_LO
	beq check_sustain_hi
	rts
check_sustain_hi:
	dec ENV_SUS_TIMER_LO ; always do this
	lda ENV_SUS_TIMER_HI
	beq advance_state ; zero = go to release
	dec ENV_SUS_TIMER_HI
	rts

do_release:
	lda ENV_VOLUME_HI 
	beq env_reset	; == 0
	sec 				; > 0
	lda ENV_VOLUME_LO 
	sbc ENV_RELEASE_LO  ; - rls
	sta ENV_VOLUME_LO 
	lda ENV_VOLUME_HI 
	sbc ENV_RELEASE_HI 
	bcc env_reset
	sta ENV_VOLUME_HI
	rts

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;	Data
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
psg_store:			.byte 4, 3, 2, 1
