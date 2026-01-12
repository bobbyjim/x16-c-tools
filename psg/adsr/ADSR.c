#include <6502.h>
#include <cbm.h>
#include <cx16.h>

#include "ADSR.h"

#define	ADSR_LIBRARY_NAME	"adsr1.prg"
#define	TO_MAIN_RAM			0 

void adsr_install()
{
   struct regs my_adsr_regs;
   cbm_k_setnam(ADSR_LIBRARY_NAME);
   cbm_k_setlfs(0,8,0);
   cbm_k_load(TO_MAIN_RAM,0x0400);

   my_adsr_regs.pc = ADSR_INSTALLER;
   _sys(&my_adsr_regs);
}

void adsr_setHandler(unsigned char handlerStatus)
{
   struct regs my_adsr_regs;
   if (handlerStatus)
      my_adsr_regs.pc = ADSR_HANDLER_ON;
   else 
      my_adsr_regs.pc = ADSR_HANDLER_OFF;
   _sys(&my_adsr_regs);
}

void adsr_setFrequency(unsigned char voice, unsigned int freq)
{
   VERA.control    = 0;
   VERA.address    = 0xf9c0 + (voice * 4);
   VERA.address_hi = VERA_INC_1 + 1;
   VERA.data0      = freq & 0xff;
   VERA.data0      = freq >> 8;
}

void adsr_activateVoice(unsigned char voice, unsigned char volume)
{
   struct regs my_adsr_regs;
   my_adsr_regs.x = voice;
   my_adsr_regs.a = volume;
   my_adsr_regs.pc = ADSR_ACTIVATE_VOICE;
   _sys(&my_adsr_regs);
}

void adsr_releaseVoice(unsigned char voice)
{
   ADSR_STATE[voice] = 8; // release
}

void adsr_setState(unsigned char voice, unsigned char state)
{
	// 0 = idle
	// 2 = attack
	// 4 = decay
	// 6 = sustain
	// 8 = release
	ADSR_STATE[voice] = state; 
}

void adsr_setAttack(unsigned char voice, unsigned char level)
{
   // Because the upper 3 bits are useless, map Attack like so:
   // ADSR Attack Bits:   15 14 13 12 11 10 9 8  7 6 5 4 3 2 1 0
   // level           :             7  6  5 4 3  2 1 0
   //
   ADSR_ATTACK[voice]            =  level << 5; // use the top 3 bits
   ADSR_ATTACK_FRACTIONAL[voice] =  level >> 3; // use the bottom 5 bits
   //cprintf("attack : %u, %u\r\n", ADSR_ATTACK[voice], ADSR_ATTACK_FRACTIONAL[voice]);
}

void adsr_setDecay(unsigned char voice, unsigned char level, unsigned char sustain_level)
{
   ADSR_DECAY_FRACTIONAL[voice] = level;
   ADSR_DECAY[voice]            = 0;
   ADSR_SUSTAIN_LEVEL[voice]    = sustain_level & 0x3f;
   //cprintf("decay  : %u, %u\r\n", ADSR_DECAY[voice], ADSR_DECAY_FRACTIONAL[voice]);
}

void adsr_setSustain(unsigned char voice, unsigned char level)
{
   // Because the upper 3 bits are just too huge, map the sustain timer like so:
   // ADSR Sustain Bits:   15 14 13 12 11 10 9 8  7 6 5 4 3 2 1 0
   // level            :                  7  6 5  4 3 2 1 0
   //
   ADSR_SUSTAIN_TIMER_FRACTIONAL[voice] = level << 3; // use the top 5 bits
   ADSR_SUSTAIN_TIMER[voice]            = level >> 5; // use the bottom 3 bits
   //cprintf("sustain (%u): %u, %u\r\n", ADSR_SUSTAIN_LEVEL[voice], ADSR_SUSTAIN_TIMER[voice], ADSR_SUSTAIN_TIMER_FRACTIONAL[voice]);
}

void adsr_setRelease(unsigned char voice, unsigned char level)
{
   ADSR_RELEASE_FRACTIONAL[voice] = level;
   ADSR_RELEASE[voice]            = 0;
   //cprintf("release: %u, %u\r\n", ADSR_RELEASE[voice], ADSR_RELEASE_FRACTIONAL[voice]);
}

void adsr_setEnvelope(unsigned char voice, unsigned char attack, unsigned char decay, unsigned char sustain_level, unsigned char sustain_timer_fractional, unsigned char release)
{
   adsr_setAttack(    voice, attack);
   adsr_setDecay(     voice, decay, sustain_level);
   adsr_setSustain(   voice, sustain_timer_fractional);
   adsr_setRelease(   voice, release);
}
