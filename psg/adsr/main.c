#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <conio.h>
#include <6502.h>
#include <cbm.h>
#include <cx16.h>

#include "ADSR.h"
#include "timer.h"

#define	TO_MAIN_RAM         0 
#define  TO_VERA_RAM        2

#define  STEREO_VOLUME(vol)            (vol + 196)
#define  ABI(x)                        ((unsigned char *)0x02[x]);

#define  PSG_WAVE_PULSE_SQUARE      63
#define  PSG_WAVE_PULSE_QUARTER     32
#define  PSG_WAVE_PULSE_EIGHTH      16
#define  PSG_WAVE_PULSE_SIXTEENTH   8
#define  PSG_WAVE_SAWTOOTH          (1 << 6)
#define  PSG_WAVE_TRIANGLE          (2 << 6)
#define  PSG_WAVE_NOISE             (3 << 6)

unsigned int tunedNotes[] = {
//   0       1     2       3      4      5          6      7      8      9      10     11
//  (C2)  , Cs2  , d2    , ds2  , e2   , f2     //  fs2  , g2   , gs2   , a2   , as2  , b2    
    0    , 186  , 197   , 208  , 221  , 234 ,      248  , 263  , 278   , 295  , 312  , 331 , // 0 - 11
//  C3   , Cs3  , d3    , ds3  , e3   , f3     //  fs3  , g3   , gs3   , a3   , as3  , b3     
    351  , 372  , 394   , 417  , 442  , 468 ,      496  , 526  , 557   , 590  , 625  , 662 , // 12 - 23
//  c4   , cs4  , d4    , ds4  , e4   , f4     //  fs4  , g4   , gs4   , a4   , as4  , b4     
    702  , 744  , 788   , 835  , 884  , 937 ,      993  , 1052 , 1114  , 1181 , 1251 , 1325, // 24 - 35
//  c5   , cs5  , d5    , ds5  , e5   , f5     //  fs5  , g5   , gs5   , a5   , as5  , b5     
    1404 , 1488 , 1576  , 1670 , 1769 , 1875,     1986 , 2104 , 2229  , 2362 , 2502 , 2651,  // 36 - 47
//  c6   , cs6  , d6    , ds6  , e6   , f6     
    2809 , 2976 , 3153  , 3340 , 3539 , 3750                                                 // 48 - 53
};

unsigned char invention13_dualVoice[] = {
//bar 0
0,11  ,  0,11  ,  0,23  ,  0,23  ,  0,11  ,  0,11  ,  0,22  ,  0,22  ,  0,18  ,  0,18  ,  0,23  ,  0,26  ,  0,25  ,  0,18  ,  0,25  ,  0,28  ,
//bar 1
0,11  , 30,11  , 35,23  , 38,23  , 37,23  , 30,23  , 37,22  , 40,22  , 38,23  , 38,18  , 42,23  , 42,26  , 34,25  , 34,18  , 42,25  , 42,28  , 
//bar 2
35,26  , 30,26  , 35,23  , 38,23  , 37,22  , 30,22  , 37,18  , 40,18  , 38,23  , 38,18  , 35,23  , 35,26  , 0,25  , 0,18  , 0,25  , 0,28  , 
//bar 3
0,26  , 42,26  , 38,23  , 42,23  , 35,26  , 38,26  , 30,23  , 33,23  , 31,28  , 31,23  , 35,19  , 35,23  , 40,16  , 40,19  , 43,11  , 43,14  , 
//bar 4
43,13  , 40,13  , 37,16  , 40,16  , 33,21  , 37,21  , 28,25  , 31,25  , 30,25  , 30,21  , 33,18  , 33,21  , 38,14  , 38,18  , 42,9  , 42,13  , 
//bar 5
42,11  , 38,11  , 35,14  , 38,14  , 31,16  , 31,19  , 40,13  , 40,16  , 40,9  , 37,9  , 33,13  , 37,13  , 30,14  , 30,18  , 38,11  , 38,14  , 
//bar 6
38,7  , 35,7  , 31,4  , 35,4  , 28,9  , 28,21  , 37,19  , 37,21  , 38,11  , 38,21  , 0,26  , 0,30  , 0,28  , 0,21  , 0,28  , 0,31  , 
//bar 7
0,30  , 33,30  , 38,26  , 42,26  , 40,25  , 33,25  , 40,21  , 43,21  , 42,26  , 42,21  , 45,26  , 45,30  , 37,28  , 37,21  , 45,28  , 45,31  , 
//bar 8
38,30  , 33,30  , 38,26  , 42,26  , 40,0  , 33,0  , 40,0  , 43,0  , 42,0  , 42,33  , 38,30  , 38,33  , 45,26  , 45,30  , 42,21  , 42,25  , 
//bar 9
50,23  , 47,23  , 42,26  , 47,26  , 38,30  , 42,30  , 35,33 /*?*/ , 38,33  , 40,32  , 40,35  , 44,28  , 44,32  , 47,23  , 47,28  , 50,20  , 50,35  , 
//bar 10
49,21  , 45,21  , 40,25  , 45,25  , 37,28  , 40,28  , 33,32 /*?*/ , 37,32  , 38,30  , 38,32  , 42,26  , 42,30  , 45,21  , 45,26  , 49,18  , 49,21  , 
//bar 11
47,20  , 44,20  , 41,23  , 44,23  , 37,25  , 40,25  , 32,29  , 35,29  , 33,0  , 33,30  , 45,26  , 45,30  , 45,23  , 42,26  , 38,30  , 42,33  , 
//bar 12
35,32  , 35,28  , 44,25  , 44,28  , 44,21  , 40,25  , 37,28  , 40,32  , 33,30  , 33,26  , 42,23  , 42,26  , 42,20  , 38,23  , 35,26  , 38,26  , 
//bar 13
32,26  , 45,25  , 44,26  , 42,25  , 41,26  , 44,26  , 37,13  , 41,13  , 42,18  , 42,30  , 0,25  , 0,21  , 0,18  , 0,13  , 0,13  , 0,13,
//bar 14
0,11   ,  0,11  ,  0,11  ,  0,9  ,  0,9  ,  0,9  ,  0,11  ,  0,11  ,  0,11  ,  0,9  , 0,9  , 0,9 ,

255, 255 // end
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
   unsigned int  loopCount = 0;
   unsigned char attack   =  2;
   unsigned char decay    =  30;
   unsigned char sustain  =  0;
   unsigned char release  =  50;

   unsigned char note0 = 0;
   unsigned char note1 = 0;
   unsigned char prev0 = 0;
   unsigned char prev1 = 0;

   set_PET_font();
   adsr_install();

   VERA.control      = 0;              // port 0
   VERA.address      = 0xf9c0;         // voice 0
   VERA.address_hi   = VERA_INC_1 + 1; // from cx16.h
   VERA.data0        = 0; // tunedNotes[19] & 0xff;    // freq 1
   VERA.data0        = 0; // tunedNotes[19] >> 8;      // freq 2
   VERA.data0        = STEREO_VOLUME(20);
   VERA.data0        = PSG_WAVE_TRIANGLE; // PSG_WAVE_PULSE_SQUARE;

   VERA.address      = 0xf9c4;         // voice 1
   VERA.address_hi   = VERA_INC_1 + 1; // from cx16.h
   VERA.data0        = 0; // tunedNotes[19] & 0xff;    // freq 1
   VERA.data0        = 0; // tunedNotes[19] >> 8;      // freq 2
   VERA.data0        = STEREO_VOLUME(20);
   VERA.data0        = PSG_WAVE_TRIANGLE;

   adsr_setAttack(    0, attack);
   adsr_setDecay(     0, decay, 42);
   adsr_setSustain(   0, sustain);
   adsr_setRelease(   0, release);

   adsr_setAttack(    1, attack);
   adsr_setDecay(     1, decay, 42);
   adsr_setSustain(   1, sustain);
   adsr_setRelease(   1, release);

   adsr_setHandler(ADSR_ON);

   for(;;) 
   {
       if (invention13_dualVoice[loopCount] == 255) loopCount = 0;

       note0 = invention13_dualVoice[loopCount];
       note1 = invention13_dualVoice[loopCount+1];

       // Voice 0: Handle note change
       if (prev0 != note0 && note0 < 54) {
          if (note0 == 0) {
             adsr_releaseVoice(0);
          } else {
             adsr_setFrequency(0, tunedNotes[note0]);
             adsr_activateVoice(0, 63);
          }
       }

       // Voice 1: Handle note change
       if (prev1 != note1 && note1 < 54) {
          if (note1 == 0) {
             adsr_releaseVoice(1);
          } else {
             adsr_setFrequency(1, tunedNotes[note1]);
             adsr_activateVoice(1, 63);
          }
       }

       prev0 = note0;
       prev1 = note1;
       loopCount += 2;

       // Wait for next note (consistent timing)
       pause_jiffies(30);
   }
}
