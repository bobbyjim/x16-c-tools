#include <stdio.h>
#include <stdlib.h>
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

int tunedNotes[] = {
//   0       1     2       3      4      5          6      7      8      9      10     11
//  C2   , Cs2  , d2    , ds2  , e2   , f2     //  fs2  , g2   , gs2   , a2   , as2  , b2    
    175  , 186  , 197   , 208  , 221  , 234 ,      248  , 263  , 278   , 295  , 312  , 331 , // 0 - 11
//  C3   , Cs3  , d3    , ds3  , e3   , f3     //  fs3  , g3   , gs3   , a3   , as3  , b3     
    351  , 372  , 394   , 417  , 442  , 468 ,      496  , 526  , 557   , 590  , 625  , 662 , // 12 - 22
//  c4   , cs4  , d4    , ds4  , e4   , f4     //  fs4  , g4   , gs4   , a4   , as4  , b4     
    702  , 744  , 788   , 835  , 884  , 937 ,      993  , 1052 , 1114  , 1181 , 1251 , 1325, // 23 - 34
//  c5   , cs5  , d5    , ds5  , e5   , f5     //  fs5  , g5   , gs5   , a5   , as5  , b5     
    1404 , 1488 , 1576  , 1670 , 1769 , 1875,     1986 , 2104 , 2229  , 2362 , 2502 , 2651,  // 35 - 46
//  c6   , cs6  , d6    , ds6  , e6   , f6     
    2809 , 2976 , 3153  , 3340 , 3539 , 3750                                                 // 47 - 52
};

void set_PET_font()
{
   struct regs fontregs;
   fontregs.a = 4; // PET-like
   fontregs.pc = 0xff62;
   _sys(&fontregs);
}

void main()
{  
   unsigned char attack   = 200;
   unsigned char decay    = 100;
   unsigned char sustain  = 1;
   unsigned char release  = 100;
   char line[80];

   set_PET_font();
   adsr_install();

   VERA.control      = 0;              // port 0
   VERA.address      = 0xf9c0;         // voice 0
   VERA.address_hi   = VERA_INC_1 + 1; // from cx16.h
   VERA.data0        = tunedNotes[19] & 0xff;    // freq 1
   VERA.data0        = tunedNotes[19] >> 8;      // freq 2
   VERA.data0        = STEREO_VOLUME(20);
   VERA.data0        = PSG_WAVE_PULSE_SQUARE;

   for(;;) 
   {
       clrscr();
       cprintf("attack: ");
       gets(line);
       attack = atoi(line);
       cprintf("decay: ");
       gets(line);
       decay = atoi(line);
       cprintf("sustain: ");
       gets(line);
       sustain = atoi(line);
       cprintf("release: ");
       gets(line);
       release = atoi(line);
       cprintf("%u %u %u %u\r\n", attack, decay, sustain, release);
    
       adsr_setAttack(    0, attack);
       adsr_setDecay(     0, decay, 42);
       adsr_setSustain(   0, sustain);
       adsr_setRelease(   0, release);
       adsr_activateVoice(0,0);
       adsr_setHandler(ADSR_ON);
   }
}
