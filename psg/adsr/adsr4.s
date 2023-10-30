; ADSR Envelope Manager
; Author:		Rob Eaglestone
;
; Adapted from:
;    Simplest Sound Effects Library for BASIC Programs
;    Author:		Dusan Strakl
;    More Info:	https://www.8bitcoding.com/p/simplest-sound-effects-library-for.html
;
;
; System:		Commander X16
; Version:		Emulator R.41
; Compiler:		CC65
; Build using:	cl65 -t cx16 adsr4.s -C cx16-asm.cfg -o ADSR4.PRG

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   Driver Injector
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
IRQ_VECTOR 					= $0314
OLD_IRQ_HANDLER 			= $06

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   PSG Registers
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
VERA_LOW					= $9F20
VERA_MID					= $9F21
VERA_HIGH					= $9F22
VERA_DATA0					= $9F23
PSG_CHANNEL_LO				= $C0

.macro SAVE_VERA_REGISTERS
	lda VERA_LOW
	sta data_store
	lda VERA_MID
	sta data_store+1
	lda VERA_HIGH
	sta data_store+2
	lda $9F25
	sta data_store+3
.endmacro

.macro RESTORE_VERA_REGISTERS
	lda data_store
	sta VERA_LOW
	lda data_store+1
	sta VERA_MID
	lda data_store+2
	sta VERA_HIGH
	lda data_store+3
	sta $9F25
.endmacro

.macro WRITE_VOLUMES
    ;
	; write current volume+1 levels into channels 0-3 (descending order)
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
	; start with volume+1 register of channel 3
	; 
	lda #($C0 + (4 * 3) + 2) 
	sta VERA_LOW
	ldx #6
@loop:
	lda volume,x
    ora #%11000000    	; L/R channel	
	sta VERA_DATA0
	dex
	dex
	bpl @loop		  ; includes zero ("negative clear")
.endmacro

.org $0400
;.org $9000							; Alternative memory location
.segment "CODE"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;  installer
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
installer:
	sei									; 1 byte
	lda IRQ_VECTOR						; 3 bytes
	sta OLD_IRQ_HANDLER					; 3 bytes
    lda #<player						; 2 bytes
    sta IRQ_VECTOR						; 3 bytes
    lda IRQ_VECTOR+1					; 3 bytes
    sta OLD_IRQ_HANDLER+1				; 3 bytes
    lda #>player						; 2 bytes
    sta IRQ_VECTOR+1					; 3 bytes
    cli									; 1 byte
	lda #$60
	sta installer						; write "RTS" to $install	
										; to re-enable install,
										; just write "SEI" (#$78) back to $install
	rts

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;  variables
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
data_store:				.res  4,0			; R = $041C
system_has_work:		.byte 1				; R+4 

;
;  envelope data starts at $0426
;							0		1		2		3
;                          L  H    L  H    L  H    L  H
envelope_state:		.byte  1, 0,   2, 0,   3, 0,   7, 0		; R+5 
volume:				.byte 61,60,  59,58,  57,56,  55,54		; R+13
attack:				.byte  4, 0,   5, 0,   6, 0,   7, 0		; R+21
decay:				.byte  8, 0,   9, 0,  10, 0,  11, 0		; R+29
timer:				.byte 30, 0,  40, 0,  50, 0,  60, 0		; R+37
sustain:			.byte 10, 0,  12, 0,  14, 0,  16, 0		; R+45
release:			.byte  8, 0,  12, 0,  16, 0,  20, 0		; R+53 .. R+60

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;  player BEGIN
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
player:
	lda system_has_work
	beq player_done   				; 0 = OFF
    SAVE_VERA_REGISTERS				; ok, ON then
	ldy #6							; voice 3
	jsr try_envelope			
	ldy #4							; voice 2
	jsr try_envelope
	ldy #2							; voice 1
	jsr try_envelope
	ldy #0							; voice 0
	jsr try_envelope			
	WRITE_VOLUMES										
    RESTORE_VERA_REGISTERS								
player_done:
    jmp (OLD_IRQ_HANDLER)

adsr_handlers: .word goofing_off, try_attack, try_decay, try_sustain, try_release

;
;  Try to run an envelope
;
try_envelope: 										
	lda envelope_state,y
	cmp #5
	bcc envelope_state_is_valid
	jmp reset_envelope
envelope_state_is_valid: 							
    asl 							; double A's value
	tax 							; transfer state into x
	jmp (adsr_handlers,x)

goofing_off:						; state 0
	rts

try_attack:							; state 1
    ;stp
	bra advance_state

try_decay:							; state 2
    ;stp
	bra advance_state	

try_sustain:						; state 3
    ;stp
	bra advance_state	

try_release:						; state 4
    ;stp
	lda volume+1,y					; = $437 (high byte) when x=0
	beq reset_envelope				;   If zero, reset envelope
	sec
	lda volume,y					; = $436 (low byte) when x=0
	sbc release,y
	sta volume,y
	lda volume+1,y 					; = $437 (high byte) when x=0
	sbc release+1,y
    bcc advance_state
	sta volume+1,y
	rts

advance_state:
	;stp
	tya								; transfer index
	tax								; into x
	inc envelope_state,x
	rts 

reset_envelope:		
	tya								; transfer index
	tax								; into x
	stz volume,x     
	stz volume+1,x  				; Silent
	stz envelope_state,x  			; disabled or off
	rts

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;  player END
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

