#include <6502.h>

#include "common.h"
#include "PSG.h"
#include "test5.h"

unsigned char test51_boom()
{
	Voice voice;
	struct regs r;

	// jmp env_run 	  ; $0403
	// jmp player 	  ; $0406
	// installer      ; $04f2
	// set up adsr    ; $0409

	r.pc =  ADDR_INSTALLER; // for some reason this doesn't work
	r.flags = 0;

    //dumpABI();
	//_sys(&r);
	//dumpABI();

	voice.frequency = 1600;
	voice.channel   = PSG_CHANNEL_BOTH;
	voice.volume    = 50;
	voice.waveform  = PSG_WAVE_NOISE;

	runVoice( 0, &voice );

	return 0;
}