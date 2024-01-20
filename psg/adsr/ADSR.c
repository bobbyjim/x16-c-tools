#include <6502.h>
#include <cbm.h>

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

void adsr_defineVoice(unsigned char voice, 
                      unsigned char freq)
{

}

void adsr_activateVoice(unsigned char voice, unsigned char volume)
{
   struct regs my_adsr_regs;
   my_adsr_regs.x = voice;
   my_adsr_regs.a = volume;
   my_adsr_regs.pc = ADSR_ACTIVATE_VOICE;
   _sys(&my_adsr_regs);
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
   ADSR_ATTACK_FRACTIONAL[voice] = level << 4;
   ADSR_ATTACK[voice]            = level >> 4;
   //cprintf("attack : %u, %u\r\n", ADSR_ATTACK[voice], ADSR_ATTACK_FRACTIONAL[voice]);
}

void adsr_setDecay(unsigned char voice, unsigned char level, unsigned char sustain_level)
{
   ADSR_DECAY_FRACTIONAL[voice] = (level << 2);
   ADSR_DECAY[voice]            = (level >> 6);
   ADSR_SUSTAIN_LEVEL[voice]    = sustain_level & 0x3f;
   //cprintf("decay  : %u, %u\r\n", ADSR_DECAY[voice], ADSR_DECAY_FRACTIONAL[voice]);
}

void adsr_setSustain(unsigned char voice, unsigned char level)
{
   ADSR_SUSTAIN_TIMER_FRACTIONAL[voice] = (level << 2);
   ADSR_SUSTAIN_TIMER[voice]            = (level >> 6);
   //cprintf("sustain (%u): %u, %u\r\n", ADSR_SUSTAIN_LEVEL[voice], ADSR_SUSTAIN_TIMER[voice], ADSR_SUSTAIN_TIMER_FRACTIONAL[voice]);
}

void adsr_setRelease(unsigned char voice, unsigned char level)
{
   ADSR_RELEASE_FRACTIONAL[voice] = (level << 2);
   ADSR_RELEASE[voice]            = (level >> 6);
   //cprintf("release: %u, %u\r\n", ADSR_RELEASE[voice], ADSR_RELEASE_FRACTIONAL[voice]);
}
