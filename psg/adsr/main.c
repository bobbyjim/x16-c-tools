#include <stdint.h>
#include <unistd.h>
#include <conio.h>
#include <6502.h>
#include <cbm.h>
#include <cx16.h>

#include "ADSR.h"

#define	TO_MAIN_RAM			0 
#define  TO_VERA_RAM       2

#define  STEREO_VOLUME(vol)            (vol + 196)
#define  ABI(x)                        ((unsigned char *)0x02[x]);

#define  PSG_WAVE_PULSE_SQUARE      63
#define  PSG_WAVE_PULSE_QUARTER     32
#define  PSG_WAVE_PULSE_EIGHTH      16
#define  PSG_WAVE_PULSE_SIXTEENTH   8
#define  PSG_WAVE_SAWTOOTH          (1 << 6)
#define  PSG_WAVE_TRIANGLE          (2 << 6)
#define  PSG_WAVE_NOISE             (3 << 6)

void set_PET_font()
{
   struct regs fontregs;
   fontregs.a = 4; // PET-like
   fontregs.pc = 0xff62;
   _sys(&fontregs);
}

void main()
{  
   set_PET_font();
   adsr_install();

//   cbm_k_setnam("adsr1test.prg");
//   cbm_k_setlfs(0,8,0);
//   LOAD_TO_MAIN_RAM(0x0600);

   VERA.control      = 0;              // port 0
   VERA.address      = 0xf9c0;         // voice 0
   VERA.address_hi   = VERA_INC_1 + 1; // from cx16.h
   VERA.data0        = 2000 & 0xff;    // freq 1
   VERA.data0        = 2000 >> 8;      // freq 2
   VERA.data0        = STEREO_VOLUME(20);
   VERA.data0        = PSG_WAVE_NOISE;

   adsr_setAttack(    0, 50);
   adsr_setDecay(     0, 100, 42);
   adsr_setSustain(   0, 100);
   adsr_setRelease(   0, 100);
   adsr_activateVoice(0,0);
   adsr_setHandler(ADSR_ON);
}
