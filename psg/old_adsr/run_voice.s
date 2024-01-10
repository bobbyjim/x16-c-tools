; Build using:	cl65 -t cx16 run_voice.s -C cx16-asm.cfg -o RUNVOZ.PRG

.org $0400
.segment "CODE"

PSG_CHANNEL		= $1F9FC
PSG_VOLUME		= PSG_CHANNEL + 2
VERA_LOW		= $9F20
VERA_MID		= $9F21
VERA_HIGH		= $9F22
INCREMENT1		= 1

.macro VERA_SET addr, increment
    lda #((^addr) | (increment << 4))
	sta VERA_HIGH
	lda #(>addr)
	sta VERA_MID
	lda #(<addr)
	sta VERA_LOW
.endmacro

;
;   Channels and Volume examples
; 
PSG_CHANNEL_OFF      =   0
PSG_CHANNEL_LEFT     =   64
PSG_CHANNEL_RIGHT    =   128
PSG_CHANNEL_STEREO   =   192

PSG_VOLUME_KNOB_11   =   63
PSG_VOLUME_KNOB_10   =   58
PSG_VOLUME_KNOB_9    =   52
PSG_VOLUME_KNOB_8    =   46
PSG_VOLUME_KNOB_7    =   40
PSG_VOLUME_KNOB_6    =   35
PSG_VOLUME_KNOB_5    =   29
PSG_VOLUME_KNOB_4    =   23
PSG_VOLUME_KNOB_3    =   17
PSG_VOLUME_KNOB_2    =   11
PSG_VOLUME_KNOB_1    =   6
PSG_VOLUME_KNOB_0    =   0
PSG_VOLUME_OFF       =   0
; 
;   Waveforms and Pulse Widths
;
PSG_WAVE_PULSE       =   0
PSG_WAVE_SAWTOOTH    =   64
PSG_WAVE_TRIANGLE    =   128
PSG_WAVE_NOISE       =   192

PSG_PULSE_SQUARE     =   63
PSG_PULSE_QUARTER    =   32
PSG_PULSE_EIGHTH     =   16
PSG_PULSE_SIXTEENTH  =   8
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;

VERA_DATA0			= $9F23
VERA_CTRL			= $9f25

FREQ_LO				= $02
FREQ_HI				= $03
CHANNEL_AND_VOL		= $04
WAVE_AND_PULSE		= $05

test:
    lda #0		; freq lo
	sta FREQ_LO
	lda #8		; freq hi (=4096)
	sta FREQ_HI
	lda PSG_CHANNEL_STEREO + PSG_VOLUME_KNOB_8
	sta CHANNEL_AND_VOL
	lda PSG_PULSE_SQUARE
	sta WAVE_AND_PULSE
	;
	;  don't forget the VOICE NUMBER
	;

doit:
    VERA_SET PSG_CHANNEL, INCREMENT1
	lda FREQ_LO			
	sta VERA_DATA0
	lda FREQ_HI			
	sta VERA_DATA0
	lda CHANNEL_AND_VOL	
	sta VERA_DATA0
	lda WAVE_AND_PULSE	
	sta VERA_DATA0

    rts
