;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   what: env_api.s 
;   why : envelope module
;   .include "env_api.s" 
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
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

.macro MACRO_ADVANCE_STATE
	inx             ; advance
	inx             ;   the
	stx ENV_STATE   ;   state
.endmacro

env_reset:
	stz ENV_STATE
	rts

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;  run envelope
;  operates on ABI registers.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
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
vol_in_range:
	sta ENV_VOLUME_HI
	rts 

do_decay:  ; vol -= decay
	lda ENV_VOLUME_LO     
	sbc ENV_DECAY_LO      
	sta ENV_VOLUME_LO

	lda ENV_VOLUME_HI 
	sbc ENV_DECAY_HI
	cmp ENV_SUSTAIN_LEVEL
	bcs vol_in_range ; not yet
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
    ; always dec timer_lo!
	dec ENV_SUS_TIMER_LO
	lda ENV_SUS_TIMER_HI
	beq advance_state
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
; 	Envelope API END
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
