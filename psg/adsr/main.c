#include <stdint.h>
#include <unistd.h>
#include <conio.h>
#include <6502.h>
#include <cbm.h>
#include <cx16.h>

#define  LOAD_TO_MAIN_RAM(addr)        cbm_k_load(0,addr)
#define  LOAD_TO_VERA(addr)            cbm_k_load(2,addr)
#define  STEREO_VOLUME(vol)            (vol + 196)
#define  ABI(x)                        ((unsigned char *)0x02[x]);

#define     PSG_WAVE_PULSE          0
#define     PSG_WAVE_SAWTOOTH       (1 << 6)
#define     PSG_WAVE_TRIANGLE       (2 << 6)
#define     PSG_WAVE_NOISE          (3 << 6)

#define     PSG_PULSE_SQUARE        63
#define     PSG_PULSE_QUARTER       32
#define     PSG_PULSE_EIGHTH        16
#define     PSG_PULSE_SIXTEENTH     8

#define  ADSR_INSTALLER       0x0460
#define  ADSR_ACTIVATE_VOICE  0x0463
#define  ADSR_VOICE7_TEST     0x0465
#define  ADSR_HANDLER_OFF     0x0468
#define  ADSR_HANDLER_ON      0x046a

#define  ADSR_STATE                    ((unsigned char*) (0x400))
#define  ADSR_VOLUME                   ((unsigned char*) (0x408))
#define  ADSR_VOLUME_FRACTIONAL        ((unsigned char*) (0x410))
#define  ADSR_ATTACK                   ((unsigned char*) (0x418))
#define  ADSR_ATTACK_FRACTIONAL        ((unsigned char*) (0x420))
#define  ADSR_DECAY                    ((unsigned char*) (0x428))
#define  ADSR_DECAY_FRACTIONAL         ((unsigned char*) (0x430))
#define  ADSR_SUSTAIN_LEVEL            ((unsigned char*) (0x438))
#define  ADSR_SUSTAIN_TIMER            ((unsigned char*) (0x440))
#define  ADSR_SUSTAIN_TIMER_FRACTIONAL ((unsigned char*) (0x448))
#define  ADSR_RELEASE                  ((unsigned char*) (0x450))
#define  ADSR_RELEASE_FRACTIONAL       ((unsigned char*) (0x458))

void set_PET_font()
{
   struct regs fontregs;
   fontregs.a = 4; // PET-like
   fontregs.pc = 0xff62;
   _sys(&fontregs);
}

void setAttack(unsigned char voice, unsigned char level)
{
   // 11111111  =>  00000011, 11111100
   ADSR_ATTACK_FRACTIONAL[voice] = level << 2;
   ADSR_ATTACK[voice]            = level >> 6;
   cprintf("attack : %u, %u\r\n", ADSR_ATTACK[voice], ADSR_ATTACK_FRACTIONAL[voice]);
}

void setDecay(unsigned char voice, unsigned char level, unsigned char sustain_level)
{
   ADSR_DECAY_FRACTIONAL[voice] = (level << 2);
   ADSR_DECAY[voice]            = (level >> 6);
   ADSR_SUSTAIN_LEVEL[voice]    = sustain_level & 0x3f;
   cprintf("decay  : %u, %u\r\n", ADSR_DECAY[voice], ADSR_DECAY_FRACTIONAL[voice]);
}

void setSustain(unsigned char voice, unsigned char level)
{
   ADSR_SUSTAIN_TIMER_FRACTIONAL[voice] = (level << 2);
   ADSR_SUSTAIN_TIMER[voice]            = (level >> 6);
   cprintf("sustain (%u): %u, %u\r\n", ADSR_SUSTAIN_LEVEL[voice], ADSR_SUSTAIN_TIMER[voice], ADSR_SUSTAIN_TIMER_FRACTIONAL[voice]);
}

void setRelease(unsigned char voice, unsigned char level)
{
   ADSR_RELEASE_FRACTIONAL[voice] = (level << 2);
   ADSR_RELEASE[voice]            = (level >> 6);
   cprintf("release: %u, %u\r\n", ADSR_RELEASE[voice], ADSR_RELEASE_FRACTIONAL[voice]);
}


void main()
{
   struct regs myregs;
   
   set_PET_font();

   cbm_k_setnam("adsr1.prg");
   cbm_k_setlfs(0,8,0);
   LOAD_TO_MAIN_RAM(0x0400);

   myregs.pc = ADSR_INSTALLER;
   _sys(&myregs);

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

   setAttack(0, 7);
   setDecay(0, 10, 40);
   setSustain(0, 100);
   setRelease(0, 10);
   //ADSR_ATTACK[0] = 1;
   //ADSR_ATTACK_FRACTIONAL[0] = 0;
   //ADSR_DECAY_FRACTIONAL[0]  = 1;
   //ADSR_SUSTAIN_LEVEL[0] = 20;
   //ADSR_SUSTAIN_TIMER[0] = 2;
   //ADSR_SUSTAIN_TIMER_FRACTIONAL[0] = 1;
   //ADSR_RELEASE[0] = 0;
   //ADSR_RELEASE_FRACTIONAL[0] = 40;

   myregs.x = 0;     // voice
   myregs.a = 20;    // volume
   myregs.pc = ADSR_ACTIVATE_VOICE;
   _sys(&myregs);

   //sleep(5);

   myregs.pc = ADSR_HANDLER_ON;
   _sys(&myregs);

}

